#include <PR/ultratypes.h>

#include "area.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "gfx_dimensions.h"
#include "main.h"
#include "memory.h"
#include "print.h"
#include "rendering_graph_node.h"
#include "shadow.h"
#include "sm64.h"
#include "types.h"
#include "level_update.h"

/**
 * This file contains the code that processes the scene graph for rendering.
 * The scene graph is responsible for drawing everything except the HUD / text boxes.
 * First the root of the scene graph is processed when geo_process_root
 * is called from level_script.c. The rest of the tree is traversed recursively
 * using the function geo_process_node_and_siblings, which switches over all
 * geo node types and calls a specialized function accordingly.
 * The types are defined in engine/graph_node.h
 *
 * The scene graph typically looks like:
 * - Root (viewport)
 *  - Master list
 *   - Ortho projection
 *    - Background (skybox)
 *  - Master list
 *   - Perspective
 *    - Camera
 *     - <area-specific display lists>
 *     - Object parent
 *      - <group with 240 object nodes>
 *  - Master list
 *   - Script node (Cannon overlay)
 *
 */

s16 gMatStackIndex;
Mat4 gMatStack[32];
Mtx *gMatStackFixed[32];

/**
 * Animation nodes have state in global variables, so this struct captures
 * the animation state so a 'context switch' can be made when rendering the
 * held object.
 */
struct GeoAnimState {
    /*0x00*/ u8 type;
    /*0x01*/ u8 enabled;
    /*0x02*/ s16 frame;
    /*0x04*/ f32 translationMultiplier;
    /*0x08*/ u16 *attribute;
    /*0x0C*/ s16 *data;
};

// For some reason, this is a GeoAnimState struct, but the current state consists
// of separate global variables. It won't match EU otherwise.
struct GeoAnimState gGeoTempState;

u8 gCurAnimType;
u8 gCurAnimEnabled;
s16 gCurrAnimFrame;
f32 gCurAnimTranslationMultiplier;
u16 *gCurrAnimAttribute;
s16 *gCurAnimData;

struct AllocOnlyPool *gDisplayListHeap;

struct RenderModeContainer {
    u32 modes[8];
};

/* Rendermode settings for cycle 1 for all 8 layers. */
struct RenderModeContainer renderModeTable_1Cycle[2] = { { {
                                                             G_RM_OPA_SURF,
                                                             G_RM_OPA_SURF,
                                                             G_RM_OPA_SURF,
                                                             G_RM_OPA_SURF,
                                                             G_RM_TEX_EDGE,
                                                             G_RM_XLU_SURF,
                                                             G_RM_XLU_SURF,
                                                             G_RM_XLU_SURF,
                                                         } },
                                                         { {
                                                             /* z-buffered */
                                                             G_RM_OPA_SURF,
                                                             G_RM_RA_ZB_OPA_SURF,
                                                             G_RM_ZB_OPA_DECAL,
                                                             G_RM_RA_ZB_OPA_INTER,
                                                             G_RM_AA_ZB_TEX_EDGE,
                                                             G_RM_ZB_XLU_SURF,
                                                             G_RM_AA_ZB_XLU_DECAL,
                                                             G_RM_AA_ZB_XLU_INTER,
                                                         } } };

/* Rendermode settings for cycle 2 for all 8 layers. */
struct RenderModeContainer renderModeTable_2Cycle[2] = { { {
                                                             G_RM_OPA_SURF2,
                                                             G_RM_OPA_SURF2,
                                                             G_RM_OPA_SURF2,
                                                             G_RM_OPA_SURF2,
                                                             G_RM_TEX_EDGE2,
                                                             G_RM_XLU_SURF2,
                                                             G_RM_XLU_SURF2,
                                                             G_RM_XLU_SURF2,
                                                         } },
                                                         { {
                                                             /* z-buffered */
                                                             G_RM_OPA_SURF2,
                                                             G_RM_ZB_OPA_SURF2,
                                                             G_RM_ZB_OPA_DECAL2,
                                                             G_RM_RA_ZB_OPA_INTER2,
                                                             G_RM_AA_ZB_TEX_EDGE2,
                                                             G_RM_ZB_XLU_SURF2,
                                                             G_RM_AA_ZB_XLU_DECAL2,
                                                             G_RM_AA_ZB_XLU_INTER2,
                                                         } } };

struct GraphNodeRoot *gCurGraphNodeRoot = NULL;
struct GraphNodeMasterList *gCurGraphNodeMasterList = NULL;
struct GraphNodePerspective *gCurGraphNodeCamFrustum = NULL;
struct GraphNodeCamera *gCurGraphNodeCamera = NULL;
struct GraphNodeObject *gCurGraphNodeObject = NULL;
struct GraphNodeHeldObject *gCurGraphNodeHeldObject = NULL;
u16 gAreaUpdateCounter = 0;

#ifdef F3DEX_GBI_2
LookAt lookAt;
#endif

/**
 * Processes the children of the given GraphNode if it has any
 */
void geo_try_process_children(const struct GraphNode *node) {
    if (node->children != NULL) {
        geo_process_node_and_siblings(node->children);
    }
}

void incrementMatStack() {
    Mtx *mtx = alloc_display_list(sizeof(*mtx));
    gMatStackIndex++;
    mtxf_to_mtx(mtx, gMatStack[gMatStackIndex]);
    gMatStackFixed[gMatStackIndex] = mtx;
}
void appendDLandReturn(const struct GraphNodeDisplayList *node) {
    if (node->displayList != NULL) {
        geo_append_display_list(node->displayList, node->node.flags >> 8);
    }
    if (((struct GraphNodeRoot *) node)->node.children != NULL) {
        geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
    }
    /*if (node->node.children != NULL) {
        geo_process_node_and_siblings(node->node.children);
    }*/
    gMatStackIndex--;
}
/**
 * Process a master list node.
 */
