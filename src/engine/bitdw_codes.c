// stuff to make programming easier
#include "sm64.h"
#include "types.h"
//#include "game/game.h"
#include "game/level_update.h"
#include "dialog_ids.h"
//#include "game/kaze_library.h"
#include "seq_ids.h"
#include "game/camera.h"
#include "engine/math_util.h"
#include "game/obj_behaviors_2.h"
#include "game/object_helpers.h"
#include "levels/wf/header.h"
#include "engine/behavior_script.h"
#include "game/spawn_sound.h"
#include "engine/surface_collision.h"
#include "game/interaction.h"
#include "engine/behavior_script.h"
#include "game/object_list_processor.h"
#include "behavior_data.h"
#include "../../enhancements/puppycam.h"
#include "game/rendering_graph_node.h"
#include "game/save_file.h"
#include <PR/gbi.h>
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject
#define objAnimID (o->header.gfx.unk38.animID)
#define objAnimFrame (o->header.gfx.unk38.animFrame)
#define marioObject gMarioState->marioObj
#include "game/OPT_FOR_SIZE.h"

struct CutsceneJump {
    int length;
    struct CutsceneSplinePoint *pos;
    struct CutsceneSplinePoint *foc;
    struct CutsceneJump *next;
};
void ropeplatform(void) {
    if (!o->oOpacity) {
        o->oFaceAngleYaw += 0x1000;
        o->oOpacity = 1;
    }
    switch (o->oAction & 1) {
        case 0:
            if (o->oTimer > 30) {
                o->oAction++;
                cur_obj_play_sound_2(SOUND_OBJ_UNKNOWN2);
            }
            break;
        case 1:
            if (o->oTimer < 30) {
                if (o->oAction & 2) {
                    o->oAngleVelYaw += 0x04;
                } else {
                    o->oAngleVelYaw -= 0x04;
                }
            }
            if (o->oTimer > 70) {
                o->oAngleVelYaw = approach_s16_symmetric(o->oAngleVelYaw, 0, 0x06);
            }
            if (o->oAngleVelYaw == 0) {
                o->oAction++;
            }
            break;
    }
    o->oFaceAngleYaw += o->oAngleVelYaw;
    load_object_collision_model();
}

#define READSP(pointer) (*((u32 *) segmented_to_virtual(pointer)))
#define GETVP(pointer) (segmented_to_virtual(pointer))
#define SPAWNOFFSET 260.f
void chainEle(void) {
    struct Object *spawn;
    if (o->oBehParams & 0xFF) {
        if (!o->oAction) {
            spawn = spawn_object_relative(0, SPAWNOFFSET, 0, 0, o, 0x36, bhvChainElevator);
            spawn->oMoveAngleYaw = o->oMoveAngleYaw + 0x8000;
            spawn->oPosY -= 650.f;
            spawn->oHomeY = o->oPosY;
            spawn = spawn_object_relative(0, -SPAWNOFFSET, 0, 0, o, 0x36, bhvChainElevator);
            spawn->oMoveAngleYaw = o->oMoveAngleYaw + 0x8000;
            spawn->oPosY -= 650.f;
            spawn->oHomeY = o->oPosY;
            spawn->oFaceAngleRoll = 0x8000;

            spawn = spawn_object_relative(0, SPAWNOFFSET, 0, 0, o, 0x36, bhvChainElevator);
            spawn->oMoveAngleYaw = o->oMoveAngleYaw + 0x8000;
            spawn->oPosY += 650.f;
            spawn->oHomeY = o->oPosY;
            spawn = spawn_object_relative(0, -SPAWNOFFSET, 0, 0, o, 0x36, bhvChainElevator);
            spawn->oMoveAngleYaw = o->oMoveAngleYaw + 0x8000;
            spawn->oPosY += 650.f;
            spawn->oHomeY = o->oPosY;
            spawn->oFaceAngleRoll = 0x8000;
            o->oAction = 1;
        }
    } else {
        cur_obj_play_sound_2(SOUND_ENV_ELEVATOR1);
        o->oAngleVelRoll = o->oFaceAngleRoll;
        switch (o->oAction) {
            case 0:
                o->oForwardVel = 6.5f;
                if (o->oTimer > 20) {
                    if (absf(o->oPosY - o->oHomeY) > 700.f) {
                        o->oAction = 1;
                    }
                }
                break;
            case 1:
                o->oFaceAngleRoll += 0x100;
                if (o->oTimer == 0x7F) {
                    o->oAction = 0;
                }
                break;
        }
        o->oAngleVelRoll = o->oFaceAngleRoll - o->oAngleVelRoll;
        o->oVelY = sins(-o->oFaceAngleRoll + 0x4000) * o->oForwardVel;
        o->oVelX = coss(-o->oFaceAngleRoll + 0x4000) * sins(o->oMoveAngleYaw + 0x4000) * o->oForwardVel;
        o->oVelZ = coss(-o->oFaceAngleRoll + 0x4000) * coss(o->oMoveAngleYaw + 0x4000) * o->oForwardVel;
        cur_obj_move_using_vel();
        load_object_collision_model();
    }
}
extern s16 *animrots[2];
void bowserBridge(void) { // todo: get distance from far ahead instead.
    s16 ang;
    f32 x, y, z;
    switch (o->oAction) {
        case 0:
            y = o->oPosY;
            x = o->oPosX + sins(o->oFaceAngleYaw) * 1000.f;
            z = o->oPosZ + coss(o->oFaceAngleYaw) * 1000.f;
            y = o->oPosY - gMarioState->pos[1];
            x = o->oPosX - gMarioState->pos[0];
            z = o->oPosZ - gMarioState->pos[2];
            cur_obj_init_animation(0);
            objAnimFrame = 118;

            if (sqrtf(x * x + y * y + z * z) < 1500.f) {
                o->oAction = 1;
                cur_obj_play_sound_2(SOUND_ACTION_METAL_HEAVY_LANDING);
            }
            break;
        case 1:
            switch (objAnimFrame) {
                case 17:
                case 36:
                case 46:
                    cur_obj_play_sound_2(SOUND_GENERAL_METAL_POUND);
            }
            cur_obj_init_animation(1);
            if (o->oTimer > 150) {
                o->oAction++;
            }
            break;
        case 2:
            cur_obj_init_animation(0);
            if (o->oTimer > 118) {
                o->oAction = 0;
            }
            break;
    }
    ang = o->oFaceAnglePitch;
    if (objAnimFrame > 0) {
        o->oFaceAnglePitch = ((s16 *) GETVP(READSP(&animrots[objAnimID])))[objAnimFrame];
    }
    o->oAngleVelPitch = o->oFaceAnglePitch - ang;
    load_object_collision_model();
}

