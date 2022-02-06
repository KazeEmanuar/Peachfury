#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "debug.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/geo_layout.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "helper_macros.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "OPT_FOR_SIZE.h"

s8 D_8032F0A0[] = { 0xF8, 0x08, 0xFC, 0x04 };
static s8 sLevelsWithRooms[] = { LEVEL_CASTLE, LEVEL_HMC, -1 };

static s32 clear_move_flag(u32 *, s32);

#define o gCurrentObject
#define OPSIZ 4
void *memset(void *dstpp, int c, size_t len) {
    long int dstp = (long int) dstpp;

    // if (len >= 8) {
    size_t xlen;
    u32 cccc;

    cccc = (unsigned char) c;
    cccc |= cccc << 8;
    cccc |= cccc << 16;
    // if (OPSIZ > 4)
    /* Do the shift in two steps to avoid warning if long has 32 bits.  */
    //  cccc |= (cccc << 16) << 16;

    /* There are at least some bytes to set.
       No need to test for LEN == 0 in this alignment loop.  */
    while (dstp % OPSIZ != 0) {
        ((u8 *) dstp)[0] = c;
        dstp += 1;
        len -= 1;
    }

    /* Write 8 `u32' per iteration until less than 8 `u32' remain.  */
    xlen = len / (OPSIZ * /*8*/ 4);
    while (xlen > 0) {
        ((u32 *) dstp)[0] = cccc;
        ((u32 *) dstp)[1] = cccc;
        ((u32 *) dstp)[2] = cccc;
        ((u32 *) dstp)[3] = cccc;
        /*((u32 *) dstp)[4] = cccc;
        ((u32 *) dstp)[5] = cccc;
        ((u32 *) dstp)[6] = cccc;
        ((u32 *) dstp)[7] = cccc;*/
        dstp += /*8*/ 4 * OPSIZ;
        xlen -= 1;
    }
    len &= (OPSIZ * /*8*/ 4 - 1);

    /* Write 1 `u32' per iteration until less than OPSIZ bytes remain.  */
    /*xlen = len / OPSIZ;
    while (xlen > 0) {
        ((u32 *) dstp)[0] = cccc;
        dstp += OPSIZ;
        xlen -= 1;
    }
    len %= OPSIZ;*/
    //}

    /* Write the last few bytes.  */
    while (len > 0) {
        ((u8 *) dstp)[0] = c;
        dstp += 1;
        len -= 1;
    }

    return dstpp;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *pd = (unsigned char *)dest;
    const unsigned char *ps = (unsigned char *)src;
    if ( ps < pd )
        for (pd += n, ps += n; n--;)
            *--pd = *--ps;
    else
        while(n--)
            *pd++ = *ps++;
    return dest;
}

Gfx *geo_update_projectile_pos_from_parent(s32 callContext, UNUSED struct GraphNode *node, Mat4 mtx) {
    Mat4 sp20;
    struct Object *sp1C;

    if (callContext == GEO_CONTEXT_RENDER) {
        sp1C = (struct Object *) gCurGraphNodeObject; // TODO: change global type to Object pointer
        if (sp1C->prevObj) {
            create_transformation_from_matrices(sp20, mtx, gCurGraphNodeCamera->matrixPtr);
            obj_update_pos_from_parent_transformation(sp20, sp1C->prevObj);
            obj_set_gfx_pos_from_pos(sp1C->prevObj);
        }
    }
    return NULL;
}

Gfx *geo_update_layer_transparency(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    Gfx *dlStart, *dlHead;
    struct Object *objectGraphNode;
    struct GraphNodeGenerated *currentGraphNode;
    s32 objectOpacity;

    dlStart = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        objectGraphNode = (struct Object *) gCurGraphNodeObject; // TODO: change this to object pointer?
        currentGraphNode = (struct GraphNodeGenerated *) node;

        if (gCurGraphNodeHeldObject) {
            objectGraphNode = gCurGraphNodeHeldObject->objNode;
        }

        objectOpacity = objectGraphNode->oOpacity;
        dlStart = alloc_display_list(sizeof(Gfx) * 3);

        dlHead = dlStart;

        if (objectOpacity == 0xFF) {
            if (currentGraphNode->parameter == 20) {
                currentGraphNode->fnNode.node.flags =
                    0x600 | (currentGraphNode->fnNode.node.flags & 0xFF);
            } else {
                currentGraphNode->fnNode.node.flags =
                    0x100 | (currentGraphNode->fnNode.node.flags & 0xFF);
            }

            objectGraphNode->oAnimState = 0;
        } else {
            if (currentGraphNode->parameter == 20) {
                currentGraphNode->fnNode.node.flags =
                    0x600 | (currentGraphNode->fnNode.node.flags & 0xFF);
            } else {
                currentGraphNode->fnNode.node.flags =
                    0x500 | (currentGraphNode->fnNode.node.flags & 0xFF);
            }

            objectGraphNode->oAnimState = 1;

            if (objectOpacity == 0 && segmented_to_virtual(bhvBowser) == objectGraphNode->behavior) {
                objectGraphNode->oAnimState = 2;
            }
            // the debug info check was removed in US. so we need to
            // perform the only necessary check instead of the debuginfo
            // one.
            if (currentGraphNode->parameter != 10) {
                if (objectGraphNode->activeFlags & ACTIVE_FLAG_DITHERED_ALPHA) {
                    gDPSetAlphaCompare(dlHead++, G_AC_DITHER);
                }
            }
        }

        gDPSetEnvColor(dlHead++, 255, 255, 255, objectOpacity);
        gSPEndDisplayList(dlHead);
    }

    return dlStart;
}

/**
 * @bug Every geo function declares the 3 parameters of callContext, node, and
 * the matrix array. This one (see also geo_switch_area) doesn't. When executed,
 * the node function executor passes the 3rd argument to a function that doesn't
 * declare it. This is undefined behavior, but harmless in practice due to the
 * o32 calling convention.
 */
#ifdef AVOID_UB
Gfx *geo_switch_anim_state(s32 callContext, struct GraphNode *node, UNUSED void *context) {
#else
Gfx *geo_switch_anim_state(s32 callContext, struct GraphNode *node) {
#endif
    struct Object *obj;
    struct GraphNodeSwitchCase *switchCase;

    if (callContext == GEO_CONTEXT_RENDER) {
        obj = (struct Object *) gCurGraphNodeObject; // TODO: change global type to Object pointer

        // move to a local var because GraphNodes are passed in all geo functions.
        // cast the pointer.
        switchCase = (struct GraphNodeSwitchCase *) node;

        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        // if the case is greater than the number of cases, set to 0 to avoid overflowing
        // the switch.
        if (obj->oAnimState >= switchCase->numCases) {
            obj->oAnimState = 0;
        }

        // assign the case number for execution.
        switchCase->selectedCase = obj->oAnimState;
    }

    return NULL;
}

#ifdef AVOID_UB
Gfx *set_prim_to_opacity(s32 callContext, struct GraphNode *node, UNUSED void *context) {
#else
Gfx *set_prim_to_opacity(s32 callContext, struct GraphNode *node) {
#endif
    Gfx *dlStart;
    Gfx *DL;
    struct Object *obj;
    struct GraphNodeSwitchCase *switchCase;
    struct GraphNodeGenerated *currentGraphNode = (struct GraphNodeGenerated *) node;
    dlStart = alloc_display_list(sizeof(Gfx) * 3);
    DL = dlStart;
    if (callContext == GEO_CONTEXT_RENDER) {
        obj = (struct Object *) gCurGraphNodeObject; // TODO: change global type to Object pointer

        gDPSetPrimColor(DL++, 0, 0, obj->oOpacity, obj->oOpacity, obj->oOpacity, obj->oOpacity);
        gSPEndDisplayList(DL);
        if (((struct GraphNodeGenerated *) node)->parameter) {
            currentGraphNode->fnNode.node.flags =
                (currentGraphNode->fnNode.node.flags & 0xFF)
                | (((struct GraphNodeGenerated *) node)->parameter << 8);
        } else {

            currentGraphNode->fnNode.node.flags =
                (currentGraphNode->fnNode.node.flags & 0xFF) | (LAYER_TRANSPARENT_DECAL << 8);
        }
        return dlStart;
    }

    return NULL;
}

Gfx *geo_switch_bparam(s32 callContext, struct GraphNode *node) {
    struct Object *obj;
    struct GraphNodeSwitchCase *switchCase;

    if (callContext == GEO_CONTEXT_RENDER) {
        obj = (struct Object *) gCurGraphNodeObject; // TODO: change global type to Object pointer

        // move to a local var because GraphNodes are passed in all geo functions.
        // cast the pointer.
        switchCase = (struct GraphNodeSwitchCase *) node;

        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        // if the case is greater than the number of cases, set to 0 to avoid overflowing
        // the switch.
        if (obj->oBehParams2ndByte >= switchCase->numCases) {
            obj->oBehParams2ndByte = 0;
        }

        // assign the case number for execution.
        switchCase->selectedCase = obj->oBehParams2ndByte;
    }

    return NULL;
}

//! @bug Same issue as geo_switch_anim_state.
#ifdef AVOID_UB
Gfx *geo_switch_area(s32 callContext, struct GraphNode *node, UNUSED void *context) {
#else
Gfx *geo_switch_area(s32 callContext, struct GraphNode *node) {
#endif
    s16 sp26;
    struct Surface *sp20;
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;

    if (callContext == GEO_CONTEXT_RENDER) {
        if (gMarioObject == NULL) {
            switchCase->selectedCase = 0;
        } else {

            find_floor(gMarioObject->oPosX, gMarioObject->oPosY, gMarioObject->oPosZ, &sp20);

            if (sp20) {
                sp26 = sp20->room - 1;

                if (sp26 >= 0) {
                    switchCase->selectedCase = sp26;
                }
            }
        }
    } else {
        switchCase->selectedCase = 0;
    }

    return NULL;
}

void obj_update_pos_from_parent_transformation(Mat4 a0, struct Object *a1) {
    f32 spC, sp8, sp4;
    s32 j;
    spC = a1->oParentRelativePosX;
    sp8 = a1->oParentRelativePosY;
    sp4 = a1->oParentRelativePosZ;

    /*a1->oPosX = spC * a0[0][0] + sp8 * a0[1][0] + sp4 * a0[2][0] + a0[3][0];
    a1->oPosY = spC * a0[0][1] + sp8 * a0[1][1] + sp4 * a0[2][1] + a0[3][1];
    a1->oPosZ = spC * a0[0][2] + sp8 * a0[1][2] + sp4 * a0[2][2] + a0[3][2];*/
    for (j = 0; j < 3; j++) {
        a1->OBJECT_FIELD_F32(0x06 + j) = spC * a0[0][j] + sp8 * a0[1][j] + sp4 * a0[2][j] + a0[3][j];
    }
}

void create_transformation_from_matrices(Mat4 a0, Mat4 a1, Mat4 a2) {
    Vec3f medium;
    s32 j, i;
    for (j = 0; j < 3; j++) {
        medium[j] = a2[3][0] * a2[j][0] + a2[3][1] * a2[j][1] + a2[3][2] * a2[j][2];
        a0[j][3] = 0;
    }
    /*medium[0] = a2[3][0] * a2[0][0] + a2[3][1] * a2[0][1] + a2[3][2] * a2[0][2];
    medium[1] = a2[3][0] * a2[1][0] + a2[3][1] * a2[1][1] + a2[3][2] * a2[1][2];
    medium[2] = a2[3][0] * a2[2][0] + a2[3][1] * a2[2][1] + a2[3][2] * a2[2][2];*/

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 3; i++) {
            a0[j][i] = a1[j][0] * a2[i][0] + a1[j][1] * a2[i][1] + a1[j][2] * a2[i][2];
        }
    }
    a0[3][0] -= medium[0];
    a0[3][1] -= medium[1];
    a0[3][2] -= medium[2];

    /*a0[0][0] = a1[0][0] * a2[0][0] + a1[0][1] * a2[0][1] + a1[0][2] * a2[0][2];
    a0[0][1] = a1[0][0] * a2[1][0] + a1[0][1] * a2[1][1] + a1[0][2] * a2[1][2];
    a0[0][2] = a1[0][0] * a2[2][0] + a1[0][1] * a2[2][1] + a1[0][2] * a2[2][2];

    a0[1][0] = a1[1][0] * a2[0][0] + a1[1][1] * a2[0][1] + a1[1][2] * a2[0][2];
    a0[1][1] = a1[1][0] * a2[1][0] + a1[1][1] * a2[1][1] + a1[1][2] * a2[1][2];
    a0[1][2] = a1[1][0] * a2[2][0] + a1[1][1] * a2[2][1] + a1[1][2] * a2[2][2];

    a0[2][0] = a1[2][0] * a2[0][0] + a1[2][1] * a2[0][1] + a1[2][2] * a2[0][2];
    a0[2][1] = a1[2][0] * a2[1][0] + a1[2][1] * a2[1][1] + a1[2][2] * a2[1][2];
    a0[2][2] = a1[2][0] * a2[2][0] + a1[2][1] * a2[2][1] + a1[2][2] * a2[2][2];

    a0[3][0] = a1[3][0] * a2[0][0] + a1[3][1] * a2[0][1] + a1[3][2] * a2[0][2] - medium[0];
    a0[3][1] = a1[3][0] * a2[1][0] + a1[3][1] * a2[1][1] + a1[3][2] * a2[1][2] - medium[1];
    a0[3][2] = a1[3][0] * a2[2][0] + a1[3][1] * a2[2][1] + a1[3][2] * a2[2][2] - medium[2];*/

    /*a0[0][3] = 0;
    a0[1][3] = 0;
    a0[2][3] = 0;*/
    *((u32 *) &a0[3][3]) = 0x3F800000;
}

void obj_set_held_state(struct Object *obj, const BehaviorScript *heldBehavior) {
    obj->parentObj = o;

    if (obj->oFlags & OBJ_FLAG_HOLDABLE) {
        if (heldBehavior == bhvCarrySomething3) {
            obj->oHeldState = HELD_HELD;
        } else if (heldBehavior == bhvCarrySomething5) {
            obj->oHeldState = HELD_THROWN;
        } else if (heldBehavior == bhvCarrySomething4) {
            obj->oHeldState = HELD_DROPPED;
        }
    } else {
        obj->curBhvCommand = segmented_to_virtual(heldBehavior);
        obj->bhvStackIndex = 0;
    }
}

f32 lateral_dist_between_objects(struct Object *obj1, struct Object *obj2) {
    f32 dx = obj1->oPosX - obj2->oPosX;
    f32 dz = obj1->oPosZ - obj2->oPosZ;

    return sqrtf(dx * dx + dz * dz);
}

f32 dist_between_objects(struct Object *obj1, struct Object *obj2) {
    f32 dx = obj1->oPosX - obj2->oPosX;
    f32 dy = obj1->oPosY - obj2->oPosY;
    f32 dz = obj1->oPosZ - obj2->oPosZ;

    return sqrtf(dx * dx + dy * dy + dz * dz);
}
f32 approach_f32_symmetric(f32 value, f32 target, f32 increment) {
    f32 dist;

    if ((dist = target - value) >= 0.0f) {
        if (dist > increment) {
            value += increment;
        } else {
            value = target;
        }
    } else {
        if (dist < -increment) {
            value -= increment;
        } else {
            value = target;
        }
    }

    return value;
}

s32 approach_s16_symmetric(s16 value, s16 target, s32 increment) {
    s16 dist = target - value;

    if (dist >= 0) {
        if (dist > increment) {
            value += increment;
        } else {
            value = target;
        }
    } else {
        if (dist < -increment) {
            value -= increment;
        } else {
            value = target;
        }
    }

    return value;
}

s32 cur_obj_rotate_yaw_toward(s16 target, s16 increment) {
    s16 startYaw;

    startYaw = (s16) o->oMoveAngleYaw;
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, target, increment);

    return ((o->oAngleVelYaw = (s16) ((s16) o->oMoveAngleYaw - startYaw)) == 0);
}