void geo_process_master_list_sub(const struct GraphNodeMasterList *node) {
    struct DisplayListNode *currList;
    s32 i;
    const s32 enableZBuffer = (node->node.flags & GRAPH_RENDER_Z_BUFFER) != 0;
    const struct RenderModeContainer *modeList = &renderModeTable_1Cycle[enableZBuffer];
    const struct RenderModeContainer *mode2List = &renderModeTable_2Cycle[enableZBuffer];

    // @bug This is where the LookAt values should be calculated but aren't.
    // As a result, environment mapping is broken on Fast3DEX2 without the
    // changes below.
#ifdef F3DEX_GBI_2
    Mtx lMtx;
    guLookAtReflect(&lMtx, &lookAt, 0, 0, 0, /* eye */ 0, 0, 1, /* at */ 1, 0, 0 /* up */);
#endif

    if (enableZBuffer != 0) {
        gDPPipeSync(gDisplayListHead++);
        gSPSetGeometryMode(gDisplayListHead++, G_ZBUFFER);
    }

    for (i = 0; i < GFX_NUM_MASTER_LISTS; i++) {
        if ((currList = node->listHeads[i]) != NULL) {
            gDPSetRenderMode(gDisplayListHead++, modeList->modes[i], mode2List->modes[i]);
            while (currList != NULL) {
                gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(currList->transform),
                          G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPDisplayList(gDisplayListHead++, currList->displayList);
                currList = currList->next;
            }
        }
    }
    if (enableZBuffer != 0) {
        gDPPipeSync(gDisplayListHead++);
        gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER);
    }
}

/**
 * Appends the display list to one of the master lists based on the layer
 * parameter. Look at the RenderModeContainer struct to see the corresponding
 * render modes of layers.
 */
void geo_append_display_list(void *displayList, const s32 layer) {
    if (gCurGraphNodeMasterList != 0) {
        struct DisplayListNode *listNode =
            alloc_only_pool_alloc(gDisplayListHeap, sizeof(struct DisplayListNode));
        gSPLookAt(gDisplayListHead++, &lookAt);

        listNode->transform = gMatStackFixed[gMatStackIndex];
        listNode->displayList = displayList;
        listNode->next = 0;
        if (gCurGraphNodeMasterList->listHeads[layer]) {
            gCurGraphNodeMasterList->listTails[layer]->next = listNode;
        } else {
            gCurGraphNodeMasterList->listHeads[layer] = listNode;
        }
        gCurGraphNodeMasterList->listTails[layer] = listNode;
    }
}

/**
 * Process the master list node.
 */
void geo_process_master_list(struct GraphNodeMasterList *node) {
    s32 i;

    if (gCurGraphNodeMasterList == NULL && node->node.children != NULL) {
        gCurGraphNodeMasterList = node;
        for (i = 0; i < GFX_NUM_MASTER_LISTS; i++) {
            node->listHeads[i] = NULL;
        }
        geo_process_node_and_siblings(node->node.children);
        geo_process_master_list_sub(node);
        gCurGraphNodeMasterList = NULL;
    }
}

/**
 * Process an orthographic projection node.
 */
void geo_process_ortho_projection(const struct GraphNodeOrthoProjection *node) {
    if (node->node.children != NULL) {
        Mtx *mtx = alloc_display_list(sizeof(*mtx));
        const f32 left = (gCurGraphNodeRoot->x - gCurGraphNodeRoot->width) * .5f * node->scale;
        const f32 right = (gCurGraphNodeRoot->x + gCurGraphNodeRoot->width) * .5f * node->scale;
        const f32 top = (gCurGraphNodeRoot->y - gCurGraphNodeRoot->height) * .5f * node->scale;
        const f32 bottom = (gCurGraphNodeRoot->y + gCurGraphNodeRoot->height) * .5f * node->scale;

        guOrtho(mtx, left, right, bottom, top, -2.0f, 2.0f, 1.0f);
        gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx),
                  G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

        geo_process_node_and_siblings(node->node.children);
    }
}

/**
 * Process a perspective projection node.
 */
void geo_process_perspective(const struct GraphNodePerspective *node) {
    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    if (node->fnNode.node.children != NULL) {
        u16 perspNorm;
        Mtx *mtx = alloc_display_list(sizeof(*mtx));

        f32 aspect = (f32) gCurGraphNodeRoot->width / (f32) gCurGraphNodeRoot->height;
        // guPerspective(mtx, &perspNorm, 45.0f, 1.33333f, 100.0f, 30000.0f, 1.0f);
        guPerspective(mtx, &perspNorm, node->fov, aspect, node->near, node->far, 1.0f);
        gSPPerspNormalize(gDisplayListHead++, perspNorm);

        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx),
                  G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

        gCurGraphNodeCamFrustum = node;
        geo_process_node_and_siblings(node->fnNode.node.children);
        gCurGraphNodeCamFrustum = NULL;
    }
}

/**
 * Process a level of detail node. From the current transformation matrix,
 * the perpendicular distance to the camera is extracted and the children
 * of this node are only processed if that distance is within the render
 * range of this node.
 */
void geo_process_level_of_detail(const struct GraphNodeLevelOfDetail *node) {
    // The fixed point Mtx type is defined as 16 longs, but it's actually 16
    // shorts for the integer parts followed by 16 shorts for the fraction parts
    const s16 *mtx = (s16 *) gMatStackFixed[gMatStackIndex];
    const s16 distanceFromCam = -mtx[14]; // z-component of the translation column

    if (node->minDistance <= distanceFromCam && distanceFromCam < node->maxDistance) {
        if (((struct GraphNodeRoot *) node)->node.children != NULL) {
            geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
        }
    }
}

/**
 * Process a switch case node. The node's selection function is called
 * if it is 0, and among the node's children, only the selected child is
 * processed next.
 */
void geo_process_switch(const struct GraphNodeSwitchCase *node) {
    struct GraphNode *selectedChild = node->fnNode.node.children;
    s32 i;

    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    for (i = 0; selectedChild != NULL && node->selectedCase > i; i++) {
        selectedChild = selectedChild->next;
    }
    if (selectedChild != NULL) {
        geo_process_node_and_siblings(selectedChild);
    }
}
u8 renderGame;
/**
 * Process a camera node.
 */