extern const Trajectory bitdw_area_1_spline_MinecartPath[];
#define opacity o->oOpacity
void bowsercart(void) {
    f32 x, z, y;
    x = ((s16 *) segmented_to_virtual(bitdw_area_1_spline_MinecartPath))[1 + opacity];
    z = ((s16 *) segmented_to_virtual(bitdw_area_1_spline_MinecartPath))[3 + opacity];
    y = ((s16 *) segmented_to_virtual(bitdw_area_1_spline_MinecartPath))[2 + opacity];
    switch (o->oAction) {
        case 0:
            if (dist_between_objects(o, gMarioState->marioObj) < 500) {
                o->oAction++;
                gMarioState->action = ACT_RIDE_MINECART;
            } else {
                obj_mark_for_deletion(o);
            }
            break;
        case 1:
            gMarioState->action = ACT_RIDE_MINECART;
            if (sqrtf((x - o->oPosX) * (x - o->oPosX) + (z - o->oPosZ) * (z - o->oPosZ)) < 80.f) {
                opacity += 4;
                if (((s16 *) segmented_to_virtual(bitdw_area_1_spline_MinecartPath))[opacity] == -1) {
                    o->oAction = 2;
                }
            }
            o->oMoveAngleYaw =
                approach_s16_symmetric(o->oMoveAngleYaw, atan2s(z - o->oPosZ, x - o->oPosX), 0x1000);
            o->oForwardVel = approach_f32(o->oForwardVel, 75.f, 5.0f, 5.0f);
            if (o->oFloorPointer) {
                o->oFaceAnglePitch = approach_s16_symmetric(
                    o->oFaceAnglePitch,
                    (coss(o->oMoveAngleYaw
                          - atan2s(o->oFloorPointer->normal.z, o->oFloorPointer->normal.x)))
                        * atan2s(o->oFloorPointer->normal.y,
                                 sqrtf(o->oFloorPointer->normal.x * o->oFloorPointer->normal.x
                                       + o->oFloorPointer->normal.z * o->oFloorPointer->normal.z)),
                    0x400);
                o->oFaceAngleRoll = approach_s16_symmetric(
                    o->oFaceAngleRoll,
                    (sins(o->oMoveAngleYaw
                          - atan2s(o->oFloorPointer->normal.z, o->oFloorPointer->normal.x)))
                        * atan2s(o->oFloorPointer->normal.y,
                                 sqrtf(o->oFloorPointer->normal.x * o->oFloorPointer->normal.x
                                       + o->oFloorPointer->normal.z * o->oFloorPointer->normal.z)),
                    0x400);
                newcam_yaw = o->oMoveAngleYaw - 0x8000;
            }
            break;
        case 2:
            o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 0.f, 0.02f);
            if (o->oTimer == 23) {
                o->oAction++;
                gMarioState->action = ACT_THROWN_FORWARD;
                gMarioState->vel[1] = 30.f;
                gMarioState->forwardVel = 50.f;
                gMarioState->pos[1] += 100.f;
                o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 0.f, 0.75f);
            }
            break;
        case 3:
            o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 0.f, 0.05f);
            o->oGraphYOffset -= 5.f;
            if (o->oTimer > 50) {
                obj_mark_for_deletion(o);
            }
            break;
    }
    cur_obj_update_floor();
    cur_obj_move_using_fvel_and_gravity();
    if ((o->oPosY - o->oFloorHeight) < 50.f) {
        o->oPosY = o->oFloorHeight;
        o->oVelY = 0;
    }
}

u8 paletteBackup2[32] = {
    0x21, 0x4b, 0x00, 0x01, 0x5a, 0x83, 0xdf, 0x5d, 0xee, 0x7d, 0x2b, 0x41, 0x9f, 0xb9, 0x33, 0x8f,
    0x03, 0x5b, 0x41, 0xb9, 0x51, 0xdf, 0x08, 0x0d, 0x98, 0x77, 0xb8, 0x25, 0x40, 0x0b, 0x58, 0xab,
};

u8 paletteBackup3[32] = {
    0x79, 0x01, 0xbc, 0x51, 0xd5, 0xa3, 0xa3, 0x05, 0x00, 0x01, 0x68, 0x41, 0xf9, 0x41, 0xfa, 0x81,
    0x48, 0x01, 0xdf, 0x41, 0x28, 0x41, 0x2f, 0xc7, 0x06, 0xf1, 0x19, 0x3f, 0xb8, 0x3d, 0xf8, 0x11,

};
/*Gfx bowser1flag_Bone_002_mesh_layer_4[] = {
        gsSPDisplayList(mat_bowser1flag_BOWSER),*/

/*Gfx bowser1flag_Bone_004_mesh_layer_4[] = {
        gsSPDisplayList(bowser1flag_Bone_004_mesh_layer_4_tri_0),
        gsSPDisplayList(mat_revert_bowser1flag_BOWSER),
        gsSPEndDisplayList(),
};*/
extern Gfx mat_bowser1flag_MARIO_layer4[];
extern Gfx mat_revert_bowser1flag_MARIO_layer4[];
Gfx DLREP[2] = {
    gsSPDisplayList(mat_bowser1flag_MARIO_layer4),
    gsSPDisplayList(mat_revert_bowser1flag_MARIO_layer4),

};
#include "levels/bitdw/header.h"
void checkpointflag(void) {
    switch (o->oAction) {
        case 0:

            if (o->oInteractStatus || (gWarpCheckpoint.warpNode == 0x0B)) {
                o->oAction = 1;
                ((Gfx *) segmented_to_virtual(bowser1flag_Bone_002_mesh_layer_4))[0] = DLREP[0];
                ((Gfx *) segmented_to_virtual(bowser1flag_Bone_004_mesh_layer_4))[1] = DLREP[1];
                cur_obj_play_sound_2(SOUND_GENERAL2_RIGHT_ANSWER);
                gWarpCheckpoint.actNum = gCurrActNum;
                gWarpCheckpoint.courseNum = gCurrCourseNum;
                gWarpCheckpoint.levelID = gCurrLevelNum;
                gWarpCheckpoint.areaNum = 1;
                gWarpCheckpoint.warpNode = 0x0B;
            }
            break;
        case 1:

            break;
    }
    o->oInteractStatus = 0;
}

void treadmillyosh(void) {
    if (o->header.gfx.unk38.animFrame == 0) {
        cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WALK);
    }
    switch (o->oAction) {
        case 0:
            // walk normal
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.f);
            if (!o->oTimer) {
                o->header.gfx.unk38.animFrame = random_u16() % 120;
                o->oAnimState = 3 + (random_u16() & 1);
            }
            if (!(random_u16() % 300)) {
                cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_PAM);
                o->oAction = 1;
            }
            o->header.gfx.unk38.animFrame += 4;
            break;
        case 1:
            o->oAnimState = 1;
            o->header.gfx.unk38.animFrame += 2;
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, -10.f, 1.f);
            if (cur_obj_lateral_dist_to_home() > 400.f) {
                o->oAction = 2;
                cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WAAH);
            }
            // slow down
            // set eye to exhausted
            break;
        case 2:
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 20.f, 1.f);
            o->oAnimState = 7;
            o->header.gfx.unk38.animFrame += 7;
            if (cur_obj_lateral_dist_to_home() < 100.f) {
                o->oAction = 0;
            }
            // speed up
            // set eye to hasty/scared
            break;
    }
    cur_obj_move_xz_using_fvel_and_yaw();
}