s32 obj_angle_to_object(struct Object *obj1, struct Object *obj2) {
    return atan2s(obj2->oPosZ - obj1->oPosZ, obj2->oPosX - obj1->oPosX);
}

s32 obj_turn_toward_object(struct Object *obj, struct Object *target, s32 angleIndex, s32 turnAmount) {
    f32 a, b, c, d;
    s32 targetAngle, startAngle;

    switch (angleIndex) {
        case O_MOVE_ANGLE_PITCH_INDEX:
        case O_FACE_ANGLE_PITCH_INDEX:
            a = target->oPosX - obj->oPosX;
            c = target->oPosZ - obj->oPosZ;
            a = sqrtf(a * a + c * c);

            b = -obj->oPosY;
            d = -target->oPosY;

            targetAngle = atan2s(a, d - b);
            break;

        case O_MOVE_ANGLE_YAW_INDEX:
        case O_FACE_ANGLE_YAW_INDEX:
            a = obj->oPosZ;
            c = target->oPosZ;
            b = obj->oPosX;
            d = target->oPosX;

            targetAngle = atan2s(c - a, d - b);
            break;
    }

    startAngle = o->rawData.asU32[angleIndex];
    o->rawData.asU32[angleIndex] = approach_s16_symmetric(startAngle, targetAngle, turnAmount);
    return targetAngle;
}

void obj_set_angle(struct Object *obj, s32 pitch, s32 yaw, s32 roll) {
    obj->oFaceAnglePitch = pitch;
    obj->oFaceAngleYaw = yaw;
    obj->oFaceAngleRoll = roll;

    obj->oMoveAnglePitch = pitch;
    obj->oMoveAngleYaw = yaw;
    obj->oMoveAngleRoll = roll;
}

/*
 * Spawns an object at an absolute location with a specified angle.
 */
struct Object *spawn_object_abs_with_rot(struct Object *parent, s16 uselessArg, u32 model,
                                         const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 rx,
                                         s16 ry, s16 rz) {
    // 'uselessArg' is unused in the function spawn_object_at_origin()
    struct Object *newObj = spawn_object_at_origin(parent, uselessArg, model, behavior);
    obj_set_pos(newObj, x, y, z);
    obj_set_angle(newObj, rx, ry, rz);
    newObj->header.gfx.pos[0] = x;
    newObj->header.gfx.pos[1] = y;
    newObj->header.gfx.pos[2] = z;
    newObj->header.gfx.angle[0] = rx;
    newObj->header.gfx.angle[1] = ry;
    newObj->header.gfx.angle[2] = rz;

    return newObj;
}

/*
 * Spawns an object relative to the parent with a specified angle... is what it is supposed to do.
 * The rz argument is never used, and the z offset is used for z-rotation instead. This is most likely
 * a copy-paste typo by one of the programmers.
 */
struct Object *spawn_object_rel_with_rot(struct Object *parent, u32 model,
                                         const BehaviorScript *behavior, s16 xOff, s16 yOff, s16 zOff,
                                         s16 rx, s16 ry, s16 rz) {
    struct Object *newObj = spawn_object_at_origin(parent, 0, model, behavior);
    newObj->oFlags |= OBJ_FLAG_TRANSFORM_RELATIVE_TO_PARENT;
    obj_set_parent_relative_pos(newObj, xOff, yOff, zOff);
    obj_set_angle(newObj, rx, ry, rz); // Nice typo you got there Nintendo.
    obj_build_transform_relative_to_parent(newObj);

    return newObj;
}

struct Object *spawn_obj_with_transform_flags(struct Object *sp20, s32 model,
                                              const BehaviorScript *sp28) {
    struct Object *sp1C = spawn_object(sp20, model, sp28);
    sp1C->oFlags |= OBJ_FLAG_0020 | OBJ_FLAG_SET_THROW_MATRIX_FROM_TRANSFORM;
    return sp1C;
}

struct Object *spawn_water_droplet(struct Object *parent, struct WaterDropletParams *params) {
    f32 randomScale;
    struct Object *newObj = spawn_object(parent, params->model, params->behavior);

    if (params->flags & WATER_DROPLET_FLAG_RAND_ANGLE) {
        newObj->oMoveAngleYaw = random_u16();
    }

    if (params->flags & WATER_DROPLET_FLAG_RAND_ANGLE_INCR_PLUS_8000) {
        newObj->oMoveAngleYaw = (s16) (newObj->oMoveAngleYaw + 0x8000)
                                + (s16) random_f32_around_zero(params->moveAngleRange);
    }

    if (params->flags & WATER_DROPLET_FLAG_RAND_ANGLE_INCR) {
        newObj->oMoveAngleYaw =
            (s16) newObj->oMoveAngleYaw + (s16) random_f32_around_zero(params->moveAngleRange);
    }

    if (params->flags & WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL) {
        newObj->oPosY = find_water_level(newObj->oPosX, newObj->oPosZ);
    }

    if (params->flags & WATER_DROPLET_FLAG_RAND_OFFSET_XZ) {
        obj_translate_xz_random(newObj, params->moveRange);
    }

    if (params->flags & WATER_DROPLET_FLAG_RAND_OFFSET_XYZ) {
        obj_translate_xyz_random(newObj, params->moveRange);
    }

    newObj->oForwardVel = random_float() * params->randForwardVelScale + params->randForwardVelOffset;
    newObj->oVelY = random_float() * params->randYVelScale + params->randYVelOffset;

    randomScale = random_float() * params->randSizeScale + params->randSizeOffset;
    obj_scale(newObj, randomScale);

    return newObj;
}

struct Object *spawn_object_at_origin(struct Object *parent, UNUSED s32 unusedArg, u32 model,
                                      const BehaviorScript *behavior) {
    struct Object *obj;
    const BehaviorScript *behaviorAddr;

    behaviorAddr = segmented_to_virtual(behavior);
    obj = create_object(behaviorAddr);

    obj->parentObj = parent;
    // obj->header.gfx.unk18 = parent->header.gfx.unk18;
    obj->header.gfx.unk19 = parent->header.gfx.unk19;
    obj->oRoom = parent->oRoom;
    geo_obj_init((struct GraphNodeObject *) &obj->header.gfx, gLoadedGraphNodes[model], gVec3fZero,
                 gVec3sZero);

    return obj;
}

struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior) {
    struct Object *obj;

    obj = spawn_object_at_origin(parent, 0, model, behavior);
    obj_copy_pos_and_angle(obj, parent);
    obj->header.gfx.pos[0] = parent->header.gfx.pos[0];
    obj->header.gfx.pos[1] = parent->header.gfx.pos[1];
    obj->header.gfx.pos[2] = parent->header.gfx.pos[2];
    obj->header.gfx.angle[0] = parent->header.gfx.angle[0];
    obj->header.gfx.angle[1] = parent->header.gfx.angle[1];
    obj->header.gfx.angle[2] = parent->header.gfx.angle[2];

    return obj;
}

struct Object *try_to_spawn_object(s16 offsetY, f32 scale, struct Object *parent, s32 model,
                                   const BehaviorScript *behavior) {
    struct Object *obj;

    if (gFreeObjectList.next != NULL) {
        obj = spawn_object(parent, model, behavior);
        obj->oPosY += offsetY;
        obj_scale(obj, scale);
        return obj;
    } else {
        return NULL;
    }
}

struct Object *spawn_object_with_scale(struct Object *parent, s32 model, const BehaviorScript *behavior,
                                       f32 scale) {
    struct Object *obj;

    obj = spawn_object_at_origin(parent, 0, model, behavior);
    obj_copy_pos_and_angle(obj, parent);
    obj_scale(obj, scale);

    return obj;
}

void obj_build_relative_transform(struct Object *obj) {
    obj_build_transform_from_pos_and_angle(obj, O_PARENT_RELATIVE_POS_INDEX, O_FACE_ANGLE_INDEX);
    obj_translate_local(obj, O_POS_INDEX, O_PARENT_RELATIVE_POS_INDEX);
}

struct Object *spawn_object_relative(s16 behaviorParam, s16 relativePosX, s16 relativePosY,
                                     s16 relativePosZ, struct Object *parent, s32 model,
                                     const BehaviorScript *behavior) {
    struct Object *obj = spawn_object_at_origin(parent, 0, model, behavior);

    obj_copy_pos_and_angle(obj, parent);
    obj_set_parent_relative_pos(obj, relativePosX, relativePosY, relativePosZ);
    obj_build_relative_transform(obj);

    obj->oBehParams2ndByte = behaviorParam;
    obj->oBehParams = (behaviorParam & 0xFF) << 16;

    return obj;
}

struct Object *spawn_object_relative_with_scale(s16 behaviorParam, s16 relativePosX, s16 relativePosY,
                                                s16 relativePosZ, f32 scale, struct Object *parent,
                                                s32 model, const BehaviorScript *behavior) {
    struct Object *obj;

    obj = spawn_object_relative(behaviorParam, relativePosX, relativePosY, relativePosZ, parent, model,
                                behavior);
    obj_scale(obj, scale);

    return obj;
}

void cur_obj_move_using_vel(void) {
    o->oPosX += o->oVelX;
    o->oPosY += o->oVelY;
    o->oPosZ += o->oVelZ;
}

void obj_copy_graph_y_offset(struct Object *dst, struct Object *src) {
    dst->oGraphYOffset = src->oGraphYOffset;
}

void obj_copy_pos_and_angle(struct Object *dst, struct Object *src) {
    s32 j;
    for (j = 0; j < 3; j++) {
        dst->OBJECT_FIELD_F32(0x06 + j) = src->OBJECT_FIELD_F32(0x06 + j);
        dst->OBJECT_FIELD_F32(0x0F + j) = src->OBJECT_FIELD_F32(0x0F + j);
        dst->OBJECT_FIELD_F32(0x12 + j) = src->OBJECT_FIELD_F32(0x12 + j);
    }
}

void obj_copy_pos(struct Object *dst, struct Object *src) {
    s32 j;
    for (j = 0; j < 3; j++) {
        dst->OBJECT_FIELD_F32(0x06 + j) = src->OBJECT_FIELD_F32(0x06 + j);
    }
}

void obj_set_gfx_pos_from_pos(struct Object *obj) {
    s32 j;
    for (j = 0; j < 3; j++) {
        obj->header.gfx.pos[j] = obj->OBJECT_FIELD_F32(0x06 + j);
    }
}

void obj_init_animation(struct Object *obj, s32 animIndex) {
    struct Animation **anims = o->oAnimations;
    geo_obj_init_animation(&obj->header.gfx, &anims[animIndex]);
}

/**
 * Multiply a vector by a matrix of the form
 * | ? ? ? 0 |
 * | ? ? ? 0 |
 * | ? ? ? 0 |
 * | 0 0 0 1 |
 * i.e. a matrix representing a linear transformation over 3 space.
 */
void linear_mtxf_mul_vec3f(Mat4 m, Vec3f dst, Vec3f v) {
    s32 i;
    for (i = 0; i < 3; i++) {
        dst[i] = m[0][i] * v[0] + m[1][i] * v[1] + m[2][i] * v[2];
    }
}

/**
 * Multiply a vector by the transpose of a matrix of the form
 * | ? ? ? 0 |
 * | ? ? ? 0 |
 * | ? ? ? 0 |
 * | 0 0 0 1 |
 * i.e. a matrix representing a linear transformation over 3 space.
 */
void linear_mtxf_transpose_mul_vec3f(Mat4 m, Vec3f dst, Vec3f v) {
    s32 i;
    for (i = 0; i < 3; i++) {
        dst[i] = m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2];
    }
}

void obj_apply_scale_to_transform(struct Object *obj) {
    f32 scaleX, scaleY, scaleZ;

    scaleX = obj->header.gfx.scale[0];
    scaleY = obj->header.gfx.scale[1];
    scaleZ = obj->header.gfx.scale[2];

    obj->transform[0][0] *= scaleX;
    obj->transform[0][1] *= scaleX;
    obj->transform[0][2] *= scaleX;

    obj->transform[1][0] *= scaleY;
    obj->transform[1][1] *= scaleY;
    obj->transform[1][2] *= scaleY;

    obj->transform[2][0] *= scaleZ;
    obj->transform[2][1] *= scaleZ;
    obj->transform[2][2] *= scaleZ;
}

void obj_copy_scale(struct Object *dst, struct Object *src) {
    s32 j;
    for (j = 0; j < 3; j++) {
        dst->header.gfx.scale[j] = src->header.gfx.scale[j];
    }
};
void obj_scale(struct Object *obj, f32 scale) {
    obj->header.gfx.scale[0] = scale;
    obj->header.gfx.scale[1] = scale;
    obj->header.gfx.scale[2] = scale;
};
void cur_obj_scale(f32 scale) {
    o->header.gfx.scale[0] = scale;
    o->header.gfx.scale[1] = scale;
    o->header.gfx.scale[2] = scale;
};

void cur_obj_init_animation(s32 animIndex) {
    struct Animation **anims = o->oAnimations;
    geo_obj_init_animation(&o->header.gfx, &anims[animIndex]);
    o->header.gfx.unk38.animID = animIndex;
}
void cur_obj_init_animation_with_sound(s32 animIndex) {
    struct Animation **anims = o->oAnimations;
    geo_obj_init_animation(&o->header.gfx, &anims[animIndex]);
    o->oSoundStateID = animIndex;
}

void cur_obj_init_animation_with_accel_and_sound(s32 animIndex, f32 accel) {
    struct Animation **anims = o->oAnimations;
    s32 animAccel = (s32) (accel * 65536.0f);
    geo_obj_init_animation_accel(&o->header.gfx, &anims[animIndex], animAccel);
    o->oSoundStateID = animIndex;
}

void obj_init_animation_with_sound(struct Object *obj, const struct Animation *const *animations,
                                   s32 animIndex) {
    struct Animation **anims = (struct Animation **) animations;
    obj->oAnimations = (struct Animation **) animations;
    geo_obj_init_animation(&obj->header.gfx, &anims[animIndex]);
    obj->oSoundStateID = animIndex;
}

void cur_obj_enable_rendering_and_become_tangible(struct Object *obj) {
    obj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    obj->oIntangibleTimer = 0;
}

void cur_obj_enable_rendering(void) {
    o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
}

void cur_obj_disable_rendering_and_become_intangible(struct Object *obj) {
    obj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    obj->oIntangibleTimer = -1;
}

void cur_obj_disable_rendering(void) {
    o->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
}

void cur_obj_unhide(void) {
    o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
}

void cur_obj_hide(void) {
    o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
}

void cur_obj_set_pos_relative(struct Object *other, f32 dleft, f32 dy, f32 dforward) {
    f32 facingZ = coss(other->oMoveAngleYaw);
    f32 facingX = sins(other->oMoveAngleYaw);

    f32 dz = dforward * facingZ - dleft * facingX;
    f32 dx = dforward * facingX + dleft * facingZ;

    o->oMoveAngleYaw = other->oMoveAngleYaw;

    o->oPosX = other->oPosX + dx;
    o->oPosY = other->oPosY + dy;
    o->oPosZ = other->oPosZ + dz;
}

u32 get_object_list_from_behavior(const BehaviorScript *behavior) {
    u32 objectList;

    // If the first behavior command is "begin", then get the object list header
    // from there
    if ((behavior[0] >> 24) == 0) {
        objectList = (behavior[0] >> 16) & 0xFFFF;
    } else {
        objectList = OBJ_LIST_DEFAULT;
    }

    return objectList;
}

struct Object *cur_obj_nearest_object_with_behavior(const BehaviorScript *behavior) {
    struct Object *obj;
    f32 dist;

    obj = cur_obj_find_nearest_object_with_behavior(behavior, &dist);

    return obj;
}

f32 cur_obj_dist_to_nearest_object_with_behavior(const BehaviorScript *behavior) {
    struct Object *obj;
    f32 dist;

    obj = cur_obj_find_nearest_object_with_behavior(behavior, &dist);
    if (obj == NULL) {
        dist = 15000.0f;
    }

    return dist;
}

f32 find_nearest_TNT(struct Object *child, struct Object **tnt) {
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvTNT);
    struct Object *obj;
    struct ObjectNode *listHead;
    f32 minDist = 0x20000;
    listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    obj = (struct Object *) listHead->next;
    child->oPosY += 300.f;
    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && obj != child) {
                if ((obj != gMarioState->heldObj) && (obj->oForwardVel > 5.f)) {
                    f32 objDist = dist_between_objects(child, obj);
                    if (objDist < minDist) {
                        minDist = objDist;
                        *tnt = obj;
                    }
                }
            }
        }
        obj = (struct Object *) obj->header.next;
    }
    child->oPosY -= 300.f;
    return minDist;
}

struct Object *cur_obj_find_nearest_object_with_behavior(const BehaviorScript *behavior, f32 *dist) {
    uintptr_t *behaviorAddr = segmented_to_virtual(behavior);
    struct Object *closestObj = NULL;
    struct Object *obj;
    struct ObjectNode *listHead;
    f32 minDist = 100000.f;

    listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && obj != o) {
                f32 objDist = dist_between_objects(o, obj);
                if (objDist < minDist) {
                    closestObj = obj;
                    minDist = objDist;
                }
            }
        }
        obj = (struct Object *) obj->header.next;
    }

    *dist = minDist;
    return closestObj;
}