extern f32 billboardMatrix[3][4];
void geo_process_camera(struct GraphNodeCamera *node) {
    Mat4 cameraTransform;
    const Mtx *rollMtx = alloc_display_list(sizeof(*rollMtx));

    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    mtxf_rotate_xy(rollMtx, node->rollScreen);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(rollMtx),
              G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);

    mtxf_lookat(cameraTransform, node->pos, node->focus, node->roll);
    mtxf_mul(gMatStack[gMatStackIndex + 1], cameraTransform, gMatStack[gMatStackIndex]);
    incrementMatStack();
    if (node->fnNode.node.children != 0) {
        renderGame = 1;
        gCurGraphNodeCamera = node;

        billboardMatrix[0][0] = coss(gCurGraphNodeCamera->roll);
        billboardMatrix[0][1] = sins(gCurGraphNodeCamera->roll);
        billboardMatrix[1][0] = -billboardMatrix[0][1];
        billboardMatrix[1][1] = billboardMatrix[0][0];
        node->matrixPtr = gMatStack[gMatStackIndex];
        geo_process_node_and_siblings(node->fnNode.node.children);
        gCurGraphNodeCamera = NULL;
    } else {
        renderGame = 0;
    }
    gMatStackIndex--;
}

void mtxf_rot_trans_mul(Vec3s rot, Vec3f trans, Mat4 dest, Mat4 src) {
    register f32 sx = sins(rot[0]);
    register f32 cx = coss(rot[0]);

    register f32 sy = sins(rot[1]);
    register f32 cy = coss(rot[1]);

    register f32 sz = sins(rot[2]);
    register f32 cz = coss(rot[2]);
    register f32 entry0;
    register f32 entry1;
    register f32 entry2;

    entry0 = cy * cz;
    entry1 = cy * sz;
    entry2 = -sy;
    dest[0][0] = entry0 * src[0][0] + entry1 * src[1][0] + entry2 * src[2][0];
    dest[0][1] = entry0 * src[0][1] + entry1 * src[1][1] + entry2 * src[2][1];
    dest[0][2] = entry0 * src[0][2] + entry1 * src[1][2] + entry2 * src[2][2];

    entry1 = sx * sy;
    entry0 = entry1 * cz - cx * sz;
    entry1 = entry1 * sz + cx * cz;
    entry2 = sx * cy;
    dest[1][0] = entry0 * src[0][0] + entry1 * src[1][0] + entry2 * src[2][0];
    dest[1][1] = entry0 * src[0][1] + entry1 * src[1][1] + entry2 * src[2][1];
    dest[1][2] = entry0 * src[0][2] + entry1 * src[1][2] + entry2 * src[2][2];

    entry1 = cx * sy;
    entry0 = entry1 * cz + sx * sz;
    entry1 = entry1 * sz - sx * cz;
    entry2 = cx * cy;
    dest[2][0] = entry0 * src[0][0] + entry1 * src[1][0] + entry2 * src[2][0];
    dest[2][1] = entry0 * src[0][1] + entry1 * src[1][1] + entry2 * src[2][1];
    dest[2][2] = entry0 * src[0][2] + entry1 * src[1][2] + entry2 * src[2][2];

    entry0 = trans[0];
    entry1 = trans[1];
    entry2 = trans[2];
    dest[3][0] = entry0 * src[0][0] + entry1 * src[1][0] + entry2 * src[2][0] + src[3][0];
    dest[3][1] = entry0 * src[0][1] + entry1 * src[1][1] + entry2 * src[2][1] + src[3][1];
    dest[3][2] = entry0 * src[0][2] + entry1 * src[1][2] + entry2 * src[2][2] + src[3][2];
    dest[0][3] = dest[1][3] = dest[2][3] = 0;
    ((u32 *) dest)[15] = 0x3F800000;
}
/**
 * Process a translation / rotation node. A transformation matrix based
 * on the node's translation and rotation is created and pushed on both
 * the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_translation_rotation(const struct GraphNodeTranslationRotation *node) {
    const Mat4 mtxf;
    const Vec3f translation;

    vec3s_to_vec3f(translation, node->translation);
    mtxf_rotate_zxy_and_translate(mtxf, translation, node->rotation);
    mtxf_mul(gMatStack[gMatStackIndex + 1], mtxf, gMatStack[gMatStackIndex]);
    incrementMatStack();
    appendDLandReturn(node);
}

/**
 * Process a translation node. A transformation matrix based on the node's
 * translation is created and pushed on both the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_translation(const struct GraphNodeTranslation *node) {
    register f32 entry0;
    register f32 entry1;
    register f32 entry2;
    f32 entires[3];
    register s32 i;
    for (i = 0; i < 3; i++) {
        gMatStack[gMatStackIndex + 1][0][i] = gMatStack[gMatStackIndex][0][i];
        gMatStack[gMatStackIndex + 1][1][i] = gMatStack[gMatStackIndex][1][i];
        gMatStack[gMatStackIndex + 1][2][i] = gMatStack[gMatStackIndex][2][i];
        entires[i] = node->translation[i];
    }
    for (i = 0; i < 3; i++) {
        gMatStack[gMatStackIndex + 1][3][i] =
            entires[0] * gMatStack[gMatStackIndex][0][i] + entires[1] * gMatStack[gMatStackIndex][1][i]
            + entires[2] * gMatStack[gMatStackIndex][2][i] + gMatStack[gMatStackIndex][3][i];
    }

    /*gMatStack[gMatStackIndex + 1][0][0] = gMatStack[gMatStackIndex][0][0];
    gMatStack[gMatStackIndex + 1][0][1] = gMatStack[gMatStackIndex][0][1];
    gMatStack[gMatStackIndex + 1][0][2] = gMatStack[gMatStackIndex][0][2];

    gMatStack[gMatStackIndex + 1][1][0] = gMatStack[gMatStackIndex][1][0];
    gMatStack[gMatStackIndex + 1][1][1] = gMatStack[gMatStackIndex][1][1];
    gMatStack[gMatStackIndex + 1][1][2] = gMatStack[gMatStackIndex][1][2];

    gMatStack[gMatStackIndex + 1][2][0] = gMatStack[gMatStackIndex][2][0];
    gMatStack[gMatStackIndex + 1][2][1] = gMatStack[gMatStackIndex][2][1];
    gMatStack[gMatStackIndex + 1][2][2] = gMatStack[gMatStackIndex][2][2];*/

    /* entry0 = node->translation[0];
     entry1 = node->translation[1];
     entry2 = node->translation[2];
     gMatStack[gMatStackIndex + 1][3][0] =
         entry0 * gMatStack[gMatStackIndex][0][0] + entry1 * gMatStack[gMatStackIndex][1][0]
         + entry2 * gMatStack[gMatStackIndex][2][0] + gMatStack[gMatStackIndex][3][0];
     gMatStack[gMatStackIndex + 1][3][1] =
         entry0 * gMatStack[gMatStackIndex][0][1] + entry1 * gMatStack[gMatStackIndex][1][1]
         + entry2 * gMatStack[gMatStackIndex][2][1] + gMatStack[gMatStackIndex][3][1];
     gMatStack[gMatStackIndex + 1][3][2] =
         entry0 * gMatStack[gMatStackIndex][0][2] + entry1 * gMatStack[gMatStackIndex][1][2]
         + entry2 * gMatStack[gMatStackIndex][2][2] + gMatStack[gMatStackIndex][3][2];*/

    gMatStack[gMatStackIndex + 1][0][3] = gMatStack[gMatStackIndex + 1][1][3] =
        gMatStack[gMatStackIndex + 1][2][3] = 0;
    ((u32 *) gMatStack[gMatStackIndex + 1])[15] = 0x3F800000;

    incrementMatStack();
    appendDLandReturn(node);
}