struct Vtxcombo {
    Vtx *verts;
    u32 vertCount;
};

#include "levels/bitdw/header.h"

#define shadeEntry(a)                                                                                  \
    { a, sizeof(a) / 0x10 }
struct Vtxcombo shadeList[] = { shadeEntry(bitdw_dl_ZGEARS_mesh_layer_6_vtx_0),
                                shadeEntry(bitdw_dl_Layer6_mesh_layer_6_vtx_1),
                                shadeEntry(bitdw_dl_Layer6_mesh_layer_6_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_009_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_008_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_007_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_006_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_005_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_004_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_003_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_002_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_001_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer4_mesh_layer_4_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_018_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_018_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_018_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_017_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_017_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_016_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_016_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_015_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_015_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_015_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_014_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_014_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_014_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_013_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_013_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_013_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_012_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_012_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_012_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_011_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_011_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_010_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_010_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_010_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_009_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_009_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_009_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_3),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_4),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_5),
                                shadeEntry(bitdw_dl_Layer1_008_mesh_layer_1_vtx_6),
                                shadeEntry(bitdw_dl_Layer1_007_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_007_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_007_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_007_mesh_layer_1_vtx_3),
                                shadeEntry(bitdw_dl_Layer1_007_mesh_layer_1_vtx_4),
                                shadeEntry(bitdw_dl_Layer1_007_mesh_layer_1_vtx_5),
                                shadeEntry(bitdw_dl_Layer1_006_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_006_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_006_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_006_mesh_layer_1_vtx_3),
                                shadeEntry(bitdw_dl_Layer1_006_mesh_layer_1_vtx_4),
                                shadeEntry(bitdw_dl_Layer1_005_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_005_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_005_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_005_mesh_layer_1_vtx_3),
                                shadeEntry(bitdw_dl_Layer1_005_mesh_layer_1_vtx_4),
                                shadeEntry(bitdw_dl_Layer1_004_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_004_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_004_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_003_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_003_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_003_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_002_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_002_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_002_mesh_layer_1_vtx_2),
                                shadeEntry(bitdw_dl_Layer1_001_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_001_mesh_layer_1_vtx_1),
                                shadeEntry(bitdw_dl_Layer1_mesh_layer_1_vtx_0),
                                shadeEntry(bitdw_dl_Layer1_mesh_layer_1_vtx_1) };

u32 currIndex = 0;
#define MAXENTRIES (sizeof(shadeList) / 8)
#define MAXVERTSPERFRAME 1000
void lavafloorcode(void) {
    u32 i, j;
    u32 ff = 0;
    Vtx *a;
    u32 r, g, b;
    s16 lavapos = o->oPosY;
    s32 diff;
    o->oDrawingDistance = 100000.f;
    if (!o->oTimer) {
        for (i = 0; i < MAXENTRIES; i++) {
            a = segmented_to_virtual(shadeList[i].verts);
            for (j = 0; j < shadeList[i].vertCount; j++) {
                a->v.flag = GPACK_RGBA5551(a->v.cn[0], a->v.cn[1], a->v.cn[2], 1);
                a++;
            }
        }
    }
    while (ff < MAXVERTSPERFRAME) {
        a = segmented_to_virtual(shadeList[currIndex].verts);
        for (j = 0; j < shadeList[currIndex].vertCount; j++) {
            // interpolate to color FF873B
            diff = a->v.ob[1] - lavapos;
            if (diff < 0) {
                r = 0xFF;
                g = 0x87;
                b = 0x3B;
#define MAXDIFF 1024
            } else if (diff <= MAXDIFF) {
                r = (a->v.flag >> 11) * 8;
                g = ((a->v.flag >> 6) & 0x1F) * 8;
                b = ((a->v.flag >> 1) & 0x1F) * 8;
                r = (((r - 0xFF) * diff) + (0xFF * MAXDIFF)) / MAXDIFF;
                g = (((g - 0x87) * diff) + (0x87 * MAXDIFF)) / MAXDIFF;
                b = (((b - 0x3B) * diff) + (0x3B * MAXDIFF)) / MAXDIFF;
            } else {
                r = (a->v.flag >> 11) * 8;
                g = ((a->v.flag >> 6) & 0x1F) * 8;
                b = ((a->v.flag >> 1) & 0x1F) * 8;
            }
            a->v.cn[0] = r;
            a->v.cn[1] = g;
            a->v.cn[2] = b;
            a++;
        }
        ff += shadeList[currIndex].vertCount;
        currIndex++;
        if (currIndex == MAXENTRIES) {
            currIndex = 0;
        }
    }

    if (o->oTimer > 350) {
        o->oPosY = approach_f32_symmetric(o->oPosY, gMarioState->floorHeight - 880.f, 3.f);
        if ((gMarioState->floorHeight - 880.f) > o->oPosY) {
            o->oPosY = approach_f32_asymptotic(o->oPosY, gMarioState->floorHeight - 880.f, 0.005f);
        }
    }
    if (o->oPosY < o->oHomeY) {
        o->oPosY = o->oHomeY;
    } else if (o->oPosY > 10200.f) {
        o->oPosY = 10200.f;
    }
    load_object_collision_model();
}

void talkyoshiB1(void) {
    yoshiBlinkManager();
    switch (o->oBehParams2ndByte) {
        case 7:
            // angry
            if (o->oAnimState == 0) {
                o->oAnimState = 7;
            }
            break;
        case 10:
            // cool yosh
            o->oAnimState = 10;
            break;
    }
    if (mario_ready_to_speak() && (o->oInteractStatus == INT_STATUS_INTERACTED)) {
        o->oOpacity = o->oAction;
        o->oAction = 3;
    }
    if (o->oAction == 3) {
        if (talkToMario(o->oBehParams & 0xFF, 4)) {
            o->oAction = 0;
            o->oInteractStatus = 0;
        }
    }
}
struct Object *mechBowserOBJ;
#define /*0x0F4*/ oPatternAction OBJECT_FIELD_S32(0x1B)
#define /*0x0F8*/ oPatternAngleVel OBJECT_FIELD_S32(0x1C)
#define /*0x0FC*/ oPatternRot OBJECT_FIELD_S32(0x1D)
#define /*0x100*/ oActionParam OBJECT_FIELD_S32(0x1E)
#define /*0x104*/ olastHP OBJECT_FIELD_S32(0x1F)
#define /*0x108*/ oRedAmount OBJECT_FIELD_F32(0x20)
// rotationspeedFunction, positiontargetfunction, positiontargetfunctionInside,
struct pattern {
    s16 (*rotSpeed)(void);
    f32 (*posOFunc)(void);
    f32 (*posIFunc)(void);
};