struct Object *cur_obj_find_nearest_object_with_bParam2andBehav(const BehaviorScript *behavior,
                                                                u32 Bparam2) {
    uintptr_t *behaviorAddr = segmented_to_virtual(behavior);
    struct Object *closestObj = NULL;
    struct Object *obj;
    struct ObjectNode *listHead;
    f32 minDist = 0x20000;

    listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr) {
            if (obj->oBehParams2ndByte == Bparam2) {
                if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && obj != o) {
                    f32 objDist = dist_between_objects(o, obj);
                    if (objDist < minDist) {
                        closestObj = obj;
                        minDist = objDist;
                    }
                }
            }
        }
        obj = (struct Object *) obj->header.next;
    }

    return closestObj;
}

struct Object *objExists(const BehaviorScript *behavior) {
    uintptr_t *behaviorAddr = segmented_to_virtual(behavior);
    struct Object *closestObj = NULL;
    struct Object *obj;
    struct ObjectNode *listHead;

    listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                return obj;
            }
        }
        obj = (struct Object *) obj->header.next;
    }
    return 0;
}

s32 count_objects_with_behavior(const BehaviorScript *behavior) {
    uintptr_t *behaviorAddr = segmented_to_virtual(behavior);
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    struct ObjectNode *obj = listHead->next;
    s32 count = 0;

    while (listHead != obj) {
        if (((struct Object *) obj)->behavior == behaviorAddr) {
            count++;
        }

        obj = obj->next;
    }

    return count;
}
void cur_obj_change_action(s32 action) {
    o->oAction = action;
    o->oPrevAction = action;
    o->oTimer = 0;
    o->oSubAction = 0;
}

void cur_obj_set_vel_from_mario_vel(f32 f12, f32 f14) {
    f32 sp4 = gMarioStates[0].forwardVel;
    f32 sp0 = f12 * f14;

    if (sp4 < sp0) {
        o->oForwardVel = sp0;
    } else {
        o->oForwardVel = sp4 * f14;
    }
}

void cur_obj_reverse_animation(void) {
    if (o->header.gfx.unk38.animFrame >= 0) {
        o->header.gfx.unk38.animFrame--;
    }
}

void cur_obj_extend_animation_if_at_end(void) {
    s32 sp4 = o->header.gfx.unk38.animFrame;
    s32 sp0 = o->header.gfx.unk38.curAnim->unk08 - 2;

    if (sp4 == sp0)
        o->header.gfx.unk38.animFrame--;
}

s32 cur_obj_check_if_near_animation_end(void) {
    u32 spC = (s32) o->header.gfx.unk38.curAnim->flags;
    s32 sp8 = o->header.gfx.unk38.animFrame;
    s32 sp4 = o->header.gfx.unk38.curAnim->unk08 - 2;
    s32 sp0 = FALSE;

    if (spC & 0x01) {
        if (sp4 + 1 == sp8) {
            return TRUE;
        }
    }

    if (sp8 == sp4) {
        return TRUE;
    }

    return 0;
}

s32 cur_obj_check_if_at_animation_end(void) {
    s32 sp4 = o->header.gfx.unk38.animFrame;
    s32 sp0 = o->header.gfx.unk38.curAnim->unk08 - 1;

    return sp4 == sp0;
}

s32 cur_obj_check_anim_frame(s32 frame) {
    s32 animFrame = o->header.gfx.unk38.animFrame;

    return animFrame == frame;
}

s32 cur_obj_check_anim_frame_in_range(s32 startFrame, s32 rangeLength) {
    s32 animFrame = o->header.gfx.unk38.animFrame;

    return (animFrame >= startFrame && animFrame < startFrame + rangeLength);
}

s32 mario_is_in_air_action(void) {
    return (gMarioStates[0].action & ACT_FLAG_AIR);
}

void cur_obj_unrender_and_reset_state(s32 sp18, s32 sp1C) {
    cur_obj_become_intangible();
    cur_obj_disable_rendering();

    if (sp18 >= 0) {
        cur_obj_init_animation_with_sound(sp18);
    }

    o->oAction = sp1C;
}

static void cur_obj_move_after_thrown_or_dropped(f32 forwardVel, f32 velY) {
    o->oMoveFlags = 0;
    o->oFloorHeight = find_floor(o->oPosX, o->oPosY + 160.0f, o->oPosZ, &o->oFloorPointer);
    // find_floor(o->oPosX, o->oPosY + 160.0f, o->oPosZ, &o->oFloorPointer);
    if (o->oFloorHeight > o->oPosY) {
        o->oPosY = o->oFloorHeight;
    } else if (o->oFloorHeight < -10000.0f) {
        //! OoB failsafe
        obj_copy_pos(o, gMarioObject);
        o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloorPointer);
    }

    o->oForwardVel = forwardVel;
    o->oVelY = velY;

    if (o->oForwardVel != 0) {
        cur_obj_move_y(/*gravity*/ -4.0f, /*bounciness*/ -0.1f, /*buoyancy*/ 2.0f);
    }
}

void cur_obj_get_thrown_or_placed(f32 forwardVel, f32 velY, s32 thrownAction) {
    cur_obj_become_tangible();
    cur_obj_enable_rendering();

    o->oHeldState = HELD_FREE;

    if ((o->oInteractionSubtype & INT_SUBTYPE_HOLDABLE_NPC) || forwardVel == 0.0f) {
        cur_obj_move_after_thrown_or_dropped(0.0f, 0.0f);
    } else {
        o->oAction = thrownAction;
        cur_obj_move_after_thrown_or_dropped(forwardVel, velY);
    }
}

void cur_obj_get_dropped(void) {
    cur_obj_become_tangible();
    cur_obj_enable_rendering();

    o->oHeldState = HELD_FREE;
    cur_obj_move_after_thrown_or_dropped(0.0f, 0.0f);
}

void cur_obj_set_model(s32 modelID) {
    o->header.gfx.sharedChild = gLoadedGraphNodes[modelID];
}

void mario_set_flag(s32 flag) {
    gMarioStates[0].flags |= flag;
}

s32 cur_obj_clear_interact_status_flag(s32 flag) {
    if (o->oInteractStatus & flag) {
        o->oInteractStatus = 0;
        return TRUE;
    }
    return FALSE;
}

void cur_obj_disable(void) {
    cur_obj_disable_rendering();
    cur_obj_hide();
    cur_obj_become_intangible();
}

void cur_obj_update_floor_height(void) {
    struct Surface *floor;
    o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloorPointer);
}

struct Surface *cur_obj_update_floor_height_and_get_floor(void) {
    o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloorPointer);
    return o->oFloorPointer;
}

static void apply_drag_to_value(f32 *value, f32 dragStrength) {
    f32 decel;

    if (*value != 0) {
        //! Can overshoot if |*value| > 1/(dragStrength * 0.0001)
        decel = (*value) * (*value) * (dragStrength * 0.0001L);

        if (*value > 0) {
            *value -= decel;
            if (*value < 0.001L) {
                *value = 0;
            }
        } else {
            *value += decel;
            if (*value > -0.001L) {
                *value = 0;
            }
        }
    }
}

void cur_obj_apply_drag_xz(f32 dragStrength) {
    apply_drag_to_value(&o->oVelX, dragStrength);
    apply_drag_to_value(&o->oVelZ, dragStrength);
}

static s32 cur_obj_move_xz(f32 steepSlopeNormalY, s32 careAboutEdgesAndSteepSlopes) {
    struct Surface *intendedFloor;

    f32 intendedX = o->oPosX + o->oVelX;
    f32 intendedZ = o->oPosZ + o->oVelZ;

    f32 intendedFloorHeight = find_floor(intendedX, o->oPosY, intendedZ, &intendedFloor);
    f32 deltaFloorHeight = intendedFloorHeight - o->oFloorHeight;

    o->oMoveFlags &= ~OBJ_MOVE_HIT_EDGE;

    if (intendedFloorHeight < -10000.0f) {
        // Don't move into OoB
        o->oMoveFlags |= OBJ_MOVE_HIT_EDGE;
        return FALSE;
    } else if (deltaFloorHeight < 5.0f) {
        if (!careAboutEdgesAndSteepSlopes) {
            // If we don't care about edges or steep slopes, okay to move
            o->oPosX = intendedX;
            o->oPosZ = intendedZ;
            return TRUE;
        } else if (deltaFloorHeight < -50.0f && (o->oMoveFlags & OBJ_MOVE_ON_GROUND)) {
            // Don't walk off an edge
            o->oMoveFlags |= OBJ_MOVE_HIT_EDGE;
            return FALSE;
        } else if (intendedFloor->normal.y > steepSlopeNormalY) {
            // Allow movement onto a slope, provided it's not too steep
            o->oPosX = intendedX;
            o->oPosZ = intendedZ;
            return TRUE;
        } else {
            // We are likely trying to move onto a steep downward slope
            o->oMoveFlags |= OBJ_MOVE_HIT_EDGE;
            return FALSE;
        }
    } else if ((intendedFloor->normal.y) > steepSlopeNormalY || o->oPosY > intendedFloorHeight) {
        // Allow movement upward, provided either:
        // - The target floor is flat enough (e.g. walking up stairs)
        // - We are above the target floor (most likely in the air)
        o->oPosX = intendedX;
        o->oPosZ = intendedZ;
        //! Returning FALSE but moving anyway (not exploitable; return value is
        //  never used)
    }

    // We are likely trying to move onto a steep upward slope
    return FALSE;
}

static void cur_obj_move_update_underwater_flags(void) {
    f32 decelY = (f32) (sqrtf(o->oVelY * o->oVelY) * (o->oDragStrength * 7.0f)) / 100.0L;

    if (o->oVelY > 0) {
        o->oVelY -= decelY;
    } else {
        o->oVelY += decelY;
    }

    if (o->oPosY < o->oFloorHeight) {
        o->oPosY = o->oFloorHeight;
        o->oMoveFlags |= OBJ_MOVE_UNDERWATER_ON_GROUND;
    } else {
        o->oMoveFlags |= OBJ_MOVE_UNDERWATER_OFF_GROUND;
    }
}

static void cur_obj_move_update_ground_air_flags(f32 bounciness) {
    o->oMoveFlags &= ~OBJ_MOVE_13;

    if (o->oPosY < o->oFloorHeight) {
        // On the first frame that we touch the ground, set OBJ_MOVE_LANDED.
        // On subsequent frames, set OBJ_MOVE_ON_GROUND
        if (!(o->oMoveFlags & OBJ_MOVE_ON_GROUND)) {
            if (clear_move_flag(&o->oMoveFlags, OBJ_MOVE_LANDED)) {
                o->oMoveFlags |= OBJ_MOVE_ON_GROUND;
            } else {
                o->oMoveFlags |= OBJ_MOVE_LANDED;
            }
        }

        o->oPosY = o->oFloorHeight;

        if (o->oVelY < 0.0f) {
            o->oVelY *= bounciness;
        }

        if (o->oVelY > 5.0f) {
            //! If OBJ_MOVE_13 tracks bouncing, it overestimates, since velY
            // could be > 5 here without bounce (e.g. jump into misa)
            o->oMoveFlags |= OBJ_MOVE_13;
        }
    } else {
        o->oMoveFlags &= ~OBJ_MOVE_LANDED;
        if (clear_move_flag(&o->oMoveFlags, OBJ_MOVE_ON_GROUND)) {
            o->oMoveFlags |= OBJ_MOVE_LEFT_GROUND;
        }
    }

    o->oMoveFlags &= ~OBJ_MOVE_MASK_IN_WATER;
}