/**
 * Process a rotation node. A transformation matrix based on the node's
 * rotation is created and pushed on both the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_rotation(const struct GraphNodeRotation *node) {
    const Mat4 mtxf;

    mtxf_rotate_zxy_and_translate(mtxf, gVec3fZero, node->rotation);
    mtxf_mul(gMatStack[gMatStackIndex + 1], mtxf, gMatStack[gMatStackIndex]);

    incrementMatStack();
    appendDLandReturn(node);
}

/**
 * Process a scaling node. A transformation matrix based on the node's
 * scale is created and pushed on both the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_scale(const struct GraphNodeScale *node) {
    Vec3f scaleVec;

    // node->deltaCurrentScale = MAX(node->scale, 0.01f);
    vec3f_set(scaleVec, node->scale, node->scale, node->scale);
    mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex], scaleVec);
    incrementMatStack();
    appendDLandReturn(node);
}

/**
 * Process a billboard node. A transformation matrix is created that makes its
 * children face the camera, and it is pushed on the floating point and fixed
 * point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_billboard(const struct GraphNodeBillboard *node) {
    Vec3f translation;

    vec3s_to_vec3f(translation, node->translation);
    mtxf_billboard(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex], translation,
                   gCurGraphNodeCamera->roll);
    if (gCurGraphNodeHeldObject != NULL) {
        mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex + 1],
                         gCurGraphNodeHeldObject->objNode->header.gfx.scale);
    } else if (gCurGraphNodeObject != NULL) {
        mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex + 1],
                         gCurGraphNodeObject->scale);
    }

    incrementMatStack();
    appendDLandReturn(node);
}

/**
 * Process a display list node. It draws a display list without first pushing
 * a transformation on the stack, so all transformations are inherited from the
 * parent node. It processes its children if it has them.
 */
void geo_process_display_list(const struct GraphNodeDisplayList *node) {

    appendDLandReturn(node);
    gMatStackIndex++;
}

/**
 * Process a generated list. Instead of storing a pointer to a display list,
 * the list is generated on the fly by a function.
 */
void geo_process_generated_list(const struct GraphNodeGenerated *node) {
    if (node->fnNode.func != NULL) {
        Gfx *list = node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node,
                                      (struct AllocOnlyPool *) gMatStack[gMatStackIndex]);

        if (list != 0) {
            geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(list), node->fnNode.node.flags >> 8);
        }
    }
    if (((struct GraphNodeRoot *) node)->node.children != NULL) {
        geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
    }
}

extern struct CutsceneJump *currentScene;
/**
 * Process a background node. Tries to retrieve a background display list from
 * the function of the node. If that function is null or returns null, a black
 * rectangle is drawn instead.
 */
extern s16 SCREEN_WIDTH_MATH;
extern s16 SCREEN_HEIGHT_MATH;
extern s16 BORDER_HEIGHT_MATH;
extern s16 BORDER_WIDTH_MATH;
extern s16 sCUpCameraPitch;
extern s16 newcam_pitch; // Y axis rotation

extern u16 viewportClipSize;
void geo_process_background(struct GraphNodeBackground *node) {
    return;
}

u8 animChanged = 0;
u8 curanimStackPos = 0;
f32 *curanimPosStack;
s16 *curanimRotStack;
s32 approach_angle(s32 current, s32 target);
s32 approach_angleF(s32 current, s32 target);
f32 approach_pos(f32 current, f32 target);
f32 approach_posF(f32 current, f32 target);
/**
 * Render an animated part. The current animation state is not part of the node
 * but set in global variables. If an animated part is skipped, everything afterwards desyncs.
 */