s16 noRot() {
    return 0;
}
s16 slowRot() {
    if (o->oActionParam & 1) {
        return -0x020;
    } else {
        return 0x020;
    }
}
s16 fastRot() {
    if (o->oActionParam & 1) {
        return -(0x020 + 0x10 * o->oHealth);
    } else {
        return (0x020 + 0x10 * o->oHealth);
    }
}
s16 backforthRot() {
    return (0x018 * o->oHealth + 0x40) * sins(o->oOpacity * 0x100);
}
f32 goUp() {
    return sins(((*((u8 *) &o->oBehParams) + mechBowserOBJ->oActionParam) * 0x8000) + o->oTimer * 0x20)
               * 192.f
           - 192.f;
}
f32 goDown() {
    return -768.f;
}
f32 downAlteration() {
    return -768.f
           + sins(((*((u8 *) &o->oBehParams) - mechBowserOBJ->oActionParam) * 0x8000)
                  + o->oTimer * 0x20)
                 * 192.f
           - 192.f;
}
f32 goUpLow() {
    return sins(((*((u8 *) &o->oBehParams) - mechBowserOBJ->oActionParam) * 0x8000) + o->oTimer * 0x18)
               * 384.f
           - 384.f;
}
f32 goUpWavy() {
    return sins(((*((u8 *) &o->oBehParams) + mechBowserOBJ->oActionParam) * 0x80) + o->oTimer * 0x18)
               * 512.f
           - 512.f;
}
f32 muchGoDown() {
    if (((*((u8 *) &o->oBehParams) + mechBowserOBJ->oActionParam) % 3)) {
        return -768.f;
    } else {
        return -128.f;
    }
}
// 0
struct pattern firstCycle = { noRot, goUp, goDown };
// 1 - 3
struct pattern slowRotation1 = { slowRot, muchGoDown, goDown };
struct pattern slowRotation2 = { slowRot, muchGoDown, downAlteration };
struct pattern firstCycle2 = { backforthRot, goUp, downAlteration };
// 2 - 4
struct pattern backforthRot1 = { backforthRot, goUp, goDown };
struct pattern backforthRot2 = { backforthRot, muchGoDown, downAlteration };
struct pattern backforthRot3 = { slowRot, muchGoDown, goDown };
struct pattern wavyRot1 = { slowRot, goUpWavy, goDown };
// 3-5
struct pattern fastRotation1 = { fastRot, goUp, goDown };
struct pattern backforthRot4 = { backforthRot, muchGoDown, downAlteration };
struct pattern wavyRot2 = { backforthRot, goUpWavy, goDown };
// 4,5
struct pattern fastRotation2 = { fastRot, goUpLow, downAlteration };
struct pattern fastRotation3 = { fastRot, muchGoDown, goDown };
struct pattern fastRotation4 = { fastRot, muchGoDown, downAlteration };
struct pattern fastRotation5 = { backforthRot, muchGoDown, downAlteration };
struct pattern wavyRot3 = { fastRot, goUpWavy, goDown };

struct pattern *act1[] = { &firstCycle, &slowRotation1, &slowRotation2, &firstCycle2 };
struct pattern *act2[] = { &slowRotation1, &slowRotation2, &firstCycle2, &backforthRot1,
                           &backforthRot2, &backforthRot3, &wavyRot1 };
struct pattern *act3[] = { &slowRotation1, &slowRotation2, &firstCycle2,   &backforthRot1,
                           &backforthRot2, &backforthRot3, &fastRotation1, &backforthRot4,
                           &wavyRot1,      &wavyRot2 };
struct pattern *act4[] = { &backforthRot1, &backforthRot2, &backforthRot3,
                           &fastRotation1, &backforthRot4, &fastRotation2,
                           &fastRotation3, &wavyRot2,      &wavyRot3 };
struct pattern *act5[] = { &fastRotation1, &backforthRot4, &fastRotation2, &fastRotation3,
                           &fastRotation4, &fastRotation5, &wavyRot2,      &wavyRot3 };
u8 mechBrightness = 0;

Gfx envColorMech[] = {
    gsDPSetEnvColor(64, 64, 64, 64),
    gsSPEndDisplayList(),
};

Gfx *geo_mechBowser_hit(s32 callContext, struct GraphNode *b, Mat4 *mtx) {
    Gfx *gfx = NULL;
    struct GraphNodeGenerated *asGenerated = (struct GraphNodeGenerated *) b;
    struct Object *obj = (struct Object *) gCurGraphNodeObject;
    if (callContext == GEO_CONTEXT_RENDER) {
        gfx = &envColorMech[0];
        envColorMech[0].words.w1 = 0;
        if (obj->oAction == 4) {
            if (obj->oTimer < 64) {
                envColorMech[0].words.w1 = ((obj->oTimer % 14) * 7) << 24;
            }
        } else if (obj->oAction == 5) {
            obj->oRedAmount += obj->oTimer / 50.f;
            if (obj->oRedAmount > 81.f) {
                obj->oRedAmount -= 162.f;
            }
            if (obj->oRedAmount > 0) {
                envColorMech[0].words.w1 = ((s32) obj->oRedAmount) << 24;
            }
        }
        asGenerated->fnNode.node.flags = (asGenerated->fnNode.node.flags & 0xFF) | (1 << 8);
    }
    return gfx;
}

u16 soundRanges1[][2] = { { 5, 20 }, { 30, 46 }, { 56, 97 }, { 107, 148 }, { 158, 186 }, { 225, 255 } };
u16 soundRanges2[][2] = {
    { 5, 21 }, { 47, 74 }, { 98, 156 }, { 166, 183 }, { 208, 243 }, { 259, 317 }
};
u16 soundRanges3[][3] = { { 46, 77 } };
u16 soundRanges4[][2] = { { 5, 16 }, { 39, 56 }, { 60, 135 }, { 145, 175 } };
u16 soundRanges5[][2] = { { 0, 500 } };
u16 **soundRanges[] = { &soundRanges1, &soundRanges2, &soundRanges3, &soundRanges4, &soundRanges5 };
u16 rangeSizes[] = { sizeof(soundRanges1) / 4, sizeof(soundRanges2) / 4, sizeof(soundRanges3) / 4,
                     sizeof(soundRanges4) / 4, sizeof(soundRanges5) / 4 };

void playBossSounds() {
    s32 i;
    s32 j;
    s32 animFrame = o->header.gfx.unk38.animFrame;
    u16 *soundRange = soundRanges[o->header.gfx.unk38.animID];
    for (j = 0; j < rangeSizes[o->header.gfx.unk38.animID]; j++) {
        if ((soundRange[j * 2] < animFrame) && (soundRange[j * 2 + 1] > animFrame)) {
            cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
        } else if (soundRange[j * 2 + 1] == animFrame) {
            cur_obj_play_sound_2(SOUND_GENERAL_BIG_CLOCK);
        }
    }
}
extern struct CutsceneSplinePoint bitdw_area_2_spline_MechSceneFoc[];
extern struct CutsceneSplinePoint bitdw_area_2_spline_MechScenePos[];
extern struct CutsceneJump *currentScene;
struct CutsceneJump mechScene = { 405, bitdw_area_2_spline_MechScenePos,
                                  bitdw_area_2_spline_MechSceneFoc, 0 };