static f32 cur_obj_move_y_and_get_water_level(f32 gravity, f32 buoyancy) {
    f32 waterLevel;

    o->oVelY += gravity + buoyancy;
    if (o->oVelY < -78.0f) {
        o->oVelY = -78.0f;
    }

    o->oPosY += o->oVelY;
    if (o->activeFlags & ACTIVE_FLAG_UNK10) {
        waterLevel = -11000.0f;
    } else {
        waterLevel = find_water_level(o->oPosX, o->oPosZ);
    }
    switch (o->oFloorType) {
        case SURFACE_PAINTING_WARP_F9:
            waterLevel = o->oFloorHeight + 694.f;
            break;
        case SURFACE_PAINTING_WARP_F8:
            waterLevel = o->oFloorHeight + 253.f;
            break;
    }
    return waterLevel;
}

void cur_obj_move_y(f32 gravity, f32 bounciness, f32 buoyancy) {
    f32 waterLevel;

    o->oMoveFlags &= ~OBJ_MOVE_LEFT_GROUND;

    if (o->oMoveFlags & OBJ_MOVE_AT_WATER_SURFACE) {
        if (o->oVelY > 5.0f) {
            o->oMoveFlags &= ~OBJ_MOVE_MASK_IN_WATER;
            o->oMoveFlags |= OBJ_MOVE_LEAVING_WATER;
        }
    }

    if (!(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)) {
        waterLevel = cur_obj_move_y_and_get_water_level(gravity, 0.0f);
        if (o->oPosY > waterLevel) {
            //! We only handle floor collision if the object does not enter
            //  water. This allows e.g. coins to clip through floors if they
            //  enter water on the same frame.
            cur_obj_move_update_ground_air_flags(bounciness);
        } else {
            o->oMoveFlags |= OBJ_MOVE_ENTERED_WATER;
            o->oMoveFlags &= ~OBJ_MOVE_MASK_ON_GROUND;
        }
    } else {
        o->oMoveFlags &= ~OBJ_MOVE_ENTERED_WATER;

        waterLevel = cur_obj_move_y_and_get_water_level(gravity, buoyancy);
        if (o->oPosY < waterLevel) {
            cur_obj_move_update_underwater_flags();
        } else {
            if (o->oPosY < o->oFloorHeight) {
                o->oPosY = o->oFloorHeight;
                o->oMoveFlags &= ~OBJ_MOVE_MASK_IN_WATER;
            } else {
                o->oPosY = waterLevel;
                o->oVelY = 0.0f;
                o->oMoveFlags &= ~(OBJ_MOVE_UNDERWATER_OFF_GROUND | OBJ_MOVE_UNDERWATER_ON_GROUND);
                o->oMoveFlags |= OBJ_MOVE_AT_WATER_SURFACE;
            }
        }
    }

    if (o->oMoveFlags & OBJ_MOVE_MASK_33) {
        o->oMoveFlags &= ~OBJ_MOVE_IN_AIR;
    } else {
        o->oMoveFlags |= OBJ_MOVE_IN_AIR;
    }
}

static s32 clear_move_flag(u32 *bitSet, s32 flag) {
    if (*bitSet & flag) {
        *bitSet &= flag ^ 0xFFFFFFFF;
        return TRUE;
    } else {
        return FALSE;
    }
}

void cur_obj_resolve_wall_coll_simple(f32 offsetY, f32 radius) {
    if (radius > 0.1L) {
        f32_find_wall_collision(&o->oPosX, &o->oPosY, &o->oPosZ, offsetY, radius);
    }
}

s16 abs_angle_diff(s16 x0, s16 x1) {
    register s16 diff = x1 - x0;

    if (diff == -0x8000) {
        diff = -0x7FFF;
    }

    if (diff < 0) {
        diff = -diff;
    }

    return diff;
}

void cur_obj_move_xz_using_fvel_and_yaw(void) {
    o->oVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);

    o->oPosX += o->oVelX;
    o->oPosZ += o->oVelZ;
}

void cur_obj_move_y_with_terminal_vel(void) {
    if (o->oVelY < -70.0f) {
        o->oVelY = -70.0f;
    }

    o->oPosY += o->oVelY;
}

void cur_obj_compute_vel_xz(void) {
    o->oVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);
}

f32 increment_velocity_toward_range(f32 value, f32 center, f32 zeroThreshold, f32 increment) {
    f32 relative;
    if ((relative = value - center) > 0) {
        if (relative < zeroThreshold) {
            return 0.0f;
        } else {
            return -increment;
        }
    } else {
        if (relative > -zeroThreshold) {
            return 0.0f;
        } else {
            return increment;
        }
    }
}

s32 obj_check_if_collided_with_object(struct Object *obj1, struct Object *obj2) {
    s32 i;
    for (i = 0; i < obj1->numCollidedObjs; i++) {
        if (obj1->collidedObjs[i] == obj2) {
            return TRUE;
        }
    }

    return FALSE;
}

void cur_obj_set_behavior(const BehaviorScript *behavior) {
    o->behavior = segmented_to_virtual(behavior);
}
s32 cur_obj_has_behavior(const BehaviorScript *behavior) {
    return (o->behavior == segmented_to_virtual(behavior));
}

s32 obj_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    return (obj->behavior == segmented_to_virtual(behavior));
}

f32 cur_obj_lateral_dist_from_mario_to_home(void) {
    f32 dx = o->oHomeX - gMarioObject->oPosX;
    f32 dz = o->oHomeZ - gMarioObject->oPosZ;

    return sqrtf(dx * dx + dz * dz);
}

f32 cur_obj_lateral_dist_to_home(void) {
    f32 dx = o->oHomeX - o->oPosX;
    f32 dz = o->oHomeZ - o->oPosZ;

    return sqrtf(dx * dx + dz * dz);
}

void cur_obj_set_pos_to_home(void) {
    o->oPosX = o->oHomeX;
    o->oPosY = o->oHomeY;
    o->oPosZ = o->oHomeZ;
}

void cur_obj_set_pos_to_home_and_stop(void) {
    cur_obj_set_pos_to_home();

    o->oForwardVel = 0;
    o->oVelY = 0;
}

void cur_obj_shake_y(f32 amount) {
    //! Technically could cause a bit of drift, but not much
    if (o->oTimer % 2 == 0) {
        o->oPosY += amount;
    } else {
        o->oPosY -= amount;
    }
}

void obj_set_billboard(struct Object *obj) {
    obj->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;
}

static void obj_spawn_loot_coins(struct Object *obj, s32 numCoins, f32 sp30,
                                 const BehaviorScript *coinBehavior, s16 posJitter, s16 model) {
    s32 i;
    f32 spawnHeight;
    struct Surface *floor;
    struct Object *coin;

    spawnHeight = find_floor(obj->oPosX, obj->oPosY, obj->oPosZ, &floor);
    if (obj->oPosY - spawnHeight > 100.0f) {
        spawnHeight = obj->oPosY;
    }

    for (; numCoins > 0; numCoins--) {
        if (obj->oNumLootCoins <= 0) {
            break;
        }

        obj->oNumLootCoins--;

        coin = spawn_object(obj, model, coinBehavior);
        obj_translate_xz_random(coin, posJitter);
        coin->oPosY = spawnHeight;
        coin->oCoinUnk110 = sp30;
    }
}

#define obj_spawn_loot_blue_coins(obj, numCoins, sp28, posJitter)                                      \
    { obj_spawn_loot_coins(obj, numCoins, sp28, bhvBlueCoinJumping, posJitter, MODEL_BLUE_COIN); }

void obj_spawn_loot_yellow_coins(struct Object *obj, s32 numCoins, f32 sp28) {
    obj_spawn_loot_coins(obj, numCoins, sp28, bhvSingleCoinGetsSpawned, 0, MODEL_YELLOW_COIN);
}

void cur_obj_spawn_loot_coin_at_mario_pos(void) {
    struct Object *coin;
    if (o->oNumLootCoins <= 0) {
        return;
    }

    o->oNumLootCoins--;

    coin = spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
    coin->oVelY = 30.0f;

    obj_copy_pos(coin, gMarioObject);
}

f32 cur_obj_abs_y_dist_to_home(void) {

    return absf(o->oHomeY - o->oPosY);
}

s32 cur_obj_advance_looping_anim(void) {
    s32 spC = o->header.gfx.unk38.animFrame;
    s32 sp8 = o->header.gfx.unk38.curAnim->unk08;
    s32 sp4;

    if (spC < 0) {
        spC = 0;
    } else if (sp8 - 1 == spC) {
        spC = 0;
    } else {
        spC++;
    }

    sp4 = (spC << 16) / sp8;

    return sp4;
}