void geo_process_animated_part(const struct GraphNodeBone *node) {
    Vec3f translation;
    vec3f_set(translation, node->translation[0], node->translation[1], node->translation[2]);
    if (gCurAnimType) {
        if (gCurAnimType != ANIM_TYPE_ROTATION) {
            if (gCurAnimType == ANIM_TYPE_TRANSLATION) {
                translation[0] +=
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                    * gCurAnimTranslationMultiplier;
                translation[1] +=
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                    * gCurAnimTranslationMultiplier;
                translation[2] +=
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                    * gCurAnimTranslationMultiplier;
            } else {
                if (gCurAnimType == ANIM_TYPE_LATERAL_TRANSLATION) {
                    translation[0] +=
                        gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                        * gCurAnimTranslationMultiplier;
                    gCurrAnimAttribute += 2;
                    translation[2] +=
                        gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                        * gCurAnimTranslationMultiplier;
                } else {
                    if (gCurAnimType == ANIM_TYPE_VERTICAL_TRANSLATION) {
                        gCurrAnimAttribute += 2;
                        translation[1] +=
                            gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                            * gCurAnimTranslationMultiplier;
                        gCurrAnimAttribute += 2;
                    } else if (gCurAnimType == ANIM_TYPE_NO_TRANSLATION) {
                        gCurrAnimAttribute += 6;
                    }
                }
            }

            if (animChanged || (gMoveSpeed == 0)) {
                curanimPosStack[0 + 0] = translation[0];
                curanimPosStack[0 + 1] = translation[1];
                curanimPosStack[0 + 2] = translation[2];
            } else {
                if (gMoveSpeed == 1) {
                    curanimPosStack[0 + 0] = approach_pos(curanimPosStack[0 + 0], translation[0]);
                    curanimPosStack[0 + 1] = approach_pos(curanimPosStack[0 + 1], translation[1]);
                    curanimPosStack[0 + 2] = approach_pos(curanimPosStack[0 + 2], translation[2]);
                } else {
                    curanimPosStack[0 + 0] = approach_posF(curanimPosStack[0 + 0], translation[0]);
                    curanimPosStack[0 + 1] = approach_posF(curanimPosStack[0 + 1], translation[1]);
                    curanimPosStack[0 + 2] = approach_posF(curanimPosStack[0 + 2], translation[2]);
                }
            }
    vec3f_set(translation, curanimPosStack[0], curanimPosStack[1], curanimPosStack[2]);
        }
        gCurAnimType = ANIM_TYPE_ROTATION;
        if (animChanged || (gMoveSpeed == 0)) {
            curanimRotStack[curanimStackPos + 0] =
                gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)];
            curanimRotStack[curanimStackPos + 1] =
                gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)];
            curanimRotStack[curanimStackPos + 2] =
                gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)];
        } else {
            if (gMoveSpeed == 1) {
                curanimRotStack[curanimStackPos + 0] = approach_angle(
                    curanimRotStack[curanimStackPos + 0],
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]);
                curanimRotStack[curanimStackPos + 1] = approach_angle(
                    curanimRotStack[curanimStackPos + 1],
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]);
                curanimRotStack[curanimStackPos + 2] = approach_angle(
                    curanimRotStack[curanimStackPos + 2],
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]);
            } else {
                curanimRotStack[curanimStackPos + 0] = approach_angleF(
                    curanimRotStack[curanimStackPos + 0],
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]);
                curanimRotStack[curanimStackPos + 1] = approach_angleF(
                    curanimRotStack[curanimStackPos + 1],
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]);
                curanimRotStack[curanimStackPos + 2] = approach_angleF(
                    curanimRotStack[curanimStackPos + 2],
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]);
            }
        }
    }
        mtxf_rot_trans_mul(&curanimRotStack[curanimStackPos], translation,
                           gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex]);
    curanimStackPos += 3;
    incrementMatStack();
    appendDLandReturn(node);
}

/**
 * Initialize the animation-related global variables for the currently drawn
 * object's animation.
 */
/*
f32 *curanimPosStack;
s16 *curanimRotStack;*/
void geo_set_animation_globals(struct GraphNodeObject_sub *node, const s32 wasRenderedLastFrame) {
    const struct Animation *anim = node->curAnim;
    animChanged = (node->curAnim != node->prevAnim) || (!wasRenderedLastFrame);
    node->prevAnim = node->curAnim;
    node->animTimer = gAreaUpdateCounter;
    if (anim->flags & ANIM_FLAG_HOR_TRANS) {
        gCurAnimType = ANIM_TYPE_VERTICAL_TRANSLATION;
    } else if (anim->flags & ANIM_FLAG_VERT_TRANS) {
        gCurAnimType = ANIM_TYPE_LATERAL_TRANSLATION;
    } else if (anim->flags & ANIM_FLAG_6) {
        gCurAnimType = ANIM_TYPE_NO_TRANSLATION;
    } else {
        gCurAnimType = ANIM_TYPE_TRANSLATION;
    }

    gCurrAnimFrame = node->animFrame;
    gCurAnimEnabled = (anim->flags & ANIM_FLAG_5) == 0;
    gCurrAnimAttribute = segmented_to_virtual((void *) anim->index);
    gCurAnimData = segmented_to_virtual((void *) anim->values);

    if (anim->unk02 == 0) {
        gCurAnimTranslationMultiplier = 1.0f;
    } else {
        gCurAnimTranslationMultiplier = (f32) node->animYTrans / (f32) anim->unk02;
    }
}
extern void mtxf_align_terrain_shadow(Mat4 dest, Vec3f upDir, Vec3f pos, f32 scale, s16 yaw);
/**
 * Process a shadow node. Renders a shadow under an object offset by the
 * translation of the first animated component and rotated according to
 * the floor below it.
 */