extern void spawn_triangle_break_particlesOffset(s16 numTris, s16 triModel, f32 triSize,
                                                 s16 triAnimState, f32 Offset);
void mechBossArm(void) {
    if (!o->oTimer) {
        if (o->oBehParams2ndByte) {
            o->oAngleVelPitch = 0x700;
            o->oAngleVelRoll = 0x170;
            o->oVelY = 10.f;
            o->oForwardVel = 14.f;
        } else {
            o->oAngleVelPitch = -0x1700;
            o->oAngleVelRoll = 0x400;
            o->oVelY = 80.f;
            o->oForwardVel = -60.f;
        }
    }
    o->oFaceAnglePitch += o->oAngleVelPitch;
    o->oFaceAngleRoll += o->oAngleVelRoll;
    if (o->oBehParams2ndByte) {
        o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 0, 0x10);
    } else {
        o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 0, 0x80);
    }
        obj_move_xyz_using_fvel_and_yaw();
    if (o->oBehParams2ndByte) {
        o->oVelY -= 4.f;
    } else {
        o->oVelY -= .5f;
    }
    cur_obj_move_using_vel();
    if (o->oTimer > 23) {
        if (o->oBehParams2ndByte) {
            spawn_mist_particles_variable(0, 0.f, 92.f);
            spawn_triangle_break_particlesOffset(30, 138, 4.0f, 4, 0.f);
            cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION7);
            obj_mark_for_deletion(o);
            set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY, o->oPosZ);
        } else {
            obj_mark_for_deletion(o);
        }
    }
    if (o->oBehParams2ndByte) {
        if (o->oTimer > 18) {
            cur_obj_scale(o->header.gfx.scale[0] + 0.1f);
        }
    }
}
void mechBowserboss(void) {
    u32 sp34;
    struct Object *flame;
    mechBowserOBJ = o;
    o->oInteractStatus = 0;
    o->oIntangibleTimer = 0;
    if (o->oAction) {
        if (o->olastHP != o->oHealth) {
            o->olastHP = o->oHealth;
            o->oOpacity = 0;
            o->oAction = 4;
            if (o->oHealth == 6) {
                cur_obj_play_sound_2(SOUND_MECHBOWSER_HIT); // //SOUND_OBJ_DIVING_INTO_WATER
                o->oAction = 5;
                gMarioState->statusForCamera->cameraEvent = 0xc8;
                currentScene = &mechScene;
            }
        }
    }
    playBossSounds();
    switch (o->oAction) {
        case 0:
            // shut down, then turning on
            o->oHealth = 0;
            o->oAction = 6;
            break;
        case 1:
            // stare at mario
            cur_obj_init_animation(1);
            o->oMoveAngleYaw =
                approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x80 + o->oHealth * 0x10);
            if ((o->header.gfx.unk38.animFrame == 0) || (o->header.gfx.unk38.animFrame == 160)) {
                if ((random_u16() % 18) < (o->oHealth + 2 + o->oSubAction)) {
                    o->oAction = 3;
                } else {
                    o->oSubAction++;
                }
            }
            break;
        case 2:
            // shoot bullet bill
            break;
        case 3:
            if (o->oTimer == 12) {
                cur_obj_play_sound_2(SOUND_MECHBOWSER_INHALE); // //SOUND_OBJ_DIVING_INTO_WATER
            }
            o->oMoveAngleYaw =
                approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x80 + o->oHealth * 0x10);
            cur_obj_init_animation(3);
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 1;
            } else {
                if ((o->header.gfx.unk38.animFrame > 60) && (o->header.gfx.unk38.animFrame < 139)) {
                    o->oFlameThowerUnk110 = 30;
                    flame = spawn_object_relative(
                        o->oBehParams2ndByte,
                        -177.f + (o->header.gfx.unk38.animFrame - 59.f) * 354 / 66.f, 930.f, 425, o,
                        MODEL_RED_FLAME, bhvFlamethrowerFlame);
                    flame->oForwardVel = 125.f;
                    flame->oMoveAngleYaw += (o->header.gfx.unk38.animFrame - 99) * (0x2000 / 38);
                    flame->oVelY = (-o->oPosY - 930.f + gMarioState->pos[1] + 75.f)
                                   / (lateral_dist_between_objects(o, gMarioState->marioObj) / 95.f);
                    cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
                }
            }
            break;
        case 4:
            // get hit, do a hand gesture at the platforms to signify him speeding them up
            if (o->oTimer == 12) {
                cur_obj_play_sound_2(SOUND_MECHBOWSER_HIT); // //SOUND_OBJ_DIVING_INTO_WATER
                set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY + 700.f, o->oPosZ);
            }
            cur_obj_init_animation(2);
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 1;
            }
            break;
        case 5:
            o->oOpacity = 100;
            // fkn die
            // fall apart piece by piece
            if (o->oTimer > 60) {
                o->oMoveAngleYaw += o->oTimer * 11;
            }
            cur_obj_init_animation(4);
            if (o->header.gfx.unk38.animFrame == 210) {
                cur_obj_play_sound_2(SOUND_MECHBOWSER_HIT); // //SOUND_OBJ_DIVING_INTO_WATER
            }
            if (o->header.gfx.unk38.animFrame == 223) {
                cur_obj_set_model(0x42);
                spawn_object_relative(0, 368.252f * 1.4f, 437.651f * 1.4f, -2.f, o, MODEL_EXPLOSION,
                                      bhvExplosion);
                spawn_object_relative(0, -368.252f * 1.4f, 437.651f * 1.4f, -2.f, o, MODEL_EXPLOSION,
                                      bhvExplosion);

                spawn_object_relative(0, 368.252f * 1.4f, 437.651f * 1.4f, -2.f, o, 0x44, bhvMechArm);
                spawn_object_relative(0, -368.252f * 1.4f, 437.651f * 1.4f, -2.f, o, 0x45, bhvMechArm);
                set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY + 700.f, o->oPosZ);
            }
            if (o->header.gfx.unk38.animFrame == 260) {
                cur_obj_play_sound_2(SOUND_MECHBOWSER_HIT); // //SOUND_OBJ_DIVING_INTO_WATER
                cur_obj_set_model(0x43);
                spawn_object_relative(0, 0.f, 598.369f * 1.4f, 448.316f * 1.4f, o, MODEL_EXPLOSION,
                                      bhvExplosion);
                spawn_object_relative(1, 0.f, 598.369f * 1.4f, 448.316f * 1.4f, o, 0x46, bhvMechArm);
                set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY + 700.f, o->oPosZ);
            }
            // anim frame 225, arms fly off
            // anim frame 260, arms fly off
            // last anim frame, body explodes
            if (o->header.gfx.unk38.animFrame >= 297) {
                cur_obj_scale(o->header.gfx.scale[0] + 0.1f);
            }
            if (o->header.gfx.unk38.animFrame >= 302) {
                if (!o->oSubAction) {
                    set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY + 700.f, o->oPosZ);
                    spawn_mist_particles_variable(0, 750.f, 92.f);
                    spawn_triangle_break_particlesOffset(30, 138, 4.0f, 4, 175.f);
                    // cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION7);
                    create_sound_spawner(SOUND_GENERAL_BOWSER_BOMB_EXPLOSION);
                    flame = spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
                    flame->oBehParams2ndByte = 30;
                    flame->oPosY += 375.f;
                    flame->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                    o->oSubAction++;
                } else {
                    o->oSubAction++;
                    cur_obj_set_model(0);
                    if (o->oSubAction > 2) {
                        obj_mark_for_deletion(o);
                        cur_obj_spawn_star_at_y_offset(o->oPosX, o->oPosY, o->oPosZ, 200.f);
                    }
                }
            }
            break;
        case 6:
            // wait to turn on
            if (!o->oSubAction) {
                o->header.gfx.unk38.animFrame = 0;
                o->oOpacity = 190;
                o->oActionParam = 0;
            } else {
                if (cur_obj_check_if_at_animation_end()) {
                    o->oAction = 1;
                    o->oActionParam = 0x8000;
                } else {
                    if (o->header.gfx.unk38.animFrame == 70) {
                        cur_obj_play_sound_2(SOUND_MECHBOWSER_HIT); // //SOUND_OBJ_DIVING_INTO_WATER
                        set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY + 700.f,
                                                    o->oPosZ);
                    }
                    if (o->header.gfx.unk38.animFrame == 180) {
                        cur_obj_play_sound_2(SOUND_MECHBOWSER_HIT); // //SOUND_OBJ_DIVING_INTO_WATER
                        set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY + 700.f,
                                                    o->oPosZ);
                    }
                    if ((o->header.gfx.unk38.animFrame > 189)
                        && (o->header.gfx.unk38.animFrame < 236)) {
                        if (o->header.gfx.unk38.animFrame < 219)
                            sp34 = 15;
                        else
                            sp34 = 238 - o->header.gfx.unk38.animFrame; // Range: [15..2]
                        o->oFlameThowerUnk110 = sp34;
                        flame = spawn_object_relative(o->oBehParams2ndByte, 0, 1075, 425, o,
                                                      MODEL_RED_FLAME, bhvFlamethrowerFlame);
                        flame->oForwardVel = 95.f;
                        flame->oVelY = 20.f;
                        cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
                    }
                }
            }
            break;
    }
    if (!o->oOpacity) {
        switch (o->oHealth) {
            case 0:
                o->oPatternAction = &firstCycle;
                break;
            case 1:
                o->oPatternAction = act1[random_u16() % 4];
                break;
            case 2:
                o->oPatternAction = act2[random_u16() % 7];
                break;
            case 3:
                o->oPatternAction = act3[random_u16() % 10];
                break;
            case 4:
                o->oPatternAction = act4[random_u16() % 9];
                break;
            case 5:
            case 6:
                o->oPatternAction = act5[random_u16() % 8];
                break;
        }
        o->oOpacity = 300 - (o->oHealth * 10) + (random_u16() % 50);
        o->oActionParam = random_u16();
    } else {
        o->oOpacity--;
        if (o->oHealth > 0) {
            if (o->oOpacity < 42) {
                mechBrightness = (o->oOpacity % 14) * 7;
            } else {
                mechBrightness = 0;
            }
        }
    }
    o->oPatternAngleVel = approach_s16_symmetric(
        o->oPatternAngleVel, ((struct pattern *) o->oPatternAction)->rotSpeed(), 0x20 + o->oHealth * 8);
    o->oPatternRot += o->oPatternAngleVel;
}