static s32 cur_obj_detect_steep_floor(s32 steepAngleDegrees) {
    struct Surface *intendedFloor;
    f32 intendedX, intendedFloorHeight, intendedZ;
    f32 deltaFloorHeight;
    f32 steepNormalY = coss((s16) (steepAngleDegrees * (0x10000 / 360)));

    if (o->oForwardVel != 0) {
        intendedX = o->oPosX + o->oVelX;
        intendedZ = o->oPosZ + o->oVelZ;
        intendedFloorHeight = find_floor(intendedX, o->oPosY, intendedZ, &intendedFloor);
        deltaFloorHeight = intendedFloorHeight - o->oFloorHeight;

        if (intendedFloorHeight < -10000.0f) {
            o->oWallAngle = o->oMoveAngleYaw + 0x8000;
            return 2;
        } else if (intendedFloor->normal.y < steepNormalY && deltaFloorHeight > 0
                   && intendedFloorHeight > o->oPosY) {
            o->oWallAngle = atan2s(intendedFloor->normal.z, intendedFloor->normal.x);
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}

s32 cur_obj_resolve_wall_collisions(void) {
    s32 numCollisions;
    struct Surface *wall;
    struct WallCollisionData collisionData;

    f32 offsetY = 20.0f;
    f32 radius = o->oWallHitboxRadius;

    if (radius > 0.1L) {
        collisionData.offsetY = offsetY;
        collisionData.radius = radius;
        collisionData.x = (s16) o->oPosX;
        collisionData.y = (s16) o->oPosY;
        collisionData.z = (s16) o->oPosZ;

        numCollisions = find_wall_collisions(&collisionData);
        if (numCollisions != 0) {
            o->oPosX = collisionData.x;
            o->oPosY = collisionData.y;
            o->oPosZ = collisionData.z;
            wall = collisionData.walls[collisionData.numWalls - 1];

            o->oWallAngle = atan2s(wall->normal.z, wall->normal.x);
            if (abs_angle_diff(o->oWallAngle, o->oMoveAngleYaw) > 0x4000) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    return 0;
}

void cur_obj_update_floor(void) {
    struct Surface *floor = cur_obj_update_floor_height_and_get_floor();
    o->oFloor = floor;

    if (floor != NULL) {
        if (floor->type & (SPECFLAG_BURNING << 8)) {
            o->oMoveFlags |= OBJ_MOVE_ABOVE_LAVA;
        } else if (SURFACETYPE(floor) == SURFACE_DEATH_PLANE) {
            //! This misses SURFACE_VERTICAL_WIND (and maybe SURFACE_WARP)
            o->oMoveFlags |= OBJ_MOVE_ABOVE_DEATH_BARRIER;
        }

        o->oFloorType = floor->type;
        // o->oFloorRoom = floor->room;
    } else {
        o->oFloorType = 0;
        // o->oFloorRoom = 0;
    }
}

static void cur_obj_update_floor_and_resolve_wall_collisions(s32 steepSlopeDegrees) {
    o->oMoveFlags &= ~(OBJ_MOVE_ABOVE_LAVA | OBJ_MOVE_ABOVE_DEATH_BARRIER);

    if (o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        cur_obj_update_floor();
        o->oMoveFlags &= ~OBJ_MOVE_MASK_HIT_WALL_OR_IN_WATER;

        if (o->oPosY > o->oFloorHeight) {
            o->oMoveFlags |= OBJ_MOVE_IN_AIR;
        }
    } else {
        o->oMoveFlags &= ~OBJ_MOVE_HIT_WALL;
        if (cur_obj_resolve_wall_collisions()) {
            o->oMoveFlags |= OBJ_MOVE_HIT_WALL;
        }

        cur_obj_update_floor();

        if (o->oPosY > o->oFloorHeight) {
            o->oMoveFlags |= OBJ_MOVE_IN_AIR;
        }

        if (cur_obj_detect_steep_floor(steepSlopeDegrees)) {
            o->oMoveFlags |= OBJ_MOVE_HIT_WALL;
        }
    }
}

void cur_obj_update_floor_and_walls(void) {
    cur_obj_update_floor_and_resolve_wall_collisions(60);
}

void cur_obj_move_standard(s32 steepSlopeAngleDegrees) {
    f32 gravity = o->oGravity;
    f32 bounciness = o->oBounciness;
    f32 buoyancy = o->oBuoyancy;
    f32 dragStrength = o->oDragStrength;
    f32 steepSlopeNormalY;
    s32 careAboutEdgesAndSteepSlopes = FALSE;
    s32 negativeSpeed = FALSE;

    //! Because some objects allow these active flags to be set but don't
    //  avoid updating when they are, we end up with "partial" updates, where
    //  an object's internal state will be updated, but it doesn't move.
    //  This allows numerous glitches and is typically referred to as
    //  deactivation (though this term has a different meaning in the code).
    //  Objects that do this will be marked with //PARTIAL_UPDATE.
    if (!(o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))) {
        if (steepSlopeAngleDegrees < 0) {
            // clang-format off
            careAboutEdgesAndSteepSlopes = TRUE; steepSlopeAngleDegrees = -steepSlopeAngleDegrees;
            // clang-format on
        }

        steepSlopeNormalY = coss(steepSlopeAngleDegrees * (0x10000 / 360));

        cur_obj_compute_vel_xz();
        cur_obj_apply_drag_xz(dragStrength);

        cur_obj_move_xz(steepSlopeNormalY, careAboutEdgesAndSteepSlopes);
        cur_obj_move_y(gravity, bounciness, buoyancy);

        if (o->oForwardVel < 0) {
            negativeSpeed = TRUE;
        }
        o->oForwardVel = sqrtf(sqr(o->oVelX) + sqr(o->oVelZ));
        if (negativeSpeed) {
            o->oForwardVel = -o->oForwardVel;
        }
    }
}

void cur_obj_move_using_vel_and_gravity(void) {
    o->oPosX += o->oVelX;
    o->oPosZ += o->oVelZ;
    o->oVelY += o->oGravity; //! No terminal velocity
    o->oPosY += o->oVelY;
}

void cur_obj_move_using_fvel_and_gravity(void) {
    cur_obj_compute_vel_xz();
    cur_obj_move_using_vel_and_gravity(); //! No terminal velocity
}

void obj_set_pos_relative(struct Object *obj, struct Object *other, f32 dleft, f32 dy, f32 dforward) {
    f32 facingZ = coss(other->oMoveAngleYaw);
    f32 facingX = sins(other->oMoveAngleYaw);

    f32 dz = dforward * facingZ - dleft * facingX;
    f32 dx = dforward * facingX + dleft * facingZ;

    obj->oMoveAngleYaw = other->oMoveAngleYaw;

    obj->oPosX = other->oPosX + dx;
    obj->oPosY = other->oPosY + dy;
    obj->oPosZ = other->oPosZ + dz;
}

s32 cur_obj_angle_to_home(void) {

    return atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX);
}

void obj_set_gfx_pos_at_obj_pos(struct Object *obj1, struct Object *obj2) {
    obj1->header.gfx.pos[0] = obj2->oPosX;
    obj1->header.gfx.pos[1] = obj2->oPosY + obj2->oGraphYOffset;
    obj1->header.gfx.pos[2] = obj2->oPosZ;

    obj1->header.gfx.angle[0] = obj2->oMoveAnglePitch & 0xFFFF;
    obj1->header.gfx.angle[1] = obj2->oMoveAngleYaw & 0xFFFF;
    obj1->header.gfx.angle[2] = obj2->oMoveAngleRoll & 0xFFFF;
}

/**
 * Transform the vector at localTranslateIndex into the object's local
 * coordinates, and then add it to the vector at posIndex.
 */
void obj_translate_local(struct Object *obj, s16 posIndex, s16 localTranslateIndex) {
    f32 dx = obj->rawData.asF32[localTranslateIndex + 0];
    f32 dy = obj->rawData.asF32[localTranslateIndex + 1];
    f32 dz = obj->rawData.asF32[localTranslateIndex + 2];

    obj->rawData.asF32[posIndex + 0] +=
        obj->transform[0][0] * dx + obj->transform[1][0] * dy + obj->transform[2][0] * dz;
    obj->rawData.asF32[posIndex + 1] +=
        obj->transform[0][1] * dx + obj->transform[1][1] * dy + obj->transform[2][1] * dz;
    obj->rawData.asF32[posIndex + 2] +=
        obj->transform[0][2] * dx + obj->transform[1][2] * dy + obj->transform[2][2] * dz;
}

void obj_build_transform_from_pos_and_angle(struct Object *obj, s16 posIndex, s16 angleIndex) {
    f32 translate[3];
    s16 rotation[3];
    s32 j;
    for (j = 0; j < 3; j++) {
        translate[j] = obj->rawData.asF32[posIndex + j];
        rotation[j] = obj->rawData.asS32[angleIndex + j];
    }

    mtxf_rotate_zxy_and_translate(obj->transform, translate, rotation);
}

void obj_set_throw_matrix_from_transform(struct Object *obj) {
    if (obj->oFlags & OBJ_FLAG_0020) {
        obj_build_transform_from_pos_and_angle(obj, O_POS_INDEX, O_FACE_ANGLE_INDEX);
        obj_apply_scale_to_transform(obj);
    }

    obj->header.gfx.throwMatrix = &obj->transform;
}

void obj_build_transform_relative_to_parent(struct Object *obj) {
    struct Object *parent = obj->parentObj;

    obj_build_transform_from_pos_and_angle(obj, O_PARENT_RELATIVE_POS_INDEX, O_FACE_ANGLE_INDEX);
    obj_apply_scale_to_transform(obj);
    mtxf_mul(obj->transform, obj->transform, parent->transform);

    obj->oPosX = obj->transform[3][0];
    obj->oPosY = obj->transform[3][1];
    obj->oPosZ = obj->transform[3][2];
    obj->header.gfx.throwMatrix = &obj->transform;

    //! Sets scale of gCurrentObject instead of obj. Not exploitable since this
    //  function is only called with obj = gCurrentObject
    obj_scale(obj, 1.0f);
}

void obj_create_transform_from_self(struct Object *obj) {
    obj->oFlags &= ~OBJ_FLAG_TRANSFORM_RELATIVE_TO_PARENT;
    obj->oFlags |= OBJ_FLAG_SET_THROW_MATRIX_FROM_TRANSFORM;

    obj->transform[3][0] = obj->oPosX;
    obj->transform[3][1] = obj->oPosY;
    obj->transform[3][2] = obj->oPosZ;
}

void cur_obj_rotate_move_angle_using_vel(void) {
    o->oMoveAnglePitch += o->oAngleVelPitch;
    o->oMoveAngleYaw += o->oAngleVelYaw;
    o->oMoveAngleRoll += o->oAngleVelRoll;
}

void cur_obj_rotate_face_angle_using_vel(void) {
    o->oFaceAnglePitch += o->oAngleVelPitch;
    o->oFaceAngleYaw += o->oAngleVelYaw;
    o->oFaceAngleRoll += o->oAngleVelRoll;
}

void cur_obj_set_face_angle_to_move_angle(void) {
    o->oFaceAnglePitch = o->oMoveAnglePitch;
    o->oFaceAngleYaw = o->oMoveAngleYaw;
    o->oFaceAngleRoll = o->oMoveAngleRoll;
}

s32 cur_obj_follow_path(UNUSED s32 unusedArg) {
    struct Waypoint *startWaypoint;
    struct Waypoint *lastWaypoint;
    struct Waypoint *targetWaypoint;
    f32 prevToNextX, prevToNextY, prevToNextZ;
    f32 objToNextXZ;
    f32 objToNextX, objToNextY, objToNextZ;

    if (o->oPathedPrevWaypointFlags == 0) {
        o->oPathedPrevWaypoint = o->oPathedStartWaypoint;
        o->oPathedPrevWaypointFlags = WAYPOINT_FLAGS_INITIALIZED;
    }

    startWaypoint = o->oPathedStartWaypoint;
    lastWaypoint = o->oPathedPrevWaypoint;

    if ((lastWaypoint + 1)->flags != WAYPOINT_FLAGS_END) {
        targetWaypoint = lastWaypoint + 1;
    } else {
        targetWaypoint = startWaypoint;
    }

    o->oPathedPrevWaypointFlags = lastWaypoint->flags | WAYPOINT_FLAGS_INITIALIZED;

    prevToNextX = targetWaypoint->pos[0] - lastWaypoint->pos[0];
    prevToNextY = targetWaypoint->pos[1] - lastWaypoint->pos[1];
    prevToNextZ = targetWaypoint->pos[2] - lastWaypoint->pos[2];

    objToNextX = targetWaypoint->pos[0] - o->oPosX;
    objToNextY = targetWaypoint->pos[1] - o->oPosY;
    objToNextZ = targetWaypoint->pos[2] - o->oPosZ;
    objToNextXZ = sqrtf(sqr(objToNextX) + sqr(objToNextZ));

    o->oPathedTargetYaw = atan2s(objToNextZ, objToNextX);
    o->oPathedTargetPitch = atan2s(objToNextXZ, -objToNextY);

    // If dot(prevToNext, objToNext) <= 0 (i.e. reached other side of target waypoint)
    if (prevToNextX * objToNextX + prevToNextY * objToNextY + prevToNextZ * objToNextZ <= 0.0f) {
        o->oPathedPrevWaypoint = targetWaypoint;
        if ((targetWaypoint + 1)->flags == WAYPOINT_FLAGS_END) {
            return PATH_REACHED_END;
        } else {
            return PATH_REACHED_WAYPOINT;
        }
    }

    return PATH_NONE;
}

void chain_segment_init(struct ChainSegment *segment) {
    segment->posX = 0.0f;
    segment->posY = 0.0f;
    segment->posZ = 0.0f;

    segment->pitch = 0;
    segment->yaw = 0;
    segment->roll = 0;
}

f32 random_f32_around_zero(f32 diameter) {
    return random_float() * diameter - diameter / 2;
}

void obj_scale_random(struct Object *obj, f32 rangeLength, f32 minScale) {
    f32 scale = random_float() * rangeLength + minScale;
    obj_scale_xyz(obj, scale, scale, scale);
}

void obj_translate_xyz_random(struct Object *obj, f32 rangeLength) {
    s32 j;
    for (j = 0; j < 3; j++) {
        obj->OBJECT_FIELD_F32(6 + j) += random_float() * rangeLength - rangeLength * 0.5f;
    }
}

void obj_translate_xz_random(struct Object *obj, f32 rangeLength) {
    obj->oPosX += random_float() * rangeLength - rangeLength * 0.5f;
    obj->oPosZ += random_float() * rangeLength - rangeLength * 0.5f;
}

void cur_obj_set_pos_via_transform(void) {
    obj_build_transform_from_pos_and_angle(o, O_PARENT_RELATIVE_POS_INDEX, O_MOVE_ANGLE_INDEX);
    o->oPosX += o->oVelX;
    o->oPosY += o->oVelY;
    o->oPosZ += o->oVelZ;
}

s16 cur_obj_reflect_move_angle_off_wall(void) {
    return o->oWallAngle - ((s16) o->oMoveAngleYaw - (s16) o->oWallAngle) + 0x8000;
}

void cur_obj_spawn_particles(struct SpawnParticlesInfo *info) {
    struct Object *particle;
    s32 i;
    f32 scale;
    s32 numParticles = info->count;

    // If there are a lot of objects already, limit the number of particles
    /*if (gPrevFrameObjectCount > 150 && numParticles > 10) {
        numParticles = 10;
    }*/

    // We're close to running out of object slots, so don't spawn particles at
    // all
    /*if (gPrevFrameObjectCount > 210) {
        numParticles = 0;
    }*/

    for (i = 0; i < numParticles; i++) {
        scale = random_float() * (info->sizeRange * 0.1f) + info->sizeBase * 0.1f;

        particle = spawn_object(o, info->model, bhvWhitePuffExplosion);

        particle->oBehParams2ndByte = info->behParam;
        particle->oMoveAngleYaw = random_u16();
        particle->oGravity = info->gravity;
        particle->oDragStrength = info->dragStrength;

        particle->oPosY += info->offsetY;
        particle->oForwardVel = random_float() * info->forwardVelRange + info->forwardVelBase;
        particle->oVelY = random_float() * info->velYRange + info->velYBase;
        particle->oOpacity = 0xAF;
        obj_scale_xyz(particle, scale, scale, scale);
    }
}

void obj_set_hitbox(struct Object *obj, struct ObjectHitbox *hitbox) {
    if (!(obj->oFlags & OBJ_FLAG_30)) {
        obj->oFlags |= OBJ_FLAG_30;

        obj->oInteractType = hitbox->interactType;
        obj->oDamageOrCoinValue = hitbox->damageOrCoinValue;
        obj->oHealth = hitbox->health;
        obj->oNumLootCoins = hitbox->numLootCoins;

        obj->oIntangibleTimer = 0;
    }

    obj->hitboxRadius = obj->header.gfx.scale[0] * hitbox->radius;
    obj->hitboxHeight = obj->header.gfx.scale[1] * hitbox->height;
    obj->hurtboxRadius = obj->header.gfx.scale[0] * hitbox->hurtboxRadius;
    obj->hurtboxHeight = obj->header.gfx.scale[1] * hitbox->hurtboxHeight;
    obj->hitboxDownOffset = obj->header.gfx.scale[1] * hitbox->downOffset;
}

s32 signum_positive(s32 x) {
    if (x >= 0) {
        return 1;
    } else {
        return -1;
    }
}

s32 absi(s32 a0) {
    if (a0 >= 0) {
        return a0;
    } else {
        return -a0;
    }
}

s32 cur_obj_wait_then_blink(s32 timeUntilBlinking, s32 numBlinks) {
    s32 timeBlinking;

    if (o->oTimer >= timeUntilBlinking) {
        if ((timeBlinking = o->oTimer - timeUntilBlinking) % 2 != 0) {
            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            if (timeBlinking / 2 > numBlinks) {
                return TRUE;
            }
        } else {
            o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    }

    return 0;
}

s32 cur_obj_is_mario_ground_pounding_platform(void) {
    if (gMarioObject->platform == o) {
        if (gMarioStates[0].action == ACT_GROUND_POUND_LAND) {
            return TRUE;
        }
    }

    return FALSE;
}

void spawn_mist_particles(void) {
    spawn_mist_particles_variable(0, 0, 46.0f);
}

void spawn_mist_particles_with_sound(u32 sp18) {
    spawn_mist_particles_variable(0, 0, 46.0f);
    create_sound_spawner(sp18);
}

void cur_obj_push_mario_away(f32 radius) {
    f32 marioRelX = gMarioObject->oPosX - o->oPosX;
    f32 marioRelZ = gMarioObject->oPosZ - o->oPosZ;
    f32 marioDist = sqrtf(sqr(marioRelX) + sqr(marioRelZ));

    if (marioDist < radius) {
        //! If this function pushes Mario out of bounds, it will trigger Mario's
        //  oob failsafe
        gMarioStates[0].pos[0] += (radius - marioDist) / radius * marioRelX;
        gMarioStates[0].pos[2] += (radius - marioDist) / radius * marioRelZ;
    }
}

void cur_obj_push_mario_away_from_cylinder(f32 radius, f32 extentY) {
    f32 marioRelY = gMarioObject->oPosY - o->oPosY;

    if (marioRelY < 0) {
        marioRelY = -marioRelY;
    }

    if (marioRelY < extentY) {
        cur_obj_push_mario_away(radius);
    }
}

void bhv_dust_smoke_loop(void) {
    o->oPosX += o->oVelX;
    o->oPosY += o->oVelY;
    o->oPosZ += o->oVelZ;

    if (o->oSmokeTimer == 10) {
        obj_mark_for_deletion(o);
    }

    o->oSmokeTimer++;
}

void bhv_backdust_init(void) {
    o->oPosX += (s32) (random_float() * 80.0f) - 40;
    o->oPosY += (s32) (random_float() * 80.0f) - 40;
    o->oPosZ += (s32) (random_float() * 80.0f) - 40;
    o->oVelY = 0.f;
    o->oForwardVel = -20.f;
    cur_obj_scale(1.2f);
}

void bhv_backdust_loop(void) {
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.0f, 2.0f);
    o->oVelY += 1.0f;
    o->oPosY += o->oVelY;
    if (o->oSmokeTimer == 10) {
        obj_mark_for_deletion(o);
    }
    o->oSmokeTimer++;
}
s32 cur_obj_progress_direction_table(void) {
}

void cur_obj_scale_over_time(s32 a0, s32 a1, f32 sp10, f32 sp14) {
    f32 sp4 = sp14 - sp10;
    f32 sp0 = (f32) o->oTimer / a1;

    if (a0 & 0x01) {
        o->header.gfx.scale[0] = sp4 * sp0 + sp10;
    }

    if (a0 & 0x02) {
        o->header.gfx.scale[1] = sp4 * sp0 + sp10;
    }

    if (a0 & 0x04) {
        o->header.gfx.scale[2] = sp4 * sp0 + sp10;
    }
}

s32 cur_obj_is_mario_on_platform(void) {
    if (gMarioObject->platform == o) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 cur_obj_shake_y_until(s32 cycles, s32 amount) {
    if (o->oTimer % 2 != 0) {
        o->oPosY -= amount;
    } else {
        o->oPosY += amount;
    }

    if (o->oTimer == cycles * 2) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 cur_obj_move_up_and_down(s32 a0) {
    if (a0 >= 4 || a0 < 0) {
        return 1;
    }

    o->oPosY += D_8032F0A0[a0];
    return 0;
}

void cur_obj_call_action_function(void (*actionFunctions[])(void)) {
    void (*actionFunction)(void) = actionFunctions[o->oAction];
    actionFunction();
}

static struct Object *spawn_star_with_no_lvl_exit(s32 sp20, s32 sp24) {
    struct Object *sp1C = spawn_object(o, MODEL_STAR, bhvSpawnedStarNoLevelExit);
    sp1C->oSparkleSpawnUnk1B0 = sp24;
    sp1C->oBehParams = o->oBehParams;
    sp1C->oBehParams2ndByte = sp20;

    return sp1C;
}

s32 cur_obj_mario_far_away(f32 max) {
    f32 dx = o->oHomeX - gMarioObject->oPosX;
    f32 dy = o->oHomeY - gMarioObject->oPosY;
    f32 dz = o->oHomeZ - gMarioObject->oPosZ;
    f32 marioDistToHome = sqrtf(dx * dx + dy * dy + dz * dz);

    if (marioDistToHome > max) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 is_mario_moving_fast_or_in_air(s32 speedThreshold) {
    if (gMarioStates[0].forwardVel > speedThreshold) {
        return TRUE;
    }

    if (gMarioStates[0].action & ACT_FLAG_AIR) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 cur_obj_set_hitbox_and_die_if_attacked(struct ObjectHitbox *hitbox, s32 deathSound,
                                           s32 noLootCoins) {
    s32 interacted = FALSE;

    obj_set_hitbox(o, hitbox);

    if (noLootCoins) {
        o->oNumLootCoins = 0;
    }

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        if (o->oInteractStatus & INT_STATUS_WAS_ATTACKED) {
            spawn_mist_particles();
            obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);
            obj_mark_for_deletion(o);
            create_sound_spawner(deathSound);
        } else {
            interacted = TRUE;
        }
    }

    o->oInteractStatus = 0;
    return interacted;
}

void obj_explode_and_spawn_coins(f32 sp18, s32 sp1C) {
    spawn_mist_particles_variable(0, 0, sp18);
    spawn_triangle_break_particles(30, 138, 3.0f, 4);
    obj_mark_for_deletion(o);

    if (sp1C == 1) {
        obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);
    } else if (sp1C == 2) {
        obj_spawn_loot_blue_coins(o, o->oNumLootCoins, 20.0f, 150);
    }
}

void obj_set_collision_data(struct Object *obj, const void *segAddr) {
    obj->collisionData = segmented_to_virtual(segAddr);
}

void cur_obj_if_hit_wall_bounce_away(void) {
    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        o->oMoveAngleYaw = o->oWallAngle;
    }
}

s32 cur_obj_hide_if_mario_far_away_y(f32 distY) {
    if (absf(o->oPosY - gMarioObject->oPosY) < distY) {
        cur_obj_unhide();
        return FALSE;
    } else {
        cur_obj_hide();
        return TRUE;
    }
}

Gfx *geo_offset_klepto_held_object(s32 callContext, struct GraphNode *node, UNUSED Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        ((struct GraphNodeTranslationRotation *) node->next)->translation[0] = 300;
        ((struct GraphNodeTranslationRotation *) node->next)->translation[1] = 300;
        ((struct GraphNodeTranslationRotation *) node->next)->translation[2] = 0;
    }

    return NULL;
}

s32 obj_is_hidden(struct Object *obj) {
    if (obj->header.gfx.node.flags & GRAPH_RENDER_INVISIBLE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void enable_time_stop(void) {
    gTimeStopState |= TIME_STOP_ENABLED;
}

void disable_time_stop(void) {
    gTimeStopState &= ~TIME_STOP_ENABLED;
}

void set_time_stop_flags(s32 flags) {
    gTimeStopState |= flags;
}

void clear_time_stop_flags(s32 flags) {
    gTimeStopState = gTimeStopState & (flags ^ 0xFFFFFFFF);
}

s32 cur_obj_can_mario_activate_textbox(f32 radius, f32 height, UNUSED s32 unused) {
    f32 latDistToMario;

    if (o->oDistanceToMario < 1500.0f) {
        latDistToMario = lateral_dist_between_objects(o, gMarioObject);

        if (latDistToMario < radius && o->oPosY < gMarioObject->oPosY + 160.0f
            && gMarioObject->oPosY < o->oPosY + height && !(gMarioStates[0].action & ACT_FLAG_AIR)
            && mario_ready_to_speak()) {
            return TRUE;
        }
    }

    return FALSE;
}

s32 cur_obj_can_mario_activate_textbox_2(f32 radius, f32 height) {
    // The last argument here is unused. When this function is called directly the argument is
    // always set to 0x7FFF.
    return cur_obj_can_mario_activate_textbox(radius, height, 0x1000);
}

static void cur_obj_end_dialog(s32 dialogFlags, s32 dialogResult) {
    o->oDialogResponse = dialogResult;
    o->oDialogState++;

    if (!(dialogFlags & DIALOG_UNK1_FLAG_4)) {
        set_mario_npc_dialog(0);
    }
}

s32 cur_obj_update_dialog(s32 actionArg, s32 dialogFlags, s32 dialogID, UNUSED s32 unused) {
    s32 dialogResponse = 0;

    switch (o->oDialogState) {
#ifdef VERSION_JP
        case DIALOG_UNK1_ENABLE_TIME_STOP:
            //! We enable time stop even if Mario is not ready to speak. This
            //  allows us to move during time stop as long as Mario never enters
            //  an action that can be interrupted with text.
            if (gMarioState->health >= 0x100) {
                gTimeStopState |= TIME_STOP_ENABLED;
                o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                o->oDialogState++;
            }
            break;
#else
        case DIALOG_UNK1_ENABLE_TIME_STOP:
            // Patched :(
            // Wait for Mario to be ready to speak, and then enable time stop
            if (mario_ready_to_speak() || gMarioState->action == ACT_READING_NPC_DIALOG) {
                gTimeStopState |= TIME_STOP_ENABLED;
                o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                o->oDialogState++;
            } else {
                break;
            }
            // Fall through so that Mario's action is interrupted immediately
            // after time is stopped
#endif

        case DIALOG_UNK1_INTERRUPT_MARIO_ACTION:
            if (set_mario_npc_dialog(actionArg) == 2) {
                o->oDialogState++;
            }
            break;

        case DIALOG_UNK1_BEGIN_DIALOG:
            if (dialogFlags & DIALOG_UNK1_FLAG_RESPONSE) {
                create_dialog_box_with_response(dialogID);
            } else if (dialogFlags & DIALOG_UNK1_FLAG_DEFAULT) {
                create_dialog_box(dialogID);
            }
            o->oDialogState++;
            break;

        case DIALOG_UNK1_AWAIT_DIALOG:
            if (dialogFlags & DIALOG_UNK1_FLAG_RESPONSE) {
                if (gDialogResponse != 0) {
                    cur_obj_end_dialog(dialogFlags, gDialogResponse);
                }
            } else if (dialogFlags & DIALOG_UNK1_FLAG_DEFAULT) {
                if (get_dialog_id() == -1) {
                    cur_obj_end_dialog(dialogFlags, 3);
                }
            } else {
                cur_obj_end_dialog(dialogFlags, 3);
            }
            break;

        case DIALOG_UNK1_DISABLE_TIME_STOP:
            if (gMarioState->action != ACT_READING_NPC_DIALOG || (dialogFlags & DIALOG_UNK1_FLAG_4)) {
                gTimeStopState &= ~TIME_STOP_ENABLED;
                o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
                dialogResponse = o->oDialogResponse;
                o->oDialogState = DIALOG_UNK1_ENABLE_TIME_STOP;
            }
            break;

        default:
            o->oDialogState = DIALOG_UNK1_ENABLE_TIME_STOP;
            break;
    }

    return dialogResponse;
}

s32 cur_obj_update_dialog_with_cutscene(s32 actionArg, s32 dialogFlags, s32 cutsceneTable,
                                        s32 dialogID) {
    s32 dialogResponse = 0;
    s32 doneTurning = TRUE;

    switch (o->oDialogState) {
#ifdef VERSION_JP
        case DIALOG_UNK2_ENABLE_TIME_STOP:
            //! We enable time stop even if Mario is not ready to speak. This
            //  allows us to move during time stop as long as Mario never enters
            //  an action that can be interrupted with text.
            if (gMarioState->health >= 0x0100) {
                gTimeStopState |= TIME_STOP_ENABLED;
                o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                o->oDialogState++;
                o->oDialogResponse = 0;
            }
            break;
#else
        case DIALOG_UNK2_ENABLE_TIME_STOP:
            // Wait for Mario to be ready to speak, and then enable time stop
            if (mario_ready_to_speak() || gMarioState->action == ACT_READING_NPC_DIALOG) {
                gTimeStopState |= TIME_STOP_ENABLED;
                o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                o->oDialogState++;
                o->oDialogResponse = 0;
            } else {
                break;
            }
            // Fall through so that Mario's action is interrupted immediately
            // after time is stopped
#endif

        case DIALOG_UNK2_TURN_AND_INTERRUPT_MARIO_ACTION:
            if (dialogFlags & DIALOG_UNK2_FLAG_0) {
                doneTurning = cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), 0x800);
                if (o->oDialogResponse >= 0x21) {
                    doneTurning = TRUE;
                }
            }

            if (set_mario_npc_dialog(actionArg) == 2 && doneTurning) {
                o->oDialogResponse = 0;
                o->oDialogState++;
            } else {
                o->oDialogResponse++;
            }
            break;

        case DIALOG_UNK2_AWAIT_DIALOG:
            if (cutsceneTable == CUTSCENE_CAP_SWITCH_PRESS) {
                if ((o->oDialogResponse = cutscene_object_with_dialog(cutsceneTable, o, -1)) != 0) {
                    o->oDialogState++;
                }
            } else {
                if ((o->oDialogResponse = cutscene_object_with_dialog(cutsceneTable, o, dialogID))
                    != 0) {
                    o->oDialogState++;
                }
            }
            break;

        case DIALOG_UNK2_END_DIALOG:
            if (dialogFlags & DIALOG_UNK2_LEAVE_TIME_STOP_ENABLED) {
                dialogResponse = o->oDialogResponse;
                o->oDialogState = DIALOG_UNK2_ENABLE_TIME_STOP;
            } else if (gMarioState->action != ACT_READING_NPC_DIALOG) {
                gTimeStopState &= ~TIME_STOP_ENABLED;
                o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
                dialogResponse = o->oDialogResponse;
                o->oDialogState = DIALOG_UNK2_ENABLE_TIME_STOP;
            } else {
                set_mario_npc_dialog(0);
            }
            break;
    }

    return dialogResponse;
}

s32 cur_obj_has_model(s32 modelID) {
    if (o->header.gfx.sharedChild == gLoadedGraphNodes[modelID]) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void cur_obj_shake_screen(s32 shake) {
    set_camera_shake_from_point(shake, o->oPosX, o->oPosY, o->oPosZ);
}

s32 obj_attack_collided_from_other_object(struct Object *obj) {
    s32 numCollidedObjs;
    struct Object *other;
    s32 touchedOtherObject = FALSE;

    numCollidedObjs = obj->numCollidedObjs;
    if (numCollidedObjs != 0) {
        other = obj->collidedObjs[0];

        if (other != gMarioObject) {
            other->oInteractStatus |= ATTACK_PUNCH | INT_STATUS_WAS_ATTACKED | INT_STATUS_INTERACTED
                                      | INT_STATUS_TOUCHED_BOB_OMB;
            touchedOtherObject = TRUE;
        }
    }

    return touchedOtherObject;
}

s32 cur_obj_was_attacked_or_ground_pounded(void) {
    s32 attacked = FALSE;

    if ((o->oInteractStatus & INT_STATUS_INTERACTED)
        && (o->oInteractStatus & INT_STATUS_WAS_ATTACKED)) {
        attacked = TRUE;
    }

    if (cur_obj_is_mario_ground_pounding_platform()) {
        attacked = TRUE;
    }

    o->oInteractStatus = 0;
    return attacked;
}

void obj_copy_behavior_params(struct Object *dst, struct Object *src) {
    dst->oBehParams = src->oBehParams;
    dst->oBehParams2ndByte = src->oBehParams2ndByte;
}

s32 cur_obj_init_animation_and_check_if_near_end(s32 animIndex) {
    cur_obj_init_animation_with_sound(animIndex);
    return cur_obj_check_if_near_animation_end();
}

s32 cur_obj_check_grabbed_mario(void) {
    if (o->oInteractStatus & INT_STATUS_GRABBED_MARIO) {
        o->oKingBobombUnk88 = 1;
        cur_obj_become_intangible();
        return TRUE;
    }

    return FALSE;
}

s32 player_performed_grab_escape_action(void) {
    static s32 grabReleaseState;
    s32 result = FALSE;

    if (gPlayer1Controller->stickMag < 30.0f) {
        grabReleaseState = 0;
    }

    if (grabReleaseState == 0 && gPlayer1Controller->stickMag > 40.0f) {
        grabReleaseState = 1;
        result = TRUE;
    }

    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        result = TRUE;
    }

    return result;
}

void disable_time_stop_including_mario(void) {
    gTimeStopState &= ~(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
    o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
}

s32 cur_obj_check_interacted(void) {
    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        o->oInteractStatus = 0;
        return TRUE;
    } else {
        return FALSE;
    }
}

void cur_obj_spawn_loot_blue_coin(void) {
    if (o->oNumLootCoins >= 5) {
        spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
        o->oNumLootCoins -= 5;
    }
}

void cur_obj_spawn_star_at_y_offset(f32 targetX, f32 targetY, f32 targetZ, f32 offsetY) {
    f32 objectPosY = o->oPosY;
    o->oPosY += offsetY;
    spawn_default_star(targetX, targetY, targetZ);
    o->oPosY = objectPosY;
}