extern const Gfx fullCircle[];
extern const Gfx fullSquare[];
void geo_process_shadow(const struct GraphNodeShadow *node) {
    Mat4 mtxf;
    Vec3f shadowPos;
    Vec3f animOffset;
    f32 objScale;
    f32 shadowScale;
    f32 sinAng;
    f32 cosAng;
    f32 distScale;
    struct GraphNode *geo;
    Gfx *displayList = alloc_display_list(2 * sizeof(Gfx));
    Gfx *head = displayList;

    if (gCurGraphNodeCamera != NULL && gCurGraphNodeObject != NULL
        && (((struct Object *) gCurGraphNodeObject)->oFloorPointer)
        && (SURFACETYPE(((struct Object *) gCurGraphNodeObject)->oFloorPointer) != 0x0A)) {
        if (gCurGraphNodeHeldObject != NULL) {
            get_pos_from_transform_mtx(shadowPos, gMatStack[gMatStackIndex],
                                       gCurGraphNodeCamera->matrixPtr);
            shadowScale = node->shadowScale;
        } else {
            vec3f_copy(shadowPos, gCurGraphNodeObject->deltaCurrentPos);
            shadowPos[1] = ((struct Object *) gCurGraphNodeObject)->oFloorHeight;
            shadowScale = node->shadowScale * gCurGraphNodeObject->deltaCurrentScale[0];
        }

        objScale = 1.0f;
        if (gCurAnimEnabled != 0) {
            if (gCurAnimType == ANIM_TYPE_TRANSLATION
                || gCurAnimType == ANIM_TYPE_LATERAL_TRANSLATION) {
                geo = node->node.children;
                if (geo != NULL && geo->type == GRAPH_NODE_TYPE_SCALE) {
                    objScale = ((struct GraphNodeScale *) geo)->deltaCurrentScale;
                }
                animOffset[0] =
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                    * gCurAnimTranslationMultiplier * objScale;
                animOffset[1] = 0.0f;
                gCurrAnimAttribute += 2;
                animOffset[2] =
                    gCurAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                    * gCurAnimTranslationMultiplier * objScale;
                gCurrAnimAttribute -= 6;
                sinAng = sins(gCurGraphNodeObject->angle[1]);
                cosAng = coss(gCurGraphNodeObject->angle[1]);

                shadowPos[0] += animOffset[0] * cosAng + animOffset[2] * sinAng;
                shadowPos[2] += -animOffset[0] * sinAng + animOffset[2] * cosAng;
            }
        }
#define SHADOWMAXHEIGHT (2500.f)
        distScale = ((((struct Object *) gCurGraphNodeObject)->oFloorHeight + SHADOWMAXHEIGHT
                      - ((struct Object *) gCurGraphNodeObject)->oPosY)
                     / SHADOWMAXHEIGHT);
        if (distScale > 0) {
            distScale = sqrtf(distScale);
            switch (node->shadowType) {
                case SHADOW_CIRCLE_9_VERTS:
                case SHADOW_CIRCLE_4_VERTS:
                case SHADOW_CIRCLE_PLAYER:
                    gDPSetPrimColor(head++, 0, 0, 0, 0, 0, node->shadowSolidity * distScale);
                    gSPBranchList(head++, fullCircle);
                    break;
                case SHADOW_SQUARE_PERMANENT:
                case SHADOW_SQUARE_SCALABLE:
                case SHADOW_SQUARE_TOGGLABLE:
                default:
                    gDPSetPrimColor(head++, 0, 0, 0, 0, 0, node->shadowSolidity * distScale);
                    gSPBranchList(head++, fullSquare);
                    break;
            }
            mtxf_align_terrain_shadow(
                mtxf, &((struct Object *) gCurGraphNodeObject)->oFloorPointer->normal.x, shadowPos,
                shadowScale / (200.f) * distScale, gCurGraphNodeObject->angle[1]);
            mtxf_mul(gMatStack[gMatStackIndex + 1], mtxf, gCurGraphNodeCamera->matrixPtr);
            incrementMatStack();
            geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(displayList), 6);
            gMatStackIndex--;
        }
    }
    if (((struct GraphNodeRoot *) node)->node.children != NULL) {
        geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
    }
}

/**
 * Check whether an object is in view to determine whether it should be drawn.
 * This is known as frustum culling.
 * It checks whether the object is far away, very close / behind the camera,
 * or horizontally out of view. It does not check whether it is vertically
 * out of view. It assumes a sphere of 300 units around the object's position
 * unless the object has a culling radius node that specifies otherwise.
 *
 * The matrix parameter should be the top of the matrix stack, which is the
 * object's transformation matrix times the camera 'look-at' matrix. The math
 * is counter-intuitive, but it checks column 3 (translation vector) of this
 * matrix to determine where the origin (0,0,0) in object space will be once
 * transformed to camera space (x+ = right, y+ = up, z = 'coming out the screen').
 * In 3D graphics, you typically model the world as being moved in front of a
 * camera instead of a moving camera through a world, which in
 * this case simplifies calculations. Note that the perspective matrix is not
 * on the matrix stack, so there are still calculations with the fov to compute
 * the slope of the lines of the frustum.
 *
 *        z-
 *
 *  \     |     /
 *   \    |    /
 *    \   |   /
 *     \  |  /
 *      \ | /
 *       \|/
 *        C       x+
 *
 * Since (0,0,0) is unaffected by rotation, columns 0, 1 and 2 are ignored.
 */
#define ASPECTRATIO (4.f / 4.f)
extern u8 wisdeScreenMode;
int obj_is_in_view(const struct GraphNodeObject *node, const Mat4 matrix) {
    s32 cullingRadius;
    s32 halfFovVertical; // half of the fov in in-game angle units instead of degrees
    struct GraphNode *geo;
    f32 vScreenEdge;

    if (node->node.flags & GRAPH_RENDER_INVISIBLE) {
        return FALSE;
    }

    geo = node->sharedChild;
    halfFovVertical = (gCurGraphNodeCamFrustum->fov + 2.0f) * 91.0222222222f + 0.5f;
    vScreenEdge = -matrix[3][2] * sins(halfFovVertical) / coss(halfFovVertical);
    if (wisdeScreenMode) {
        vScreenEdge *= (2.f);
    }
    // -matrix[3][2] is the depth, which gets multiplied by tan(halfFov) to get
    // the amount of units between the center of the screen and the horizontal edge
    // given the distance from the object to the camera.

    if (geo != NULL && geo->type == GRAPH_NODE_TYPE_CULLING_RADIUS) {
        cullingRadius =
            ((struct GraphNodeCullingRadius *) geo)->cullingRadius; //! Why is there a f32 cast?
    } else {
        cullingRadius = 300;
    }

    // Don't render if the object is close to or behind the camera
    if (matrix[3][2] > -100.0f + cullingRadius) {
        return FALSE;
    }
    //! This makes the HOLP not update when the camera is far away, and it
    //  makes PU travel safe when the camera is locked on the main map.
    //  If Mario were rendered with a depth over 65536 it would cause overflow
    //  when converting the transformation matrix to a fixed point matrix.
    if (matrix[3][2] < -20000.0f - cullingRadius) {
        return FALSE;
    }

    if (absf(matrix[3][0]) > vScreenEdge * ASPECTRATIO + cullingRadius) {
        return FALSE;
    }
    if (absf(matrix[3][1]) > vScreenEdge + cullingRadius) {
        return FALSE;
    }

    return TRUE;
}