Gfx envColorRobo[] = {
    gsDPSetEnvColor(64, 64, 64, 64),
    gsSPEndDisplayList(),
};

Gfx *geo_set_env_from_robo(s32 callContext, struct GraphNode *b, Mat4 *mtx) {
    Gfx *gfx = NULL;
    struct GraphNodeGenerated *asGenerated = (struct GraphNodeGenerated *) b;
    if (callContext == GEO_CONTEXT_RENDER) {
        gfx = &envColorRobo[0];
        envColorRobo[0].words.w1 = mechBrightness << 24;
        asGenerated->fnNode.node.flags = (asGenerated->fnNode.node.flags & 0xFF) | (1 << 8);
    }
    return gfx;
}
#define oOffsetY OBJECT_FIELD_F32(0x1D)
void bossplatform(void) {
    s16 rotationgoal;
    f32 targetPos;
    f32 targetSpeed;
    f32 x, y, z;
    s16 lastRot = o->oMoveAngleYaw;
    x = o->oPosX;
    y = o->oPosY;
    z = o->oPosZ;
    if (!o->oAction) {
        if (o->oTimer > 1) {
            if (!o->oHiddenBlueCoinSwitch) {
                o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvMechBowserBoss);
            } else {
                if ((o->oBehParams & 0x0000FFFF) < 0x700) {
                    targetPos =
                        ((struct pattern *) o->oHiddenBlueCoinSwitch->oPatternAction)->posIFunc()
                        + o->oHomeY;
                } else {
                    targetPos =
                        ((struct pattern *) o->oHiddenBlueCoinSwitch->oPatternAction)->posOFunc()
                        + o->oHomeY;
                }
                if (gMarioState->action == ACT_LAVA_BOOST){
                    targetPos = o->oHomeY -850.f;
                }
                targetSpeed = 25.f + o->oHiddenBlueCoinSwitch->oHealth * 3.f;
                o->oPosY = approach_f32_symmetric(o->oPosY - o->oOffsetY, targetPos, targetSpeed)
                           + o->oOffsetY;
            }
            rotationgoal =
                o->oHiddenBlueCoinSwitch->oPatternRot + *((u8 *) &o->oBehParams) * 0x10000 / 12;
            o->oPosX = sins(rotationgoal) * (o->oBehParams & 0x0000FFFF);
            o->oPosZ = coss(rotationgoal) * (o->oBehParams & 0x0000FFFF);
            o->oOffsetY += sins(o->oTimer * 0x100) * 2.f;
            o->oMoveAngleYaw = rotationgoal + 0x8000;
            load_object_collision_model();
            if (o->oHiddenBlueCoinSwitch->oAction == 5) {
                o->oAction = 1;
            }
        }
        o->oVelX = o->oPosX - x;
        o->oVelY = o->oPosY - y;
        o->oVelZ = o->oPosZ - z;
        o->oAngleVelYaw = o->oMoveAngleYaw - lastRot;
    } else {
        o->oAngleVelYaw = approach_s16_symmetric(o->oAngleVelYaw, 0, 0x20);
        rotationgoal = o->oMoveAngleYaw + 0x8000 + o->oAngleVelYaw;
        o->oPosX = sins(rotationgoal) * (o->oBehParams & 0x0000FFFF);
        o->oPosZ = coss(rotationgoal) * (o->oBehParams & 0x0000FFFF);
        o->oOffsetY += sins(o->oTimer * 0x100) * 2.f;
        o->oMoveAngleYaw = rotationgoal + 0x8000;
        o->oVelX = o->oPosX - x;
        o->oVelY = o->oPosY - y;
        o->oVelZ = o->oPosZ - z;
        o->oAngleVelYaw = o->oMoveAngleYaw - lastRot;

        load_object_collision_model();
    }
}