void interpolate_node(struct Object *node) {
    register s32 i;
    for (i = 0; i < 3; i++) {
        node->header.gfx.deltaCurrentPos[i] =
            approach_pos(node->header.gfx.deltaCurrentPos[i], node->header.gfx.pos[i]);
        node->header.gfx.deltaCurrentScale[i] =
            approach_pos(node->header.gfx.deltaCurrentScale[i], node->header.gfx.scale[i]);
        node->header.gfx.deltaCurrentRot[i] =
            approach_angle(node->header.gfx.deltaCurrentRot[i], node->header.gfx.angle[i]);
    }
}

void interpolate_nodeF(struct Object *node) {
    register s32 i;
    for (i = 0; i < 3; i++) {
        node->header.gfx.deltaCurrentPos[i] = node->header.gfx.pos[i] + ((f32 *) &node->oVelX)[i];
        node->header.gfx.deltaCurrentScale[i] =
            approach_posF(node->header.gfx.deltaCurrentScale[i], node->header.gfx.scale[i]);
        node->header.gfx.deltaCurrentRot[i] =
            approach_angleF(node->header.gfx.deltaCurrentRot[i], node->header.gfx.angle[i]);
    }
}

/**
 * Process an object node.
 */
void geo_process_object(struct Object *node) {
    Mat4 mtxf;

    if ((node->header.gfx.sharedChild != NULL) && (node->header.gfx.node.flags & GRAPH_RENDER_ACTIVE)) {
        if ((node->header.gfx.firstBit) && gMoveSpeed) {
            if (gMoveSpeed == 1) {
                interpolate_node(node);
            } else {
                interpolate_nodeF(node);
            }
        } else {
            warp_node(node);
        }
        if (node->header.gfx.throwMatrix) {
            mtxf_mul(gMatStack[gMatStackIndex + 1], node->header.gfx.throwMatrix,
                     gMatStack[gMatStackIndex]);
        } else if (node->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD) {
            mtxf_billboard(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex],
                           node->header.gfx.deltaCurrentPos, gCurGraphNodeCamera->roll);
        } else {
            mtxf_rotate_zxy_and_translate(mtxf, node->header.gfx.deltaCurrentPos,
                                          node->header.gfx.deltaCurrentRot);
            mtxf_mul(gMatStack[gMatStackIndex + 1], mtxf, gMatStack[gMatStackIndex]);
        }

        mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex + 1],
                         node->header.gfx.deltaCurrentScale);
        node->header.gfx.throwMatrix = gMatStack[++gMatStackIndex];
        vec3f_copy(&node->header.gfx.cameraToObject[0], &gMatStack[gMatStackIndex][3][0]);
        if (obj_is_in_view(&node->header.gfx, gMatStack[gMatStackIndex])) {

            // FIXME: correct types
            if (node->header.gfx.unk38.curAnim != NULL) {
                geo_set_animation_globals(&node->header.gfx.unk38, node->header.gfx.firstBit);
            }
            curanimPosStack = node->header.gfx.unk38.animPosStack;
            curanimRotStack = node->header.gfx.unk38.animRotStack;
            curanimStackPos = 0;
            gMatStackIndex--;
            incrementMatStack();
            gCurGraphNodeObject = (struct GraphNodeObject *) node;
            node->header.gfx.sharedChild->parent = &node->header.gfx.node;
            geo_process_node_and_siblings(node->header.gfx.sharedChild);
            if (node->header.gfx.sharedChild) {
                node->header.gfx.sharedChild->parent = NULL;
            }
            gCurGraphNodeObject = NULL;
            /*if (((struct GraphNodeRoot *) node)->node.children != NULL) {
                geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
            }*/
            node->header.gfx.firstBit = 1;
        } else {
            node->header.gfx.firstBit = 0;
        }

        gMatStackIndex--;
        node->header.gfx.throwMatrix = 0;
        gCurAnimType = ANIM_TYPE_NONE;
    }
}

/**
 * Process an object parent node. Temporarily assigns itself as the parent of
 * the subtree rooted at 'sharedChild' and processes the subtree, after which the
 * actual children are be processed. (in practice they are null though)
 */
void geo_process_object_parent(struct GraphNodeObjectParent *node) {
    if (node->sharedChild != NULL) {
        node->sharedChild->parent = (struct GraphNode *) node;
        geo_process_node_and_siblings(node->sharedChild);
        node->sharedChild->parent = NULL;
    }
    if (((struct GraphNodeRoot *) node)->node.children != NULL) {
        geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
    }
}

/**
 * Process a held object node.
 */