void clownfoe_move(void) {
    u32 sp34;
    struct Object *flame;
    s16 angleVel;
    cur_obj_update_floor_and_walls();
    switch (o->oAction) {
        case 0:
            cur_obj_become_tangible();
            cur_obj_unhide();
            o->oAction = 1;
            break;
        case 1:
            if (o->oTimer > 50 + (random_u16() & 0x1FF)) {
                o->oAction = 2;
            }
            break;
        case 2:
            if (1000.0f < cur_obj_lateral_dist_from_mario_to_home()) {
                o->oAngleToMario = cur_obj_angle_to_home();
            } else if ((o->oTimer > 30) && (o->oTimer < 270)) {
                if (o->oTimer < 261)
                    sp34 = 10;
                else
                    sp34 = 272 - o->oTimer; // Range: [15..2]
                o->oFlameThowerUnk110 = sp34;
                flame = spawn_object_relative(o->oBehParams2ndByte, 0, 46, 50, o, MODEL_RED_FLAME,
                                              bhvFlamethrowerFlame);
                flame->oForwardVel = 65.f;
                cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
            }
            if (o->oTimer > 150) {
                o->oHeaveHoUnkF4 = (302 - o->oTimer) / 152.0f;
                if (o->oHeaveHoUnkF4 < 0.1) {
                    o->oHeaveHoUnkF4 = 0.1;
                    o->oAction = 1;
                }
            } else {
                o->oHeaveHoUnkF4 = 1.0f;
            }
            o->oForwardVel = o->oHeaveHoUnkF4 * 10.0f;
            angleVel = o->oHeaveHoUnkF4 * 0x400;
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, angleVel);
            break;
    }
    cur_obj_move_standard(-78);
    if (o->oForwardVel > 3.0f) {
        cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
    }
    if (o->oAction != 0 && o->oMoveFlags & (0x40 | 0x20 | 0x10 | 0x8)) {
        o->oAction = 0;
        cur_obj_set_pos_to_home();
    }
}

#define animState OBJECT_FIELD_F32(0x1D)
void clwonfoe(void) {
    clownfoe_move();
    if (cur_obj_is_mario_ground_pounding_platform()) {
        spawn_mist_particles_variable(0, 0, 46.f);
        spawn_triangle_break_particles(30, 138, 3.0f, 4);
        cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION7);
        spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
        obj_mark_for_deletion(o);
    }
    o->animState -= o->oForwardVel / 20.f;
    if (o->animState < 0.f) {
        o->animState += 4.f;
    } else if (o->animState > 4.f) {
        o->animState -= 4.f;
    }
    o->oAnimState = o->animState;
    load_object_collision_model();
}

void bonzaicode(void) {
    switch (o->oAction) {
        case 0:
            if (!o->oHiddenBlueCoinSwitch) {
                o->oHiddenBlueCoinSwitch =
                    cur_obj_nearest_object_with_behavior(bhvFloorSwitchAnimatesObject);
            }
            if (o->oHiddenBlueCoinSwitch->oAction) {
                o->oAction = 1;
                cur_obj_shake_screen(SHAKE_POS_SMALL);
            }
            break;
        case 1:
            if (o->oTimer & 1) {
                spawn_object_relative(0, 0, 50.f, -200.f, o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oTimer > 20) {
                o->oAction = 2;
                o->oForwardVel = 70.f;
                o->oVelY = -10.f;
                cur_obj_play_sound_2(SOUND_OBJ_POUNDING_CANNON);
            }
            break;
        case 2:
            if (o->oTimer & 1) {
                spawn_object_relative(0, 0, 50.f, -200.f, o, MODEL_SMOKE, bhvDustBackwards);
            }
        obj_move_xyz_using_fvel_and_yaw();
            cur_obj_update_floor();
            if (o->oTimer > 32) {
                mechBowserOBJ->oHealth++;
                spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
                create_sound_spawner(SOUND_GENERAL_BOWSER_BOMB_EXPLOSION);
                set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY, o->oPosZ);
            }
            break;
    }
}

extern struct CutsceneSplinePoint bitdw_area_2_spline_BowserSceneFoc[];
extern struct CutsceneSplinePoint bitdw_area_2_spline_BowserSceneFoc_001[];
extern struct CutsceneSplinePoint bitdw_area_2_spline_BowserSceneFoc_002[];
extern struct CutsceneSplinePoint bitdw_area_2_spline_BowserScenePos[];
extern struct CutsceneSplinePoint bitdw_area_2_spline_BowserScenePos_001[];
extern struct CutsceneSplinePoint bitdw_area_2_spline_BowserScenePos_002[];

extern s16 sCutsceneDialogID;
extern s32 gDialogResponse;
extern int timer;
extern int timeAccumulated;
struct CutsceneJump bowserScene3 = { 360, bitdw_area_2_spline_BowserScenePos_002,
                                     bitdw_area_2_spline_BowserSceneFoc_002, 0 };
struct CutsceneJump bowserScene2 = { 190, bitdw_area_2_spline_BowserScenePos_001,
                                     bitdw_area_2_spline_BowserSceneFoc_001, &bowserScene3 };
struct CutsceneJump bowserScene = { 131, bitdw_area_2_spline_BowserScenePos,
                                    bitdw_area_2_spline_BowserSceneFoc, &bowserScene2 };