void geo_process_held_object(struct GraphNodeHeldObject *node) {
    Mat4 mat;
    Vec3f translation;
    Mat4 tempMtx;

#ifdef F3DEX_GBI_2
    gSPLookAt(gDisplayListHead++, &lookAt);
#endif

    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    if (node->objNode != NULL && node->objNode->header.gfx.sharedChild != NULL) {
        s32 hasAnimation = (node->objNode->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION) != 0;

        translation[0] = node->translation[0] * .25f;
        translation[1] = node->translation[1] * .25f;
        translation[2] = node->translation[2] * .25f;

        mtxf_translate(mat, translation);
        mtxf_copy(gMatStack[gMatStackIndex + 1], (void *) gCurGraphNodeObject->throwMatrix);
        vec3f_copy(gMatStack[gMatStackIndex + 1][3], gMatStack[gMatStackIndex][3]);
        mtxf_copy(tempMtx, gMatStack[gMatStackIndex + 1]);
        mtxf_mul(gMatStack[gMatStackIndex + 1], mat, tempMtx);
        mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex + 1],
                         node->objNode->header.gfx.scale);
        if (node->fnNode.func != NULL) {
            node->fnNode.func(GEO_CONTEXT_HELD_OBJ, &node->fnNode.node,
                              (struct AllocOnlyPool *) gMatStack[gMatStackIndex + 1]);
        }
        incrementMatStack();
        gGeoTempState.type = gCurAnimType;
        gGeoTempState.enabled = gCurAnimEnabled;
        gGeoTempState.frame = gCurrAnimFrame;
        gGeoTempState.translationMultiplier = gCurAnimTranslationMultiplier;
        gGeoTempState.attribute = gCurrAnimAttribute;
        gGeoTempState.data = gCurAnimData;
        gCurAnimType = 0;
        gCurGraphNodeHeldObject = (void *) node;
        if (node->objNode->header.gfx.unk38.curAnim != NULL) {
            geo_set_animation_globals(&node->objNode->header.gfx.unk38,
                                      node->objNode->header.gfx.firstBit);
        }
        curanimPosStack = node->objNode->header.gfx.unk38.animPosStack;
        curanimRotStack = node->objNode->header.gfx.unk38.animRotStack;
        curanimStackPos = 0;

        geo_process_node_and_siblings(node->objNode->header.gfx.sharedChild);
        gCurGraphNodeHeldObject = NULL;
        gCurAnimType = gGeoTempState.type;
        gCurAnimEnabled = gGeoTempState.enabled;
        gCurrAnimFrame = gGeoTempState.frame;
        gCurAnimTranslationMultiplier = gGeoTempState.translationMultiplier;
        gCurrAnimAttribute = gGeoTempState.attribute;
        gCurAnimData = gGeoTempState.data;
        gMatStackIndex--;
    }

    if (((struct GraphNodeRoot *) node)->node.children != NULL) {
        geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
    }
}

typedef void (*GeoIngameTable)(struct GraphNode *firstNode);

const GeoIngameTable GeoInteractTable[] = {
    geo_try_process_children,
    geo_try_process_children,
    geo_process_ortho_projection,
    geo_try_process_children,
    geo_process_master_list,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_level_of_detail,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_translation_rotation,
    geo_process_translation,
    geo_process_rotation,
    geo_process_object,
    geo_try_process_children,
    geo_process_billboard,
    geo_process_display_list,
    geo_process_scale,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_animated_part,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_shadow,
    geo_process_object_parent,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_perspective,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_switch,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_camera,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_generated_list,
    geo_try_process_children,
    geo_process_background,
    geo_try_process_children,
    geo_process_held_object,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_try_process_children,
    geo_process_generated_list,
    geo_try_process_children,
    geo_process_background,
    geo_try_process_children,
    geo_process_held_object,
    geo_try_process_children,

};

/**
 * Process a generic geo node and its siblings.
 * The first argument is the start node, and all its siblings will
 * be iterated over.
 */
void geo_process_node_and_siblings(struct GraphNode *firstNode) {
    s16 iterateChildren = TRUE;
    struct GraphNode *curGraphNode = firstNode;
    struct GraphNode *parent = curGraphNode->parent;

    // In the case of a switch node, exactly one of the children of the node is
    // processed instead of all children like usual
    if (parent != NULL) {
        iterateChildren = (parent->type != GRAPH_NODE_TYPE_SWITCH_CASE);
    }

    do {
        if (curGraphNode->flags & GRAPH_RENDER_ACTIVE) {
            if (curGraphNode->flags & GRAPH_RENDER_CHILDREN_FIRST) {
                geo_try_process_children(curGraphNode);
            } else {
                // call here
                GeoInteractTable[curGraphNode->type](curGraphNode);
            }
        }
    } while (iterateChildren && (curGraphNode = curGraphNode->next) != firstNode);
}

/**
 * Process a root node. This is the entry point for processing the scene graph.
 * The root node itself sets up the viewport, then all its children are processed
 * to set up the projection and draw display lists.
 */
void geo_process_root(struct GraphNodeRoot *node, Vp *b, Vp *c,const  s32 clearColor) {
    if (node->node.flags & GRAPH_RENDER_ACTIVE) {
        Mtx *initialMatrix;
        Vp *viewport = alloc_display_list(sizeof(*viewport));

        gDisplayListHeap = alloc_only_pool_init(main_pool_available() - sizeof(struct AllocOnlyPool),
                                                MEMORY_POOL_LEFT);
        initialMatrix = alloc_display_list(sizeof(*initialMatrix));
        gMatStackIndex = 0;
        gCurAnimType = 0;
        vec3s_set(viewport->vp.vtrans, node->x * 4, node->y * 4, 511);
        vec3s_set(viewport->vp.vscale, node->width * 4, node->height * 4, 511);
        if (b != NULL) {
            clear_frame_buffer(clearColor);
            make_viewport_clip_rect(b);
            *viewport = *b;
        }

        else if (c != NULL) {
            clear_frame_buffer(clearColor);
            make_viewport_clip_rect(c);
        }

        mtxf_identity(gMatStack[0]);
        mtxf_to_mtx(initialMatrix, gMatStack[0]);
        gMatStackFixed[0] = initialMatrix;
        gSPViewport(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(viewport));
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(gMatStackFixed[0]),
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gCurGraphNodeRoot = node;

        if (((struct GraphNodeRoot *) node)->node.children != NULL) {
            geo_process_node_and_siblings(((struct GraphNodeRoot *) node)->node.children);
        }
        gCurGraphNodeRoot = NULL;
        main_pool_free(gDisplayListHeap);
    }
}