extern s32 dialogState;
extern f32 gDefaultSoundArgs[3];
extern u8 openEyes[];
extern u8 halfOpenEyes[];
extern u8 closedEyes[];
extern u8 angryEyes[];
extern u8 angryHappy[];
extern u8 bowserB1_EyeOpen_rgba16[];
void bowserBlink(s32 *blinkTimer) {
    if (*blinkTimer == 0) {
        if ((s16) (random_float() * 125.0f) == 0) {
            o->oAnimState = 1;
            *blinkTimer = 1;
        }
    } else if (*blinkTimer == 1) {
        o->oAnimState = 2;
        *blinkTimer = 2;
    } else {
        (*blinkTimer)++;
        if (*blinkTimer >= 6)
            o->oAnimState = 0;
        if (*blinkTimer >= 11)
            o->oAnimState = 1;
        if (*blinkTimer >= 13)
            o->oAnimState = 2;
        if (*blinkTimer >= 16) {
            o->oAnimState = 0;
            *blinkTimer = 0;
        }
    }
}
extern Gfx mat_bowserB1_EYE1_002[];
void blinkBowser() {
    u8 *copyThis = 0;
    switch (o->oAnimState) {
        case 0:
            copyThis = (openEyes);
            break;
        case 1:
            copyThis = (halfOpenEyes);
            break;
        case 2:
            copyThis = (closedEyes);
            break;
        case 3:
            copyThis = (angryEyes);
            break;
        case 4:
            copyThis = (angryHappy);
            break;
        case 5:

            break;
        case 6:

            break;
    }
    ((Gfx *) segmented_to_virtual(&mat_bowserB1_EYE1_002[5]))->words.w1 = copyThis;
    // memcpy(segmented_to_virtual(bowserB1_EyeOpen_rgba16), copyThis, 0x1000);
}
extern Gfx mat_bitdw_dl_DISAPPEARB1[];
void bowserbosscode(void) {
    Gfx *disapp = segmented_to_virtual(&mat_bitdw_dl_DISAPPEARB1[3]);
    u8 app;
    if (app = (disapp->words.w1 & 0x00FF)){
        app = approach_s16_symmetric(app, 0, 2);
        disapp->words.w1 = app;
    }
    bowserBlink(&o->oBobombBlinkTimer);
    switch (o->oAction) {
        case 0:
            if (gMarioState->pos[2] > -6000.f) {
                if (gMarioState->pos[1] == gMarioState->floorHeight) {
                    o->oAction++;
                    gMarioState->statusForCamera->cameraEvent = 0xc8;
                    currentScene = &bowserScene;
                    cur_obj_init_animation(0);
                }
            }
            break;
        case 1:
            if (o->header.gfx.unk38.animFrame > 298) {
                o->header.gfx.unk38.animFrame -= 200;
            }
            if (timer > 130) {
                timer = 130;
                timeAccumulated = 130;
            }
            if (o->header.gfx.unk38.animFrame > 298) {
                o->header.gfx.unk38.animFrame -= 200;
            }
            if (gDialogResponse) {
                o->oAction++;
                o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvStaticObject);
                o->oHiddenBlueCoinSwitch->header.gfx.sharedChild = gLoadedGraphNodes[0x3F];
                play_sound(SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE, gDefaultSoundArgs);
                o->header.gfx.unk38.animFrame = 299;
            }
            if (o->oTimer >= 70) {
                create_dialog_box(119);
            }
            break;
        case 2:
#define TIMEFORDOWN 55
            o->oAnimState = 4;
            if (o->oTimer < TIMEFORDOWN) {
                o->oHiddenBlueCoinSwitch->oPosY -= 20.f;
            }
            if (o->oTimer == TIMEFORDOWN + 20) {
                o->oPosX = 961.f;
                o->oPosY = 7130.f;
                o->oPosZ = 6538.f;
                play_sound(SOUND_OBJ_BOWSER_LAUGH, gDefaultSoundArgs);
            }
            if (o->oTimer > TIMEFORDOWN + 30) {
                if (o->oTimer == TIMEFORDOWN + 31) {
                    play_sound(SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE, gDefaultSoundArgs);
                }
                o->oHiddenBlueCoinSwitch->oPosY += 10.f;
            } else {
            }
            if (o->oTimer > TIMEFORDOWN + TIMEFORDOWN + 80) {
                o->oAction++;
                // obj_mark_for_deletion(o->oHiddenBlueCoinSwitch);
                cur_obj_nearest_object_with_behavior(bhvMechBowserBoss)->oSubAction = 1;
            }
            break;
        case 3:
            if (!currentScene) {
                o->oHiddenBlueCoinSwitch->oPosY = 7674.f;
            }
            if (o->oTimer > 200) {
                cur_obj_set_model(0);
            }
            break;
    }
    blinkBowser();
}
extern f32 newcam_pos[3];
#define FLYSPEEDMUL4 15.f
extern struct CutsceneSplinePoint bitdw_area_2_spline_LAKITUSPLINE[];
void supernerd(void) {
    f32 x, z, y;
    s16 targetAngle;
    if (!o->oAction) {
        if (currentScene == segmented_to_virtual(&bowserScene3)) {
            o->oAction = 1;
        }
    } else if (o->oAction == 1) {
        cur_obj_play_sound_1(SOUND_AIR_LAKITU_FLY);
        x = ((s16 *) segmented_to_virtual(bitdw_area_2_spline_LAKITUSPLINE))[1 + opacity];
        z = ((s16 *) segmented_to_virtual(bitdw_area_2_spline_LAKITUSPLINE))[3 + opacity];
        y = ((s16 *) segmented_to_virtual(bitdw_area_2_spline_LAKITUSPLINE))[2 + opacity];
        targetAngle = atan2s(z - o->oPosZ, x - o->oPosX);
        o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x800);
        if (sqrtf((x - o->oPosX) * (x - o->oPosX) + (z - o->oPosZ) * (z - o->oPosZ)) < 300.f) {
            opacity += 4;
            if (opacity == 16) {
                o->oAction = 2;
                o->oVelY *= 0.7f;
                goto physics;
            }
        }
        if (absf(o->oPosY - y) > 10.f) {
            o->oVelY += (y - o->oPosY) / absf(o->oPosY - y) * 2.f;
        } else {
            o->oVelY *= 0.99f;
        }
        if (o->oVelY > 30.f) {
            o->oVelY = 30.f;
        }
        if (o->oVelY < -30.f) {
            o->oVelY = -30.f;
        }
        o->oForwardVel = approach_f32(
            o->oForwardVel, FLYSPEEDMUL4 * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 2.f, 2.f);
        if (o->oOpacity > 10) {
            o->oVelY *= 0.91f;
            o->oForwardVel *= 0.91f;
        }
    } else if (o->oAction == 2) {
        cur_obj_play_sound_1(SOUND_AIR_LAKITU_FLY);
        o->oForwardVel = approach_f32(o->oForwardVel, 0.f, 2.f, 2.f);
        if (!currentScene) {
            o->oForwardVel = approach_f32(o->oForwardVel, -40.f, 5.f, 5.f);
            o->oVelY += 3.f;
            if (o->oTimer > 100) {
                obj_mark_for_deletion(o);
            }
        } else {
            o->oVelY = approach_f32(o->oVelY, sins(o->oTimer * 0x400) * 7.f, 2.f, 2.f);
            o->oTimer = 0;
        }
    }
physics:
    targetAngle = atan2s(newcam_pos[2] - o->oPosZ, newcam_pos[0] - o->oPosX);
    o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, targetAngle, 0x100);
        obj_move_xyz_using_fvel_and_yaw();
}
