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
#include "audio/external.h"
#include "audio/internal.h"
#include "audio/load.h"
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject
extern f32 newcam_pos[3];

extern u8 bolted;
extern Gfx envColor[];
#define LIGHTINGTIMER (envColor[0].words.w1 & 0x000000FF)
// generate polgonal lightning instead?
void lightning(void) {
    struct Object *bolt;
    u16 dir;
    f32 dist;
    if (o->oBehParams2ndByte) {
        if (bolted) {
            bolted = 0;
            bolt = spawn_object(o, 0x35, bhvLightning);
            dir = random_u16();
            dist = random_f32_around_zero(5000.f) + 10000.f;
            bolt->oPosX = sins(dir) * dist;
            bolt->oPosZ = coss(dir) * dist;
            bolt->oFaceAngleYaw = random_u16();

            bolt = spawn_object(o, 0x35, bhvLightning);
            dir = random_u16();
            dist = random_f32_around_zero(5000.f) + 10000.f;
            bolt->oPosX = sins(dir) * dist;
            bolt->oPosZ = coss(dir) * dist;
            bolt->oFaceAngleYaw = random_u16();
            bolt->oAction = 1;
        }
    } else {
        if ((o->oTimer == 10) && (o->oAction)) {
            // play_sound(SOUND_GENERAL_VOLCANO_EXPLOSION, o->header.gfx.cameraToObject);
            play_sound(SOUND_GENERAL_VOLCANO_EXPLOSION, gDefaultSoundArgs);
        }
        if (o->oTimer > 10) {
            mark_obj_for_deletion(o);
        }
    }
}

Vtx SailVertices[6][15];
extern Vtx bbh_dl_Zail_mesh_layer_4_vtx_0[15];
extern Vtx bbh_dl_Zail_001_mesh_layer_4_vtx_0[15];
extern Vtx bbh_dl_Zail_002_mesh_layer_4_vtx_0[15];
extern Vtx bbh_dl_Zail_003_mesh_layer_4_vtx_0[15];
extern Vtx bbh_dl_Zail_004_mesh_layer_4_vtx_0[15];
extern Vtx bbh_dl_Zail_005_mesh_layer_4_vtx_0[15];
Vtx *sailList[6] = { &bbh_dl_Zail_mesh_layer_4_vtx_0,     &bbh_dl_Zail_001_mesh_layer_4_vtx_0,
                     &bbh_dl_Zail_002_mesh_layer_4_vtx_0, &bbh_dl_Zail_003_mesh_layer_4_vtx_0,
                     &bbh_dl_Zail_004_mesh_layer_4_vtx_0, &bbh_dl_Zail_005_mesh_layer_4_vtx_0

};

#pragma GCC reset_options
f32 findHighestVertHeight(f32 maxHeight, Vtx *list) {
    f32 bestHeight = -10000.f;
    u32 i;
    for (i = 0; i < 15; i++) {
        if ((bestHeight < list[i].n.ob[1]) && (list[i].n.ob[1] < maxHeight)) {
            bestHeight = list[i].n.ob[1];
        }
    }
    return bestHeight;
}
s32 findSameXPos(Vtx *list, u16 index, u16 listIndex) {
    u32 i;
    for (i = 0; i < 15; i++) {
        if (((list[i].n.flag & 0x0F00) == index) && (!SailVertices[listIndex][i].v.cn[3])) {
            return ((Vtx *) segmented_to_virtual(sailList[listIndex]))[i].v.ob[0];
        }
    }
}

/*#define top         OBJECT_FIELD_F32(0x1F)
#define  topmid         OBJECT_FIELD_F32(0x1E)
#define  botmid         OBJECT_FIELD_F32(0x1D)*/
// minimize segmented_to_virtuals
#define LEFTNUM 50.f
#define RIGHTNUM -50.f
f32 windRightStrength = 0.f;
f32 windLeftStrength = 0.f;
void animateSails(void) {
    u32 i, j;
    f32 top, topmid, botmid;
    u32 number;
    f32 proportionalStrength;
    f32 blowStrength;
    f32 strengthMod;
    f32 windR, windM, windL;
    Vtx *currVert;
    if (random_u16() % 70 == 0) {
        o->oBobombBuddyPosXCopy = 20.f;
    }
    if (random_u16() % 90 == 0) {
        o->oBobombBuddyPosZCopy = 25.f;
    }
    if (random_u16() % 30 == 0) {
        o->oBobombBuddyPosXCopy = 7.f;
    }
    if (random_u16() % 20 == 0) {
        o->oBobombBuddyPosZCopy = 5.f;
    }
    if (o->oTimer > 200) {
        if (random_u16() % 170 == 0) {
            o->oBobombBuddyPosXCopy = 35.f;
        }
        if (random_u16() % 150 == 0) {
            o->oBobombBuddyPosZCopy = 32.f;
        }
    }
    o->oVelX += o->oBobombBuddyPosXCopy;
    o->oVelZ += o->oBobombBuddyPosZCopy;
    windRightStrength += o->oVelX;
    windLeftStrength += o->oVelZ;
    windRightStrength *= 0.95f;
    windLeftStrength *= 0.95f;
    o->oBobombBuddyPosXCopy *= 0.9f;
    o->oBobombBuddyPosZCopy *= 0.9f;
    o->oVelX *= 0.8f;
    o->oVelZ *= 0.8f;
#define WINDCLOSURE 0.03f
    windRightStrength = approach_f32_asymptotic(windRightStrength, windLeftStrength, WINDCLOSURE);
    windLeftStrength = approach_f32_asymptotic(windLeftStrength, windRightStrength, WINDCLOSURE);

    windR = windRightStrength;
    windM = (windRightStrength + windLeftStrength) / 2.f;
    windL = windLeftStrength;
    if (!(SailVertices[0][0].v.ob[1])) {
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 15; j++) {
                SailVertices[i][j] = ((Vtx *) segmented_to_virtual(sailList[i]))[j];
            }
        }
        // give each vertex a flag depending on context
        // flag 1: stuck in place
        // flag 2: cant go too far back
        // flag 4: top middle piece
        // flag 8: bottom middle piece
        // flag 16: bottom piece
        // flag 0x00E0: size of the proportinal indexing, goes for whole sail, depends on botmid
        // flag 0xFF00: identifier number
        for (i = 0; i < 6; i++) {
            top = findHighestVertHeight(32000.f, &SailVertices[i]) - 10.f;
            topmid = findHighestVertHeight(top, &SailVertices[i]) - 10.f;
            botmid = findHighestVertHeight(topmid, &SailVertices[i]) - 10.f;
            for (j = 0; j < 15; j++) {
                if (SailVertices[i][j].v.ob[2] > LEFTNUM) {
                    number = 2;
                } else if (SailVertices[i][j].v.ob[2] < RIGHTNUM) {
                    number = 0;
                } else {
                    number = 1;
                    SailVertices[i][j].v.flag += 0x0002;
                }
                if (SailVertices[i][j].v.ob[1] > top) {
                    number += 0;
                    SailVertices[i][j].v.flag += 0x0001;
                } else if (SailVertices[i][j].v.ob[1] > topmid) {
                    number += 3;
                    SailVertices[i][j].v.flag += 0x0004;
                } else if (SailVertices[i][j].v.ob[1] > botmid) {
                    number += 6;
                    SailVertices[i][j].v.flag += 0x0008;
                } else {
                    number += 9;
                    SailVertices[i][j].v.flag += 0x0010;
                }
                SailVertices[i][j].v.flag += number * 256;
                SailVertices[i][j].v.flag +=
                    (((u32) (((botmid + 1050) * 64.f) / -3619.f)) & 0xE) * 0x10;
            }
        }
    } else {
        for (i = 0; i < 6; i++) { // apply wind and material stiffness
            proportionalStrength =
                (512.f - (SailVertices[i][j].v.flag & 0x00E0)) / 1324.f; // small sail connect fast
            blowStrength =
                ((SailVertices[i][j].v.flag & 0x00E0) + 256.f) / 512.f; // big sail connect slow
            for (j = 0; j < 15; j++) {
                currVert = &((Vtx *) segmented_to_virtual(sailList[i]))[j];
                if (SailVertices[i][j].v.flag & 1) {
                    continue;
                }
                if ((SailVertices[i][j].v.flag & 0x08) && SailVertices[i][j].v.cn[3]) {
                    continue;
                }
                if (SailVertices[i][j].v.flag & 0x04) {
                    strengthMod = 0.6f;
                } else if (SailVertices[i][j].v.flag & 0x08) {
                    strengthMod = 0.7f;
                } else {
                    strengthMod = 0.5f;
                }
                if (((SailVertices[i][j].v.flag & 0x0F00) % 0x0300) == 0x0200) {
                    // windR
                    currVert->v.flag =
                        ((s16) currVert->v.flag) + blowStrength * strengthMod * windR * 0.05f;
                } else if (((SailVertices[i][j].v.flag & 0x0F00) % 0x0300) == 0x0100) {
                    // windM
                    currVert->v.flag =
                        ((s16) currVert->v.flag) + blowStrength * strengthMod * windM * 0.05f;
                } else {
                    // windL
                    currVert->v.flag =
                        ((s16) currVert->v.flag) + blowStrength * strengthMod * windL * 0.05f;
                }
#define WEAKPROP 128
#define STRONGPROP 32
                switch ((SailVertices[i][j].v.flag & 0x0F00) >> 8) {
                    case 3:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 4, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 6, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 4:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 3, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 7, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 5, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 5:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 4, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 8, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 6:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 3, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 7, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 9, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 7:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 6, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 8, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 10, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 4, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 8:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 7, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 5, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 11, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 9:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 6, i)) / STRONGPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 10, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 10:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 11, i)) / WEAKPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 7, i)) / STRONGPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 9, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                    case 11:
                        currVert->v.flag +=
                            ((-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 8, i)) / STRONGPROP
                             + (-currVert->v.ob[0] + findSameXPos(&SailVertices[i], 10, i)) / WEAKPROP)
                            * proportionalStrength;
                        break;
                }
            }
        }
        for (i = 0; i < 6; i++) { // apply speed and friction
            currVert = &((Vtx *) segmented_to_virtual(sailList[i]))[0];
            for (j = 0; j < 15; j++) {
                if ((SailVertices[i][j].v.flag & 0x08) && SailVertices[i][j].v.cn[3]) {
                    continue;
                }
                currVert[j].v.flag = ((s16) currVert[j].v.flag) - (currVert[j].v.ob[0] / 16);
                currVert[j].v.ob[0] += (s16) (currVert[j].v.flag);
                if (SailVertices[i][j].v.flag & 2) {
                    if (currVert[j].v.ob[0] < 0) {
                        currVert[j].v.ob[0] = 0;
                    }
                }
            }
        }

        for (i = 0; i < 6; i++) { // copy xPos of already calculated sailpoints
            currVert = &((Vtx *) segmented_to_virtual(sailList[i]))[0];
            for (j = 0; j < 15; j++) {
                if ((SailVertices[i][j].v.flag & 0x08) && SailVertices[i][j].v.cn[3]) {
                    currVert[j].v.ob[0] =
                        findSameXPos(&SailVertices[i], SailVertices[i][j].v.flag & 0x0F00, i);
                }
            }
        }
    }
}
#include "game/OPT_FOR_SIZE.h"
void shipCannon(void) {
    switch (o->oAction) {
        case 0:
            o->oFaceAnglePitch = 0;
            if (gMarioState->floor && (gMarioState->floor->object == o)
                && (gMarioState->pos[1] == gMarioState->floorHeight)
                && (gMarioState->action != ACT_LEDGE_GRAB)
                && (gMarioState->action != ACT_LEDGE_CLIMB_FAST
                    && (gMarioState->action != ACT_LEDGE_CLIMB_SLOW_1)
                    && (gMarioState->action != ACT_LEDGE_CLIMB_SLOW_2))) {
                o->oAction = 1;
                m->action = ACT_WAITING_FOR_DIALOG;
            }
            load_object_collision_model();
            break;
        case 1:
            gMarioState->pos[0] = approach_f32_asymptotic(gMarioState->pos[0], o->oPosX, 0.2f);
            gMarioState->pos[2] = approach_f32_asymptotic(gMarioState->pos[2], o->oPosZ, 0.2f);
            if (o->oTimer > 10) {
                o->oAction++;
                cur_obj_play_sound_2(SOUND_OBJ_CANNON2);
            }
            load_object_collision_model();
            break;
        case 2:
            if (o->oTimer < 13) {
                gMarioState->pos[1] -= o->oTimer * 4;
            } else {
                gMarioState->faceAngle[1] = o->oFaceAngleYaw;
                gMarioState->marioObj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
                gMarioState->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
            }
            if (o->oTimer > 20) {
                o->oFaceAnglePitch = approach_s16_symmetric(
                    o->oFaceAnglePitch, 0x3000 - o->oBehParams2ndByte * 0x40, 0x200);
                play_sound(SOUND_MOVING_AIM_CANNON, m->marioObj->header.gfx.cameraToObject);
            }
            if (o->oTimer > 60) {
                play_sound(SOUND_ACTION_FLYING_FAST, m->marioObj->header.gfx.cameraToObject);
                play_sound(SOUND_OBJ_POUNDING_CANNON, m->marioObj->header.gfx.cameraToObject);
                m->forwardVel = 70.f - o->oBehParams2ndByte / 8.f;
                m->vel[1] = 50.f + o->oBehParams2ndByte / 2.f;
                o->oAction++;
                set_mario_action(m, ACT_SHOT_FROM_CANNON, 0);
                o->oAngleVelPitch = -0x800 + o->oBehParams2ndByte * 8;
            }
            break;
        case 3:
            o->oFaceAnglePitch += o->oAngleVelPitch;
            o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 0, 0x0080);
            o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0x0100);
            if (o->oTimer > 1) {
                gMarioState->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                m->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
            }
            if (o->oTimer > 100) {
                o->oAction = 0;
            }
            break;
    }
}

void ghostplatform(void) {
    switch (o->oAction) {
        case 0:
            if (o->oOpacity) {
                o->oOpacity--;
                if (o->oOpacity & 2) {
                    cur_obj_hide();
                } else {
                    cur_obj_unhide();
                }
            } else {
                o->oAnimState = 0;
            }
            o->oAction = cur_obj_is_mario_on_platform();
            o->oVelY = 0.f;
            load_object_collision_model();
            break;
        case 1:
            if (!o->oTimer) {
                cur_obj_play_sound_2(SOUND_GENERAL_BIG_CLOCK);
            }
            o->oAnimState = 1;
            /*if (!cur_obj_is_mario_on_platform()) {
                o->oAction++;
                cur_obj_update_floor_height();
            }*/
            o->oGraphYOffset = (o->oTimer & 1) * -8.f;
            if (o->oTimer > 64) {
                o->oAction++;
                cur_obj_update_floor_height();
                o->oGraphYOffset = 0;
            }

            load_object_collision_model();
            break;
        case 2:
            o->oVelY -= 2.f;
            o->oPosY += o->oVelY;
            if (o->oPosY < o->oFloorHeight) {
                o->oAction++;
                cur_obj_set_model(0);
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
            }
            load_object_collision_model();
            break;
        case 3:
            if (o->oTimer > 240) {
                cur_obj_set_model(0x36);
                o->oPosY = o->oHomeY;
                o->oAction = 0;
                o->oOpacity = 40;
            }
            break;
    }
}

u32 getHitBox() {
    f32 x, y, z;
    x = o->oPosX - gMarioState->pos[0];
    y = (o->oPosY - 100.f) - gMarioState->pos[1];
    z = o->oPosZ - gMarioState->pos[2];
    return (sqrtf(x * x + y * y + z * z) < 150.f);
}
// hang on until you press A again
// swing back and forth in idle
// hold stick to rotate mario
void ghostLantern(void) {
    f32 speedScale = 1.f;
    if (!o->oOpacity) {
        o->oOpacity = random_u16() * 0x100;
    }
    if (!(o->oBehParams & 0x01000000)) {
        o->oPosY += sins(o->oOpacity += 0x100) * 2.f;
    }
    o->oAngleVelPitch -= o->oFaceAnglePitch / 64;
    o->oAngleVelPitch *= .99f;
    o->oFaceAnglePitch += o->oAngleVelPitch;
    switch (o->oAction) {
        case 0:
            if (o->oOpacity & 0x4000) {
                o->oAngleVelPitch += 0x0020;
            }
            if (o->oTimer > 20) {
                if (getHitBox()) {
                    o->oAction++;
                    gMarioState->action = ACT_HANG_LANTERN;
                    gMarioState->usedObj = o;
                    o->oAngleVelPitch = -gMarioState->forwardVel / 0.01581917687f / 2.f;
                }
            }
            break;
        case 1:
#define OFFSET -165.f;
            speedScale += 4.f - absf((o->oAngleVelPitch - (o->oFaceAnglePitch / 64)) / 0x1000);
            gMarioState->faceAngle[1] =
                approach_s16_symmetric(gMarioState->faceAngle[1], o->oMoveAngleYaw, 0xC00);
            gMarioState->action = ACT_HANG_LANTERN;
            gMarioState->usedObj = o;
            gMarioState->pos[0] = o->oPosX + sins(o->oFaceAnglePitch) * sins(o->oMoveAngleYaw) * OFFSET;
            gMarioState->pos[1] = o->oPosY + coss(o->oFaceAnglePitch) * OFFSET;
            gMarioState->pos[2] = o->oPosZ + sins(o->oFaceAnglePitch) * coss(o->oMoveAngleYaw) * OFFSET;
            o->oAngleVelPitch -= coss(gMarioState->intendedYaw - o->oFaceAngleYaw)
                                 * gMarioState->intendedMag * speedScale;
            o->oAngleVelPitch *= .95f;
            if (gMarioState->controller->buttonPressed & A_BUTTON) {
                o->oAction = 0;
                gMarioState->action = ACT_TRIPLE_JUMP;
                gMarioState->vel[1] =
                    o->oAngleVelPitch * coss(o->oFaceAnglePitch - 0x4000) * 0.01581917687f * 2.f;
                gMarioState->forwardVel =
                    o->oAngleVelPitch * sins(o->oFaceAnglePitch - 0x4000) * 0.01581917687f * 2.f;
                if (gMarioState->vel[1] < 30.f) {
                    gMarioState->vel[1] = 30.f;
                }
            }
            switch (o->oAnimState) {
                case 0:
                    if (absi(o->oAngleVelPitch) > 0x400) {
                        o->oAnimState++;
                        cur_obj_play_sound_2(SOUND_GENERAL_SWISH_AIR);
                    }
                    break;
                case 1:
                    if (absi(o->oAngleVelPitch) < 0x200) {
                        o->oAnimState = 0;
                    }
                    break;
            }
            break;
    }
}
void destroybarrelcode(void) {
    if (o->oAction == 0) {
        if (cur_obj_is_mario_ground_pounding_platform()) {
            o->oAction++;
            o->oTimer = 0;
        }
    }
#define MINSUB 0.1167f
    if (o->oAction == 1) {
        o->header.gfx.scale[1] -= MINSUB;
        o->header.gfx.scale[0] = sqrtf(1.f / o->header.gfx.scale[1]);
        o->header.gfx.scale[2] = o->header.gfx.scale[0];
        if (o->header.gfx.scale[1] < (MINSUB * 4)) {
            mark_obj_for_deletion(o);
            spawn_mist_particles_variable(0, 0, 46.f);
            spawn_triangle_break_particles(30, 138, 3.0f, 4);
            switch (o->oBehParams2ndByte) {
                case 0:
                    spawn_object(o, 0, bhvTemporaryYellowCoin);
                    break;
                case 1:
                    spawn_object(o, 0, bhvThreeCoinsSpawn);
                    break;
                case 2:
                    spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
                    break;
                case 3:
                    spawn_object(o, 0, bhvTenCoinsSpawn);
                    break;
            }
        }
    }
    load_object_collision_model();
}

// start peeking
// peek
// stop peeking (anim stops here)
// throw mario up
void boobarrel(void) {
    switch (o->oAction) {
        case 0:
            cur_obj_init_animation(0); // startpeek
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction++;
            }
            break;
        case 1:
            cur_obj_init_animation(1); // peeking
            if (o->oDistanceToMario < 1000.f) {
                o->oAction++;
            }
            break;
        case 2:
            cur_obj_init_animation(2); // stoppeek
            if (o->oDistanceToMario > 1000.f) {
                o->oAction = 0;
            }
            if (cur_obj_is_mario_on_platform()) {
                o->oAction++;
                m->action = ACT_THROWN_BACKWARD;
                play_sound(SOUND_MARIO_WAAAOOOW, gDefaultSoundArgs);
                cur_obj_play_sound_2(SOUND_OBJ_HEAVEHO_TOSSED);
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_UNK2;
                m->faceAngle[1] = o->oMoveAngleYaw;
                m->vel[1] = 65.f + (o->oBehParams & 0xff);
                m->forwardVel = -25.f;
            }
            break;
        case 3:
            cur_obj_init_animation(3); // shootup
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 0;
            }
            break;
    }
    load_object_collision_model();
}

void breakabletile(void) {
    if (cur_obj_is_mario_ground_pounding_platform()) {
        mark_obj_for_deletion(o);
        spawn_mist_particles_variable(0, 0, 46.f);
        spawn_triangle_break_particles(30, 138, 3.0f, 4);
    }
    load_object_collision_model();
}

u32 isInVision() {
    f32 pitchToMario;
    f32 x, y, z;
    x = gMarioState->pos[0] - o->oPosX;
    z = gMarioState->pos[2] - o->oPosZ;
    y = gMarioState->pos[1] - o->oPosY;
    pitchToMario = -atan2s(sqrtf(x * x + z * z), y);
    if (o->oDistanceToMario < 1200.f) {
        if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < 0x1800) {
            if (abs_angle_diff(o->oFaceAnglePitch, pitchToMario) < 0x1400) {
                return 1;
            }
        }
    }
    return 0;
}
// make their model pulsate white to turkoise
// different idle and attackstart behavior depending on bParam2
// charge at mario once they see him
// 25% chance to do half a looping after missing mario and attack again
// afterwrards, turn into ghosts that slowly walk towards mario, just like boos. go back to idle if
// mario is far away from their home.
void booGuy(void) {
    struct WallCollisionData collisionData;
    u16 targetAngle;
    u16 targetPitch;
    o->oPosY += sins(o->oOpacity += 0x800) * 0.5f;
    o->oGraphYOffset = 50.f;
    o->oIntangibleTimer = 0;
    switch (o->oAction) {
        case 0:
            switch (o->oBehParams2ndByte) {
                case 0:
                    if (!o->oHiddenBlueCoinSwitch) {
                        o->oHiddenBlueCoinSwitch =
                            spawn_object_at_origin(gCurrentObject, 0, MODEL_BUBBLE, bhvGoombaBubble);
                        obj_copy_pos_and_angle(o->oHiddenBlueCoinSwitch, gCurrentObject);
                    }
                    cur_obj_init_animation(1);
#define SLEEPTIME 59
                    if (o->header.gfx.unk38.animFrame > SLEEPTIME) {
                        o->header.gfx.unk38.animFrame -= SLEEPTIME;
                    }
                    if (o->oDistanceToMario < 1500.f) {
                        if ((gMarioState->action == ACT_GROUND_POUND_LAND)) {
                            o->oAction = 3;
                        }
                        if ((gMarioState->forwardVel + gMarioState->vel[1])
                            > o->oDistanceToMario / 15.f) {
                            o->oAction = 3;
                        }
                    }
                    break;
                case 1:
                    // keep watch (looks left and right)
                    cur_obj_init_animation(0);
                    if (isInVision()) {
                        o->oAction = 3;
                    }
                    break;
                case 2:
                    // look at something
                    cur_obj_init_animation(2);
                    if (isInVision()) {
                        o->oAction = 3;
                    }
                    break;
                case 3:
                    // patrol
                    cur_obj_init_animation(3);
                    switch (o->oSubAction) {
                        case 0:
                            o->oForwardVel = 10.f;
                            if (o->oTimer > (o->oBehParams & 0xFF) + 30) {
                                o->oSubAction = 1;
                                o->oTimer = 0;
                            }
                            break;
                        case 1:
                            o->oForwardVel = 0.f;
                            o->oMoveAngleYaw += 0x400;
                            if (o->oTimer > 0x1F) {
                                o->oSubAction = 0;
                                o->oTimer = 0;
                            }
                            break;
                    }
                    if (isInVision()) {
                        o->oAction = 3;
                    }
                    break;
                case 4:

                    break;
            }
            break;
        case 1:
            // charge anim
            cur_obj_init_animation(4);
#define CHARGETIME 19
            if (o->header.gfx.unk38.animFrame > CHARGETIME) {
                o->header.gfx.unk38.animFrame -= CHARGETIME;
            }
            targetAngle = atan2s(gMarioState->pos[2] - o->oPosZ, gMarioState->pos[0] - o->oPosX);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x100);
            targetPitch =
                -atan2s(sqrtf((gMarioState->pos[2] - o->oPosZ) * (gMarioState->pos[2] - o->oPosZ)
                              + (gMarioState->pos[0] - o->oPosX) * (gMarioState->pos[0] - o->oPosX)),
                        gMarioState->pos[1] - o->oPosY);
            o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x100);
#define FLYSPEEDMUL4 10.f
            if (o->oTimer < 10) {
                o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x800);
                o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x800);
            }
            o->oForwardVel = approach_f32(o->oForwardVel,
                                          FLYSPEEDMUL4 * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                              * (coss(targetPitch - o->oFaceAnglePitch) + 1.0f),
                                          2.f, 2.f);
            if ((o->oDistanceToMario < 500.f) || (o->oTimer > 60)) {
                o->oAction = 3;
            }
            break;
        case 2:
            // looping while charge
            cur_obj_init_animation(4);
            if (o->header.gfx.unk38.animFrame > CHARGETIME) {
                o->header.gfx.unk38.animFrame -= CHARGETIME;
            }
            break;
        case 3:
            // slowly fly at mario
            // if (cur_obj_check_if_near_animation_end()) {
            cur_obj_init_animation(3);
            // }
            targetAngle = atan2s(gMarioState->pos[2] - o->oPosZ, gMarioState->pos[0] - o->oPosX);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x400);
            targetPitch =
                -atan2s(sqrtf((gMarioState->pos[2] - o->oPosZ) * (gMarioState->pos[2] - o->oPosZ)
                              + (gMarioState->pos[0] - o->oPosX) * (gMarioState->pos[0] - o->oPosX)),
                        gMarioState->pos[1] - o->oPosY);
            o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x400);
#define FLYSPEEDMUL4 4.f
            o->oForwardVel = approach_f32(o->oForwardVel,
                                          FLYSPEEDMUL4 * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                              * (coss(targetPitch - o->oFaceAnglePitch) + 1.0f),
                                          0.5f, 2.f);
            if (cur_obj_mario_far_away(4000.f)) {
                o->oAction = 5;
            }
            break;
        case 4:
            // die
            // make him shrink too
            o->oIntangibleTimer = -1;
            o->oFaceAnglePitch += 0x800;
            o->oFaceAngleRoll += 0xC00;
            collisionData.offsetY = 0.f;
            collisionData.radius = 50.f;
            collisionData.x = (s16) o->oPosX;
            collisionData.y = (s16) o->oPosY;
            collisionData.z = (s16) o->oPosZ;
            if ((o->oTimer > 23) || (find_wall_collisions(&collisionData))) {
                mark_obj_for_deletion(o);
                spawn_mist_particles_variable(0, 0, 46.f);
                obj_spawn_yellow_coins(o, 2);
            }
            break;
        case 5:
            // go home
            o->oVelY -= 2.f;
            o->oVelX = approach_f32_symmetric(o->oVelX, 0, 2.f);
            o->oVelZ = approach_f32_symmetric(o->oVelZ, 0, 2.f);
            if (o->oTimer > 100) {
                o->oAction = 0;
                o->oHiddenBlueCoinSwitch = 0;
                o->oPosX = o->oHomeX;
                o->oPosY = o->oHomeY;
                o->oPosZ = o->oHomeZ;
            }
            break;
    }
    if (o->oInteractStatus & INT_STATUS_WAS_ATTACKED) {
        o->oAction = 4;
        o->oMoveAngleYaw = gMarioState->faceAngle[1] + 0x8000;
        o->oForwardVel = -35.f;
        o->oVelY = 10.f;
        o->oIntangibleTimer = 0 - 1;
    }
    if ((o->oAction != 5) && (o->oAction != 4)) {
        o->oVelY = sins(-o->oFaceAnglePitch) * o->oForwardVel;
        o->oVelX = coss(-o->oFaceAnglePitch) * sins(o->oMoveAngleYaw) * o->oForwardVel;
        o->oVelZ = coss(-o->oFaceAnglePitch) * coss(o->oMoveAngleYaw) * o->oForwardVel;
    } else {
        cur_obj_compute_vel_xz();
    }
    cur_obj_move_using_vel();
    o->oInteractStatus = 0;
    // can go through walls, but not above ceilings or OOB
    // kill by GP or dive, drops 2 coins
}

extern struct CutsceneSplinePoint bbh_area_3_spline_EelSceneFocIntro[];
extern struct CutsceneSplinePoint bbh_area_3_spline_EelScenePosIntro[];

struct CutsceneJump {
    int length;
    struct CutsceneSplinePoint *pos;
    struct CutsceneSplinePoint *foc;
    struct CutsceneJump *next;
};
extern s16 sCutsceneDialogID;
extern s32 gDialogResponse;
extern struct CutsceneJump *currentScene;
extern int timer;
extern int timeAccumulated;
struct CutsceneJump eelScene = { 132, bbh_area_3_spline_EelScenePosIntro,
                                 bbh_area_3_spline_EelSceneFocIntro, 0 };

extern Gfx bbh_dl_Cube_001_mesh_layer_4_tri_0[];
extern u8 bbh_dl_ComedyclubSign2_rgba16[];
extern u8 bbh_dl_ComedyclubSign_rgba16[];

// 2, 13, 24, 35
u16 eelJokes[6][2] = {
    { 600, 95 }, { 600, 97 }, { 600, 96 }, { 600, 102 }, { 600, 103 }, { 600, 104 }
}; // make this 3, have him pause before laughing and telling the joke
extern s16 gDialogTextPos;
void unagiJokes(void) {
    Gfx *light = segmented_to_virtual(bbh_dl_Cube_001_mesh_layer_4_tri_0);
    u8 *texture = bbh_dl_ComedyclubSign_rgba16;
    if (o->oOpacity) {
        o->oOpacity--;
        texture = bbh_dl_ComedyclubSign2_rgba16;
        play_sound(SOUND_AIR_AMP_BUZZ, gDefaultSoundArgs);
    }
    light[2].words.w1 = texture;
    light[13].words.w1 = texture;
    light[24].words.w1 = texture;
    light[35].words.w1 = texture;
    if ((random_u16() % 20) == 0) {
        o->oOpacity = (random_u16() % 5) + 2;
        play_sound(SOUND_AIR_AMP_BUZZ, gDefaultSoundArgs);
    }
    switch (o->oAction) {
        case 0:
            cur_obj_init_animation(2);
            if (gMarioState->pos[1] == gMarioState->floorHeight) {
                o->oAction++;
                gMarioState->statusForCamera->cameraEvent = 0xc8;
                currentScene = &eelScene;
            }
            break;
        case 1:
            if (timer > 130) {
                timer = 130;
                timeAccumulated = 130;
            }
            if (!o->oSubAction) {
                if (o->oTimer > 120) {
                    if (cur_obj_check_if_near_animation_end()) {
                        cur_obj_init_animation(1);
                        o->oSubAction = 1;
                    }
                }
            }
            if (o->oSubAction) {
                if (o->oTimer > eelJokes[gCurrActNum][0]) {
                    if (cur_obj_check_if_near_animation_end()) {
                        cur_obj_init_animation(0);
                        play_music(1, SEQUENCE_ARGS(15, SEQ_EVENT_EELJOKE), 0);
                    }
                } else {
                    if (o->header.gfx.unk38.animFrame > 71) {
                        o->header.gfx.unk38.animFrame -= 71;
                    }
                }
                if (gDialogResponse) {
                    o->oAction++;
                    switch (random_u16() % 2) {
                        case 0:
                            play_sound(SOUND_MARIO_MAMA_MIA, gDefaultSoundArgs);
                            break;
                        case 1:
                            play_sound(SOUND_MARIO_WAAAOOOW, gDefaultSoundArgs);
                            break;
                    }
                } else {
                    create_dialog_box(eelJokes[gCurrActNum][1]);
                }
            }
            break;
        case 2:
            break;
    }
}

static float const E = 2.718281828459f;
static float slow_logf(float x) {
    float p = 0.0f;
    float r = 0.0f, c = -1.0f;
    int i;

    if (x == 0.0f) {
        return -9999999999999999999999999999999999999999999999999999999.f;
    }
    while (x < 0.5f) {
        x *= E;
        ++p;
    }

    x -= 1.0f;
    for (i = 1; i < 8; ++i) {
        c *= -x;
        r += c / i;
    }
    return r - p;
}
static float slow_expf(float x) {
    float r = 1.0f, c = 1.0f;
    int i;
    x = -x;
    for (i = 1; i < 8; ++i) {
        c *= x / i;
        r += c;
    }
    return 1.0f / r;
}
float slow_powf(float base, float exponent) {
    if (base <= 0.0f) {
        return 0.0f;
    }
    return slow_expf(exponent * slow_logf(base));
}

void scaleFish(void) {
    cur_obj_scale(1.f + o->oBehParams2ndByte / 100.f);
    if (!o->oOpacity) {
        o->oOpacity = 1;
        o->header.gfx.unk38.animFrame = random_u16() % 30;
    }
}
#define scaleX o->header.gfx.scale[0]
#define scaleY o->header.gfx.scale[1]
#define scaleZ o->header.gfx.scale[2]
void jellyblock(void) {
    f32 x, y, z;
    f32 normalizing = 1.f;
    u16 moveVec;
    f32 floorY;
    x = o->oPosX;
    y = o->oPosY;
    z = o->oPosZ;
    o->oCollisionDistance = 6000.f;
    if (cur_obj_is_mario_ground_pounding_platform()) {
        if (!o->oSubAction) {
            o->oTimer = 0;
            cur_obj_play_sound_2(SOUND_OBJ_SPINY_UNK59);
        }
        o->oSubAction = 1;
        if (o->oTimer < 5) {
            o->oBobombBuddyPosYCopy = -0.075f;
        }
    } else {
        o->oSubAction = 0;
    }
    if (!(o->oBehParams & 0x0FF)) {
        if ((gMarioState->wall) && (gMarioState->wall->object == o)) {
            if (m->flags & MARIO_UNKNOWN_31) {
                moveVec = (gMarioState->faceAngle[1] + 0x2000) & 0xC000;
#define PUSHSPEED 8.f
                o->oPosX += sins(moveVec) * PUSHSPEED * (scaleZ - 0.2f);
                o->oPosZ += coss(moveVec) * PUSHSPEED * (scaleX - 0.2f);
                m->pos[0] += sins(moveVec) * PUSHSPEED * (scaleZ - 0.2f);
                m->pos[2] += coss(moveVec) * PUSHSPEED * (scaleX - 0.2f);
            }
        }
#define JELLYSIZE 175.f
        floorY = find_floor_height(x, y + 10.f, z);
        if (absf(floorY - o->oPosY) > scaleY * JELLYSIZE) {
            o->oPosX = x;
            o->oPosZ = z;
        }
        floorY = find_floor_height(x + JELLYSIZE * scaleZ, y + 10.f, z);
        if (absf(floorY - o->oPosY) > 10.f) {
            o->oBobombBuddyPosZCopy = (x - o->oPosX) / JELLYSIZE;
            o->oPosX = x;
        }
        floorY = find_floor_height(x - JELLYSIZE * scaleZ, y + 10.f, z);
        if (absf(floorY - o->oPosY) > 10.f) {
            o->oBobombBuddyPosZCopy = (x - o->oPosX) / JELLYSIZE;
            o->oPosX = x;
        }
        floorY = find_floor_height(x, y + 10.f, z + JELLYSIZE * scaleX);
        if (absf(floorY - o->oPosY) > 10.f) {
            o->oBobombBuddyPosXCopy = (z - o->oPosZ) / JELLYSIZE;
            o->oPosZ = z;
        }
        floorY = find_floor_height(x, y + 10.f, z - JELLYSIZE * scaleX);
        if (absf(floorY - o->oPosY) > 10.f) {
            o->oBobombBuddyPosXCopy = (z - o->oPosZ) / JELLYSIZE;
            o->oPosZ = z;
        }
    }
#define SLOWSCALE 50.f
#define MINSCALE 0.71f
    if (scaleX < MINSCALE) {
        o->oBobombBuddyPosXCopy += (1.f - scaleX) / SLOWSCALE;
    }
    if (scaleY < MINSCALE) {
        o->oBobombBuddyPosYCopy += (1.f - scaleY) / SLOWSCALE;
    }
    if (scaleZ < MINSCALE) {
        o->oBobombBuddyPosZCopy += (1.f - scaleZ) / SLOWSCALE;
    }
    scaleX += o->oBobombBuddyPosXCopy;
    scaleY += o->oBobombBuddyPosYCopy;
    scaleZ += o->oBobombBuddyPosZCopy;
    o->oBobombBuddyPosXCopy = 0;
    o->oBobombBuddyPosYCopy = 0;
    o->oBobombBuddyPosZCopy = 0;
    normalizing = scaleX * scaleY * scaleZ;
    normalizing = slow_powf(normalizing, 0.33333333333f);
    scaleX /= normalizing;
    scaleY /= normalizing;
    scaleZ /= normalizing;
    load_object_collision_model();
}

//                cur_obj_play_sound_1(SOUND_ENV_METAL_BOX_PUSH);

#include "enhancements/puppycam.h"
void booGuyPainting(void) {
    switch (o->oAction) {
        case 0:
            o->oOpacity = 255;
            o->oAction++;
            break;
        case 1:
            if (o->oDistanceToMario < 1250.f) {
                if (abs_angle_diff(newcam_yaw + 0x8000,
                                   atan2s(o->oPosZ - newcam_pos[2], o->oPosX - newcam_pos[0]))
                    < 0x2000) {
                    o->oAction = 2;
                }
            }
            break;
        case 2:
            o->oOpacity -= 5;
            if (o->oOpacity == 0) {
                o->oAction++;
            }
            break;
        case 3:
            o->oHiddenBlueCoinSwitch = spawn_object(o, 0x3B, bhvBooGuy);
            o->oHiddenBlueCoinSwitch->oPosY -= 50.f;
            o->oHiddenBlueCoinSwitch->oAction = 1;
            o->oHiddenBlueCoinSwitch->oIntangibleTimer = 0;
            o->oHiddenBlueCoinSwitch->oBehParams2ndByte = 4;
            o->oAction++;
            break;
        case 4:
            if (o->oHiddenBlueCoinSwitch->oAction == 0) {
                o->oAction = 0;
                mark_obj_for_deletion(o->oHiddenBlueCoinSwitch);
            }
            if (o->oHiddenBlueCoinSwitch->oAction == 4) {
                mark_obj_for_deletion(o);
            }
            break;
    }
}

void wallsword(void) {
    switch (o->oAction) {
        case 0:
            if (o->oDistanceToMario < 1500.f) {
                o->oAction++;
            }
            break;
        case 1:
            cur_obj_shake_y(10.0f);
            if (o->oTimer > 20) {
                o->oAction++;
            }
            break;
        case 2:
            if (o->oTimer > 8) {
                o->oIntangibleTimer = 0;
            }
            o->oAngleVelYaw = approach_s16_symmetric(o->oAngleVelYaw, 0x1300, 0x80);
            o->oFaceAngleYaw += o->oAngleVelYaw;
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 40.f, 1.f);
            break;
    }
    o->oInteractStatus = 0;
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x100);
    if (o->oMoveFlags & (OBJ_MOVE_HIT_WALL | OBJ_MOVE_HIT_EDGE)) {
        spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
        obj_mark_for_deletion(o);
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
}

void rotatetunnel(void) {
    if (o->oBehParams2ndByte) {
        o->oAngleVelRoll = 0x080;
    } else {
        o->oAngleVelRoll = -0x080;
    }
    o->oFaceAngleRoll += o->oAngleVelRoll;
    if (absf(o->oPosX - gMarioState->pos[0]) < 1175) {
        load_object_collision_model();
    }
}
#define Flame1 OBJECT_FIELD_OBJ(0x1B)
#define Flame2 OBJECT_FIELD_OBJ(0x1C)
#define Flame3 OBJECT_FIELD_OBJ(0x1D)
#define Flame4 OBJECT_FIELD_OBJ(0x1E)
#define Flame5 OBJECT_FIELD_OBJ(0x1F)
#define Flame6 OBJECT_FIELD_OBJ(0x20)
#define Flame7 OBJECT_FIELD_OBJ(0x21)
#define Flame8 OBJECT_FIELD_OBJ(0x22)
#define Flame9 OBJECT_FIELD_OBJ(0x1A)
extern void obj_build_relative_transform(struct Object *obj);
void chandelier(void) {
    u8 i;
    if (!o->oOpacity) {
        o->oAngleVelPitch = 0x40;
        o->oOpacity = 1;
        for (i = 0; i < 9; i++) {
            o->OBJECT_FIELD_OBJ(0x1A + i) = spawn_object(o, MODEL_BLUE_FLAME, bhvFlame);
        }
    }
    obj_copy_pos_and_angle(o->OBJECT_FIELD_OBJ(0x1A), o);
    obj_set_parent_relative_pos(o->OBJECT_FIELD_OBJ(0x1A), 0, -1786.f, 0);
    obj_build_relative_transform(o->OBJECT_FIELD_OBJ(0x1A));

    for (i = 0; i < 8; i++) {
        obj_copy_pos_and_angle(o->OBJECT_FIELD_OBJ(0x1B + i), o);
        obj_set_parent_relative_pos(o->OBJECT_FIELD_OBJ(0x1B + i), sins(0x2000 * i) * 930.f, -1816.f,
                                    coss(0x2000 * i) * 930.f);
        obj_build_relative_transform(o->OBJECT_FIELD_OBJ(0x1B + i));
    }
    o->oAngleVelPitch -= o->oFaceAnglePitch / 1024;
    o->oFaceAngleYaw += 0x0020;
    o->oAngleVelYaw = 0x0020;
    o->oFaceAnglePitch += o->oAngleVelPitch;
    load_object_collision_model();
}

void pushbehavior(f32 xwall, f32 zwall) {
    f32 x, y, z;
    u16 moveVec;
    f32 floorY;
    x = o->oPosX;
    y = o->oPosY;
    z = o->oPosZ;
#define PUSHSPEED 3.f
    /*xwall -= (PUSHSPEED-1.f);
    zwall -= (PUSHSPEED-1.f);*/
    xwall -= 1.f;
    zwall -= 1.f;
    if ((gMarioState->wall) && (gMarioState->wall->object == o)) {
        if (m->flags & MARIO_UNKNOWN_31) {
            // play sound
            cur_obj_play_sound_1(SOUND_ENV_METAL_BOX_PUSH);
            moveVec = (gMarioState->faceAngle[1] + 0x2000) & 0xC000;
            o->oPosX += sins(moveVec) * PUSHSPEED;
            o->oPosZ += coss(moveVec) * PUSHSPEED;
            m->pos[0] += sins(moveVec) * PUSHSPEED;
            m->pos[2] += coss(moveVec) * PUSHSPEED;
            floorY = find_floor_height(x, y + 1500.f, z);
            if (absf(floorY - o->oPosY) > 10.f) {
                o->oPosX = x;
                o->oPosZ = z;
            }
            floorY = find_floor_height(o->oPosX + xwall, y + 1500.f, o->oPosZ + zwall);
            if (absf(floorY - o->oPosY) > 10.f) {
                o->oPosX = x;
                o->oPosZ = z;
            }
            floorY = find_floor_height(o->oPosX + xwall, y + 1500.f, o->oPosZ - zwall);
            if (absf(floorY - o->oPosY) > 10.f) {
                o->oPosX = x;
                o->oPosZ = z;
            }
            floorY = find_floor_height(o->oPosX - xwall, y + 1500.f, o->oPosZ + zwall);
            if (absf(floorY - o->oPosY) > 10.f) {
                o->oPosX = x;
                o->oPosZ = z;
            }
            floorY = find_floor_height(o->oPosX - xwall, y + 1500.f, o->oPosZ - zwall);
            if (absf(floorY - o->oPosY) > 10.f) {
                o->oPosX = x;
                o->oPosZ = z;
            }
        }
    }

    load_object_collision_model();
}

void pushthingy2(void) {

    pushbehavior(50.f, 216.f);
}

void pushthingy(void) {

    pushbehavior(200.f, 200.f);
}

extern Vtx ropebridge_Plane_031_mesh_layer_4_vtx_0[10];
extern const Collision ropebridge_collision[];
f32 vertSpeeds[10];
void ropebridge(void) {
    u8 i, k;
    u8 runCount = 1;
    f32 x, y;
    s16 *coll = segmented_to_virtual(ropebridge_collision);
    Vtx *vert = segmented_to_virtual(ropebridge_Plane_031_mesh_layer_4_vtx_0);
    //coll += 2;
    if (!o->oOpacity) {
        for (i = 0; i < 10; i++) {
            vertSpeeds[i] = 0;
            for (k = 0; k < 10; k++) {
                if (coll[k * 3 + 3] == vert[i].v.ob[0]) {
                    if (coll[k * 3 + 4] == vert[i].v.ob[1]) {
                        if (coll[k * 3 + 5] == vert[i].v.ob[2]) {
                            vert[i].v.flag = k;
                        }
                    }
                }
            }
        }
        o->oOpacity = 1;
    }
    if (cur_obj_is_mario_on_platform()) {
        for (i = 0; i < 10; i++) {
            if ((vert[i].v.ob[0] != 287) && (vert[i].v.ob[0] != -287)) {
                x = gMarioState->pos[0] - vert[i].v.ob[0] - o->oPosX;
                y = gMarioState->pos[2] - vert[i].v.ob[2] - o->oPosZ;
                x = sqrtf(x * x + y * y);
#define MAXIMPACT 200.f
#define MARIOIMPACT 2.f
                if (x < MAXIMPACT) {
                    vertSpeeds[i] -= MARIOIMPACT * (MAXIMPACT - x) / MAXIMPACT;
                }
            }
        }
    }
    if (cur_obj_is_mario_ground_pounding_platform()) {
        for (i = 0; i < 10; i++) {
            if ((vert[i].v.ob[0] != 287) && (vert[i].v.ob[0] != -287)) {
                x = gMarioState->pos[0] - vert[i].v.ob[0] - o->oPosX;
                y = gMarioState->pos[2] - vert[i].v.ob[2] - o->oPosZ;
                x = sqrtf(x * x + y * y);
                if (x < MAXIMPACT) {
                    vertSpeeds[i] -= 2.f * MARIOIMPACT * (MAXIMPACT - x) / MAXIMPACT;
                }
            }
        }
    }
#define RESISTANCE 0.93f
#define SNAPPINESS 0.02f
    if (!o->oTimer) {
        runCount = 255;
    }
    for (; runCount > 0; runCount--) {
        for (i = 0; i < 10; i++) {
            if ((vert[i].v.ob[0] != 287) && (vert[i].v.ob[0] != -287)) {
                vert[i].v.ob[1] += vertSpeeds[i];
                vertSpeeds[i] += (22.f - vert[i].v.ob[1])
                                 * (SNAPPINESS * ((sqrtf(absi(vert[i].v.ob[0]) + 250.f)) / 14.f));
                vertSpeeds[i] -= 1.f;
                vertSpeeds[i] *= RESISTANCE;
            }
        }
    }

    for (i = 0; i < 10; i++) {
        coll[vert[i].v.flag * 3 + 3] = vert[i].v.ob[0];
        coll[vert[i].v.flag * 3 + 4] = vert[i].v.ob[1];
        coll[vert[i].v.flag * 3 + 5] = vert[i].v.ob[2];
    }
    load_object_collision_model();
}

void crumblebridge(void) {
    u8 i, j;
    struct Object *coin;
    // spawn 5x4 grid 500x312.5f distance
    if (o->oBehParams2ndByte) {
        // spawn the thing
        if (!o->oAction) {
            for (i = 0; i < 5; i++) {
                for (j = 0; j < 4; j++) {
                    spawn_object_abs_with_rot(o, 0, 0x45, bhvCrumbleBridge,
#define XRANGE 312.5f
#define ZRANGE 500.f
                                              o->oPosX - XRANGE * 1.5f + j * XRANGE, o->oPosY,
                                              o->oPosZ + ZRANGE * 1.5f - ZRANGE * i, 0, 0, 0);
                }
            }
            o->oAction = 1;
        } else {
            if (!cur_obj_nearest_object_with_behavior(bhvCrumbleBridge)) {
                if (o->oOpacity < 15) {
                    if (!o->oOpacity) {
                        play_puzzle_jingle();
                    }
                    coin = spawn_object_relative(0, 0, 600.f, 0, gMarioState->marioObj,
                                                 MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
                    coin->oCoinUnk110 = -60.f;
                    o->oOpacity++;
                } else {
                    mark_obj_for_deletion(o);
                }
            }
        }
    } else {
        o->oRoom = 2;
        if (o->oAction) {
            if (o->oTimer > 3) {
                if (o->oAngleVelPitch < 0x400)
                    o->oAngleVelPitch += 0x80;
                if (o->oAngleVelRoll > -0x400 && o->oAngleVelRoll < 0x400)
                    o->oAngleVelRoll += o->oTumblingBridgeUnkF4; // acceleration?
                o->oGravity = -3.0f;
                cur_obj_rotate_face_angle_using_vel();
                cur_obj_move_using_fvel_and_gravity();
                if (o->oTimer > 80) {
                    obj_mark_for_deletion(o);
                }
            } else if (o->oTimer == 3) {
                cur_obj_play_sound_2(SOUND_GENERAL_PLATFORM);
            }
        } else {
            if (cur_obj_is_mario_on_platform()) {
                o->oAction = 1;
                o->oTumblingBridgeUnkF4 = random_sign() * 0x80;
            }
        }
        load_object_collision_model();
    }
}

u8 blockUp = 0;

extern u8 prevRoom;
extern u8 curRoom;
u32 checkFakeOut(f32 diffiCulty) {
    return ((random_u16() % 255) < diffiCulty);
}

void shyguysays(void) {
    f32 difficulty = (gMarioState->pos[0] + 4500.f) / 26900.f * 255.f;
    // print_text_fmt_int(10, 10, " %d", curRoom);
    switch (o->oAction) {
        case 0:
            // wait for mario to approach
            cur_obj_init_animation(0);
            if (curRoom == 5) {
                if (gMarioState->pos[0] > -4500.f) {
                    o->oAction = 1;
                }
            }
            break;
        case 1:
            // talk
            cur_obj_init_animation(0);
            if (talkToMario(107, 4)) {
                o->oAction = 2;
                o->oHealth = 0;
            }
            o->oOpacity = 50;
            break;
        case 2:
            // float in front of mario and put flags
            o->oVelX += (gMarioState->pos[0] + 1000.f - o->oPosX) / 100.f;
            o->oVelZ += (gMarioState->pos[2] - o->oPosZ) / 100.f;
            o->oVelX *= 0.97f;
            o->oVelZ *= 0.97f;
            #define MAXSPEED 60.f
            if (o->oVelX < -MAXSPEED){
                o->oVelX = -MAXSPEED;
            }
            else if (o->oVelX > MAXSPEED){
                o->oVelX = MAXSPEED;
            }
            if (o->oVelZ < -MAXSPEED){
                o->oVelZ = -MAXSPEED;
            }
            else if (o->oVelZ > MAXSPEED){
                o->oVelZ = MAXSPEED;
            }
            cur_obj_move_using_vel();
            if (o->oOpacity) {
                o->oOpacity--;
            }
            switch (o->oSubAction) {
                case 0:
                    cur_obj_init_animation(0);
                    if (!o->oOpacity) {
                        o->oSubAction = (random_u16() & 1) + 1;
                        o->oHealth = 0;
                    }
                    break;
                case 1:
                    cur_obj_init_animation(1);
                    if (o->header.gfx.unk38.animFrame == 18) {
                        if (checkFakeOut(difficulty - o->oHealth)) {
                            o->oSubAction = 3;
                        }
                    }
                    if (o->header.gfx.unk38.animFrame == 20) {
                        blockUp = 0;
                    }
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oSubAction = 0;
                        o->oOpacity = random_u16() % 40 + 40 - difficulty / 10.f;
                    }
                    break;
                case 2:
                    cur_obj_init_animation(2);
                    if (o->header.gfx.unk38.animFrame == 18) {
                        if (checkFakeOut(difficulty - o->oHealth)) {
                            o->oSubAction = 4;
                        }
                    }
                    if (o->header.gfx.unk38.animFrame == 20) {
                        blockUp = 1;
                    }
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oSubAction = 0;
                        o->oOpacity = random_u16() % 40 + 40 - difficulty / 10.f;
                    }
                    break;
                case 3:
                    o->header.gfx.unk38.animFrame -= 2;
                    if (o->header.gfx.unk38.animFrame < 3) {
                        o->oSubAction = 2;
                        o->oHealth += 20;
                    }
                    break;
                case 4:
                    o->header.gfx.unk38.animFrame -= 2;
                    if (o->header.gfx.unk38.animFrame < 3) {
                        o->oSubAction = 1;
                        o->oHealth += 20;
                    }
                    break;
            }
            if (gMarioState->pos[0] > 20150.f) {
                o->oAction = 3;
            }
            if (curRoom != 5) {
                o->oVelX = 0;
                o->oVelZ = 0;
                o->oPosX = o->oHomeX;
                o->oPosZ = o->oHomeZ;
                o->oAction = 0;
            }
            break;
        case 3:
            cur_obj_init_animation(0);
            o->oPosX = approach_f32_asymptotic(o->oPosX, 21100.f, 0.05f);
            o->oPosZ = approach_f32_asymptotic(o->oPosZ, -4000.f, 0.05f);
            // wait at end
            if (curRoom == 5) {
                if (gMarioState->pos[0] > 21400.f) {
                    o->oAction = 4;
                }
            }
            break;
        case 4:
            // talk at end
            if (talkToMario(108, 4)) {
                o->oAction = 5;
            }
            break;
        case 5:
            // idle at end
            break;
    }
    o->oFaceAngleYaw = o->oAngleToMario;
}

void shyguyblock(void) {
    o->oAction = (blockUp == o->oBehParams2ndByte);
#define SWITCHTIMER 24
    if (!o->oAction) {
        if (o->oTimer == SWITCHTIMER) {
            o->oAnimState = 1 + o->oBehParams2ndByte * 2;
        }
    } else {
        o->oAnimState = o->oBehParams2ndByte * 2;
    }
    if (!(o->oAnimState & 1)) {
        load_object_collision_model();
    }
}

void shyguybed(void) {
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_on_platform()) {
                o->oAction = 1;
                o->oVelY = -20.f;
            }
            if (cur_obj_is_mario_ground_pounding_platform()) {
                o->oAction = 1;
                o->oVelY = -35.f;
            }
            break;
        case 1:
            if (o->oVelY > 0.f) {
                if (o->header.gfx.scale[1] < 1.f) {
                    set_mario_action(m, ACT_TRIPLE_JUMP, 0);
                }
            }
            if (!cur_obj_is_mario_on_platform()) {
                o->oAction = 0;
                    set_mario_action(m, ACT_TRIPLE_JUMP, 0);
            }
            break;
        case 2:

            break;
        case 3:

            break;
        case 4:

            break;
    }
    o->header.gfx.scale[1] -= o->oVelY / 40.f;
    if (o->header.gfx.scale[1] < 0.f) {
        o->header.gfx.scale[1] = 0.f;
        o->oVelY = o->oVelY * -0.5f;
    }
    o->oVelY += (o->header.gfx.scale[1] - 1.f) * 3.5f;
    o->oVelY *= 0.93f;
    load_object_collision_model();
}

// spawn dust and sound too
#define INCREMENT 0.0035f
extern f32 camRoll;
extern f32 camRollSpeed;
#define PERCENTAGE 0.001f

#define DUSTSPAWN 80.f
#define /*0x108*/ oMacroUnk108 OBJECT_FIELD_F32(0x20)
void movecratecode(void) {
    for (; o->oOpacity < 0x400; o->oOpacity++) {
        o->oMacroUnk10C = DUSTSPAWN / 2.f;
        camRoll += camRollSpeed;
        camRollSpeed -= camRoll * PERCENTAGE;
        o->oVelZ += camRoll * INCREMENT * 127.f / o->oBehParams2ndByte;
        o->oVelZ *= 0.99f;
        o->oPosZ += o->oVelZ;
        if (o->oPosZ > (o->oHomeZ + ((o->oBehParams >> 24) * 100.f))) {
            o->oPosZ = (o->oHomeZ + ((o->oBehParams >> 24) * 100.f));
            o->oVelZ = 0.f;
        }
        if (o->oPosZ < (o->oHomeZ - ((o->oBehParams >> 24) * 100.f))) {
            o->oPosZ = (o->oHomeZ - ((o->oBehParams >> 24) * 100.f));
            o->oVelZ = 0.f;
        }
    }
    o->oVelZ += camRoll * INCREMENT * 127.f / o->oBehParams2ndByte;
    o->oVelZ *= 0.99f;
    o->oPosZ += o->oVelZ;
    if (o->oPosZ > (o->oHomeZ + ((o->oBehParams >> 24) * 100.f))) {
        o->oPosZ = (o->oHomeZ + ((o->oBehParams >> 24) * 100.f));
        o->oVelZ = 0.f;
    }
    if (o->oPosZ < (o->oHomeZ - ((o->oBehParams >> 24) * 100.f))) {
        o->oPosZ = (o->oHomeZ - ((o->oBehParams >> 24) * 100.f));
        o->oVelZ = 0.f;
    }
    if (absf(o->oVelZ) > 10.f) {
        cur_obj_play_sound_1(SOUND_AIR_ROUGH_SLIDE);
    }
    o->oMacroUnk108 += o->oVelZ;
    o->oMacroUnk10C += o->oVelZ;
    if (o->oMacroUnk108 < -DUSTSPAWN) {
        o->oMacroUnk108 += DUSTSPAWN;
        spawn_object_relative(0, 200.f, 0, 200.f, o, 0, bhvMistParticleSpawner);
    }
    if (o->oMacroUnk108 > DUSTSPAWN) {
        o->oMacroUnk108 -= DUSTSPAWN;
        spawn_object_relative(0, -200.f, 0, 200.f, o, 0, bhvMistParticleSpawner);
    }

    if (o->oMacroUnk10C < -DUSTSPAWN) {
        o->oMacroUnk10C += DUSTSPAWN;
        spawn_object_relative(0, 200.f, 0, -200.f, o, 0, bhvMistParticleSpawner);
    }
    if (o->oMacroUnk10C > DUSTSPAWN) {
        o->oMacroUnk10C -= DUSTSPAWN;
        spawn_object_relative(0, -200.f, 0, -200.f, o, 0, bhvMistParticleSpawner);
    }

    load_object_collision_model();
}

u16 cloakTimer = 0;
extern Vtx captainVerts[78];
extern Vtx shyguycaptain_BODY_mesh_layer_5_vtx_0[78];
void animateCloak() {
    Vtx *mod = segmented_to_virtual(shyguycaptain_BODY_mesh_layer_5_vtx_0);
    Vtx *read = segmented_to_virtual(captainVerts);
    u32 i;
    u16 dir;
    cloakTimer += 0x800;
    for (i = 0; i < 78; i++) {
        if (!mod[i].v.cn[3]) {
            dir = atan2s(read[i].v.ob[2], read[i].v.ob[0]);
#define MINVERT 75.f
            mod[i].v.ob[0] = read[i].v.ob[0]
                             + sins(dir) * (MINVERT - read[i].v.ob[1]) * 0.1f
                                   * sins(cloakTimer + read[i].v.ob[0] * 256 + read[i].v.ob[2] * 128);
            mod[i].v.ob[2] = read[i].v.ob[2]
                             + coss(dir) * (MINVERT - read[i].v.ob[1]) * 0.1f
                                   * sins(cloakTimer + read[i].v.ob[0] * 256 + read[i].v.ob[2] * 128);
        }
    }
}
// play keyboard
// talk to mario
// space idly
// sping attack
// flame attack
// get hit->piano dash
// come out of piano
//
void shyguymovement() {
    u16 moveDir, targetAngle;
    f32 curSpeed;
    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oVelX += sins(targetAngle) * 2.f;
    o->oVelZ += coss(targetAngle) * 2.f;

    o->oForwardVel = sqrtf(o->oVelX * o->oVelX + o->oVelZ * o->oVelZ);
    moveDir = atan2s(o->oVelZ, o->oVelX);
    o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, o->oAngleToMario, 0x800);
    /*o->oForwardVel = approach_f32(o->oForwardVel,
                                  30.f * (coss(moveDir - o->oMoveAngleYaw) + 1.0f), 2.f, 3.f);*/
    o->oForwardVel *= 0.995f;
    o->oVelX = sins(moveDir) * o->oForwardVel;
    o->oVelZ = coss(moveDir) * o->oForwardVel;
    o->oPosY = approach_f32_symmetric(o->oPosY, o->oBobombBuddyPosYCopy, 11.f);
    o->oVelY = approach_f32_symmetric(o->oVelY, 0, 1.5f);
    cur_obj_move_using_vel();
}
void muteCptInstruments() {
    gSequencePlayers[SEQ_PLAYER_ENV].channels[0]->volume = 0;
    gSequencePlayers[SEQ_PLAYER_ENV].channels[2]->volume = 0;
    gSequencePlayers[SEQ_PLAYER_ENV].channels[4]->volume = 0;
    gSequencePlayers[SEQ_PLAYER_ENV].channels[5]->volume = 0;
    gSequencePlayers[SEQ_PLAYER_ENV].channels[7]->volume = 0;
    gSequencePlayers[SEQ_PLAYER_ENV].channels[9]->volume = 0;
}
extern struct CutsceneJump *currentScene;
void shyguycpt(void) {
    struct Object *flame;
    f32 x, y, z;
    if (o->oDistanceToMario < 4000.f) {
        animateCloak();
    }
    if (!o->oHiddenBlueCoinSwitch) {
        o->oHiddenBlueCoinSwitch =
            spawn_object_relative(0, 0, 0, 295.f, o, MODEL_MAD_PIANO, bhvMadPiano);
        o->oHiddenBlueCoinSwitch->oMoveAngleYaw += 0x8000;
        o->oPosY += 65.f;
    }
    o->oIntangibleTimer = 0;
    cur_obj_update_floor_height();
    switch (o->oAction) {
        case 0:
            // sit in front of piano, play it
            switch (o->oSubAction) {
                case 0:
                    if ((o->oDistanceToMario < 3500.f) && (gMarioState->pos[1] > o->oPosY - 1000.f)) {
                        play_secondary_music(SEQ_EVENT_SHYGUYBOSS, 0, 255, 1000);
                        muteCptInstruments();
                        o->oSubAction = 1;
                        // keep channel 3, delete channel 0, 2, 4, 5, 7, 9
                    }
                    break;
                case 1:
                    muteCptInstruments();
                    if (o->oDistanceToMario > 4000.f) {
                        func_80321080(50);
                        o->oSubAction = 0;
                    }
                    break;
            }
            if ((o->oDistanceToMario < 1000.f) && !currentScene) {
                o->oAction = 1;
                func_80321080(50);
            }
            cur_obj_init_animation(0);
            break;
        case 1:
            // turn around, talk to mario
            if (o->oTimer > 20) {
                cur_obj_init_animation(1);
            } else {
                o->header.gfx.unk38.animFrame = 0;
            }
            if (o->oTimer > 40) {
                o->oFlags |= OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;
                cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), 0x240);
            } else {
                o->oMoveAngleYaw = o->oFaceAngleYaw;
            }
            if (o->oTimer < 70) {
                o->oDialogState = 0;
            }
            if (o->header.gfx.unk38.animFrame > 98) {
                o->header.gfx.unk38.animFrame -= 50;
            }
            if (talkToMarioNoRotation(111, 4)) {
                o->oAction = 2;
                o->oFlags &= ~OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;
                o->oOpacity = 1;
                o->oHealth = 0;
                sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
                play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_EVENT_SHYGUYBOSS), 0);
            }
            break;
        case 2:
            // fight idle, space in front of mario
            cur_obj_init_animation(2);
            if (o->header.gfx.unk38.animFrame > 98) {
                o->header.gfx.unk38.animFrame -= 50;
            }
            o->oBobombBuddyPosXCopy = sins(o->oAngleToMario + 0x8000) * 1200.f + gMarioState->pos[0];
            o->oBobombBuddyPosYCopy = o->oFloorHeight + 100.f;
            o->oBobombBuddyPosZCopy = coss(o->oAngleToMario + 0x8000) * 1200.f + gMarioState->pos[2];
            if (o->oBobombBuddyPosYCopy < o->oHomeY + 100.f) {
                o->oBobombBuddyPosYCopy = o->oHomeY + 600.f;
            }
            if (random_u16() > (0x10000 - (o->oTimer * 4 + o->oHealth * 0x20))) {
                if (o->oOpacity & 1) {
                    o->oAction = 4;
                    // SOUNDFI
                } else {
                    o->oAction = 5;
                }
            }
            shyguymovement();
            break;
        case 4:
            // swing sword, start rotating at the end of the animation
            switch (o->oSubAction) {
                case 0:
                    cur_obj_init_animation(3);
                    if (o->header.gfx.unk38.animFrame > 31) {
                        o->oMoveAngleYaw = o->oAngleToMario;
#define SHYGUYSPINSPEED 65.f
                        o->oVelX = SHYGUYSPINSPEED * sins(o->oMoveAngleYaw);
                        o->oVelZ = SHYGUYSPINSPEED * coss(o->oMoveAngleYaw);
                        o->oTimer = 0;
                        o->oSubAction++;
                        o->oObjPointer2 = spawn_object(o, 0, bhvHitbox);
                        o->oObjPointer2->hitboxHeight = 20.f;
                        o->oObjPointer2->hitboxRadius = 250.f;
                        o->oObjPointer2->oPosY += 60.f;
                        o->oObjPointer2->oDamageOrCoinValue = 2;
                        cur_obj_play_sound_2(SOUND_OBJ_MRI_SHOOT);
                    } else {
                        // shyguymovement();
                        o->oPosX = approach_f32_asymptotic(o->oPosX, o->oBobombBuddyPosXCopy, 0.1f);
                        o->oPosY = approach_f32_asymptotic(o->oPosY, o->oBobombBuddyPosYCopy, 0.1f);
                        o->oPosZ = approach_f32_asymptotic(o->oPosZ, o->oBobombBuddyPosZCopy, 0.1f);
                        if (o->oTimer == 10) {

                            cur_obj_play_sound_2(SOUND_OBJ_UNKNOWN2);
                        }
                    }
                    break;
                case 1:
                    if (!(o->oTimer % 12)) {

                        cur_obj_play_sound_2(SOUND_ACTION_SPIN);
                    }
                    obj_copy_pos_and_angle(o->oObjPointer2, o);
                    o->oObjPointer2->hitboxHeight = 20.f;
                    o->oObjPointer2->hitboxRadius = 250.f;
                    o->oObjPointer2->oPosY += 60.f;
                    o->oObjPointer2->oDamageOrCoinValue = 2;
#define TIMEFORATTACK 70
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oAngleVelYaw = 0x1200;
                    }
                    if (o->oTimer < TIMEFORATTACK / 2) {
                        o->oPosY = approach_f32_asymptotic(o->oPosY, gMarioState->pos[1], 0.05f);
                    } else {
                        o->oVelY += 0.5f;
                    }
                    if (o->oTimer > TIMEFORATTACK - 10) {
                        o->oVelX *= 0.9f;
                        o->oVelY *= 0.9f;
                        o->oVelZ *= 0.9f;
                    }
                    if (o->oTimer > TIMEFORATTACK) {
                        o->oSubAction++;
                        mark_obj_for_deletion(o->oObjPointer2);
                        o->oObjPointer2 = 0;
                    }
                    cur_obj_move_using_vel();
                    break;
                case 2:
                    o->oAngleVelYaw = approach_s16_symmetric(o->oAngleVelYaw, 0, 0x120);
                    if (!o->oAngleVelYaw) {
                        o->oSubAction++;
                    }
                    cur_obj_move_using_vel();
                    break;
                case 3:
                    cur_obj_move_using_vel();
                    cur_obj_init_animation(2);
                    o->header.gfx.unk38.animFrame = 19;
                    o->oAction = 2;
                    o->oOpacity = 0;
                    break;
            }
            break;
        case 5:
            // swing sword a few times, spawn flame at the tip of his sword
            o->oMoveAngleYaw = o->oAngleToMario;
            o->oFaceAngleYaw = o->oMoveAngleYaw;
            switch (o->oSubAction) {
                case 0:
                    o->oBobombBuddyPosXCopy =
                        sins(o->oAngleToMario + 0x8000) * 750.f + gMarioState->pos[0];
                    o->oBobombBuddyPosYCopy = o->oFloorHeight + 300.f;
                    o->oBobombBuddyPosZCopy =
                        coss(o->oAngleToMario + 0x8000) * 750.f + gMarioState->pos[2];
                    if (o->oBobombBuddyPosYCopy < o->oHomeY + 100.f) {
                        o->oBobombBuddyPosYCopy = o->oHomeY + 600.f;
                    }
                    o->oPosX = approach_f32_asymptotic(o->oPosX, o->oBobombBuddyPosXCopy, 0.15f);
                    o->oPosY = approach_f32_asymptotic(o->oPosY, o->oBobombBuddyPosYCopy, 0.15f);
                    o->oPosZ = approach_f32_asymptotic(o->oPosZ, o->oBobombBuddyPosZCopy, 0.15f);
                    x = o->oPosX - o->oBobombBuddyPosXCopy;
                    y = o->oPosY - o->oBobombBuddyPosYCopy;
                    z = o->oPosZ - o->oBobombBuddyPosZCopy;
                    if (sqrtf(x * x + y * y + z * z) < 250.f) {
                        o->oSubAction++;
                    }
                    break;
                case 1:
                    cur_obj_init_animation(4);
                    if (o->header.gfx.unk38.animFrame > 19) {
                        o->oSubAction++;
                        o->oBobombBuddyCannonStatus = 0;
                    }
                    break;
                case 2:
                    if ((o->header.gfx.unk38.animFrame == 54)) {
#define YOFF 140.f
#define ZOFF 200.f
                        flame = spawn_object_relative(0, 0, YOFF, ZOFF, o, MODEL_BLUE_FLAME,
                                                      bhvBouncingFireballFlame);
                        obj_scale_xyz(flame, 5.f, 5.f, 5.f);
                        cur_obj_play_sound_2(SOUND_OBJ_FLAME_BLOWN);
                    }
                    if ((o->header.gfx.unk38.animFrame == 29)) {
#define YOFF 140.f
#define ZOFF 200.f
                        flame = spawn_object_relative(1, 0, YOFF, ZOFF, o, MODEL_BLUE_FLAME,
                                                      bhvSmallPiranhaFlame);
                        flame->oSmallPiranhaFlameUnkF4 = 50.f;
                        flame->oSmallPiranhaFlameUnkF8 = 30.f;
                        flame->oSmallPiranhaFlameUnkFC = MODEL_BLUE_FLAME;
                        flame->oMoveAnglePitch =
                            0x4000
                            - lateral_dist_between_objects(gMarioState->marioObj, o)
                                  * 6.f; // calc from lateral dist to mario
                        obj_scale_xyz(flame, 5.f, 5.f, 5.f);
                        cur_obj_play_sound_2(SOUND_OBJ_FLAME_BLOWN);
                    }
                    if (o->header.gfx.unk38.animFrame > 78) {
                        if ((4 - o->oHealth) < o->oBobombBuddyCannonStatus) {
                            o->oSubAction++;
                        } else {
                            o->header.gfx.unk38.animFrame -= 59;
                            o->oBobombBuddyCannonStatus++;
                        }
                    }
                    break;
                case 3:
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oOpacity++;
                        cur_obj_init_animation(2);
                        o->header.gfx.unk38.animFrame = 19;
                        o->oAction = 2;
                    }
                    break;
            }
            break;
        case 6:
            // dash into piano
            o->oIntangibleTimer = -1;
            switch (o->oSubAction) {
                case 0:
                    o->oVelY = 0;
                    cur_obj_init_animation(5);
                    if (!o->oTimer) {
                        o->oForwardVel = -50.f;
                        o->oAngleVelPitch = 0x1370;
                        o->oAngleVelYaw = -0x10C0;
                    }
                    o->oFaceAngleYaw += o->oAngleVelYaw;
                    o->oFaceAnglePitch += o->oAngleVelPitch;
                    o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 0, 0x137 / 2);
                    o->oAngleVelYaw = approach_s16_symmetric(o->oAngleVelYaw, 0, 0x10C / 2);
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.f);
                    if (o->oTimer > 32) {
                        o->oSubAction++;
                        o->oAngleVelYaw = 0;
                        o->oAngleVelPitch = 0;
                        o->oForwardVel = 0;
                        o->oHealth++;
                        if (o->oHealth > 2) {
                            o->oAction = 8;
                        }
                    }
                    o->oPosY = approach_f32_symmetric(o->oPosY, gMarioState->pos[1] + 100.f, 3.f);
                    break;
                case 1:
                    o->oFaceAngleYaw =
                        approach_s16_symmetric(o->oFaceAngleYaw, o->oAngleToMario, 0x800);
                    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0x800);
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oSubAction++;
                    }
                    break;
                case 2:
                    o->oMoveAngleYaw = approach_s16_symmetric(
                        o->oMoveAngleYaw, obj_angle_to_object(o, o->oHiddenBlueCoinSwitch), 0x800);
                    if (o->oFaceAngleYaw == o->oMoveAngleYaw) {
                        o->oSubAction++;
                        o->oForwardVel = 10.f;
                        cur_obj_init_animation(6);
                    }
                    o->oFaceAngleYaw = o->oMoveAngleYaw;
                    break;
                case 3:
                    o->oMoveAngleYaw = obj_angle_to_object(o, o->oHiddenBlueCoinSwitch);
                    o->oForwardVel = approach_f32_symmetric(
                        o->oForwardVel, lateral_dist_between_objects(o, o->oHiddenBlueCoinSwitch) / 8.f,
                        5.f);
                    o->oPosY = approach_f32_asymptotic(o->oPosY,
                                                       o->oHiddenBlueCoinSwitch->oPosY + 350.f, 0.1f);
                    if (lateral_dist_between_objects(o, o->oHiddenBlueCoinSwitch) < 40.f) {
                        o->oSubAction++;
                        o->oForwardVel = 0;
                        o->oTimer = 0;
                    }
                    break;
                case 4:
                    o->oHiddenBlueCoinSwitch->oAction = 1;
                    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0x4000, 0xC00);
                    o->oVelY -= 4.f;
                    cur_obj_scale(1.f - 0.08 * o->oTimer);
                    if (o->oTimer > 11) {
                        o->oAction = 7;
                        cur_obj_set_model(0);
                        o->oHiddenBlueCoinSwitch->oAction = 2;
                    }
                    break;
            }
        obj_move_xyz_using_fvel_and_yaw();
            break;
        case 7:
            o->oIntangibleTimer = -1;
            // angry piano attack mario, chases for 10 + 5*damage seconds
            switch (o->oSubAction) {
                case 0:
                    o->oHiddenBlueCoinSwitch->oBehParams2ndByte = o->oHealth;
                    if (o->oTimer > o->oHealth * 150 + 300) {
                        cur_obj_set_model(0x4B);
                        o->oHiddenBlueCoinSwitch->oAction = 0;
                        o->oVelY = 14.f;
                        o->oFaceAnglePitch = 0xC000;
                        o->oTimer = 0;
                        cur_obj_init_animation(2);
                        o->header.gfx.unk38.animFrame = 19;
                        o->oSubAction++;
                        obj_copy_pos_and_angle(o, o->oHiddenBlueCoinSwitch);
                        o->oPosY += 50.f;
                    }
                    break;
                case 1:
                    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0xC00);
                    o->oVelY -= 2.f;
                    o->oPosY += o->oVelY;
                    cur_obj_scale(0.08 * o->oTimer);
                    if (o->oTimer > 11) {
                        o->oAction = 2;
                        cur_obj_scale(1.f);
                    }
                    o->oInteractStatus = 0;
                    break;
            }
            break;
        case 8:
            // die and give mario star
            if (!o->oTimer) {
                cur_obj_set_model(0);
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
            } else {
                stop_background_music(SEQUENCE_ARGS(4, SEQ_EVENT_SHYGUYBOSS));
                spawn_default_star(o->oHomeX, o->oHomeY + 200.f, o->oHomeZ);
                mark_obj_for_deletion(o);
            }
            break;
    }
    if (o->oAction < 6) {
        if (o->oInteractStatus & INT_STATUS_INTERACTED) {
            if (o->oInteractStatus & INT_STATUS_WAS_ATTACKED) {
                cur_obj_play_sound_2(SOUND_OBJ_KOOPA_FLYGUY_DEATH);
                o->oAction = 6;
                o->oMoveAngleYaw = o->oAngleToMario;
                if (o->oObjPointer2) {
                    mark_obj_for_deletion(o->oObjPointer2);
                    o->oObjPointer2 = 0;
                    o->oOpacity++;
                }
            }
        }
        o->oInteractStatus = 0;
    }
    if ((gMarioState->pos[1] < 1000.f) && !currentScene) {
        o->oAction = 0;
        o->oPosX = o->oHomeX;
        o->oPosY = o->oHomeY;
        o->oPosZ = o->oHomeZ;
        o->oFaceAngleYaw = 0x8000;
        o->oMoveAngleYaw = 0x8000;
        mark_obj_for_deletion(o->oHiddenBlueCoinSwitch);
        o->oHiddenBlueCoinSwitch = 0;
        stop_background_music(SEQUENCE_ARGS(4, SEQ_EVENT_SHYGUYBOSS));
    }
    o->oFaceAngleYaw += o->oAngleVelYaw;
}

void cptPiano(void) {
    f32 y;
    o->oIntangibleTimer = 0;
    o->oInteractStatus = 0;
    switch (o->oAction) {
        case 0:
            o->oForwardVel = 0.f;
            o->oInteractType = INTERACT_IGLOO_BARRIER;
            if (cur_obj_check_if_near_animation_end()) {
                cur_obj_init_animation(0);
            }
            break;
        case 1:
            cur_obj_init_animation(1);
            break;
        case 2:
            o->oInteractType = INTERACT_MR_BLIZZARD;
            cur_obj_init_animation_with_sound(1);
            cur_obj_play_sound_at_anim_range(0, 0, SOUND_OBJ_MAD_PIANO_CHOMPING);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x800);

            o->oForwardVel =
                approach_f32_symmetric(o->oForwardVel,
                                       (10.f + o->oBehParams2ndByte * 1.5f)
                                           * (coss(o->oAngleToMario - o->oMoveAngleYaw) + 1.0f),
                                       1.f);
            break;
    }
    cur_obj_update_floor_and_walls();
    y = o->oPosY;
    cur_obj_move_standard(78);
    if (o->oFloorHeight < 1200.f) {
        o->oPosY = y;
        o->oVelY = 0;
    }
}
extern s32 cur_obj_wait_then_blink2(s32 timeUntilBlinking, s32 numBlinks);
extern const Collision fallplatform1_collision[];
extern const Collision fallplatform2_collision[];
void fallplatform(void) {
    u32 *collisionData;
    if (o->oBehParams2ndByte) {
        collisionData = segmented_to_virtual(fallplatform2_collision);
    } else {
        collisionData = segmented_to_virtual(fallplatform1_collision);
    }
    o->collisionData = collisionData;
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_on_platform()) {
                o->oAction++;
                cur_obj_play_sound_2(SOUND_OBJ_UNKNOWN2);
                // cur_obj_shake_screen(SHAKE_POS_SMALL);
            }
            load_object_collision_model();
            break;
        case 1:
            o->oGraphYOffset = (o->oTimer & 1) * -8.f;
            if (o->oTimer > 32) {
                o->oAction++;
                o->oGraphYOffset = 0;
            }
            load_object_collision_model();
            break;
        case 2:
            o->oVelY -= 1.f;
            if (o->oVelY < -75.f) {
                o->oVelY = -75.f;
            }
            if (o->oPosY < -1500.f) {
                o->oAction++;
            }
            load_object_collision_model();
            break;
        case 3:
            if (cur_obj_wait_then_blink(0, 10)) {
                o->oAction = 0;
                o->oPosX = o->oHomeX;
                o->oPosY = o->oHomeY;
                o->oPosZ = o->oHomeZ;
                o->oVelY = 0;
                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
            }
            load_object_collision_model();
            break;
    }
    o->oPosY += o->oVelY;
}

extern const Trajectory bbh_area_4_spline_LinePlatformPath1[];
extern const Trajectory bbh_area_4_spline_LinePlatformPath2[];
extern const Trajectory bbh_area_4_spline_LinePlatformPath3[];

Trajectory *trajectoryList[3] = { &bbh_area_4_spline_LinePlatformPath1,
                                  &bbh_area_4_spline_LinePlatformPath2,
                                  &bbh_area_4_spline_LinePlatformPath3 };

void lineplatform(void) {
    s16 *curPath = segmented_to_virtual(trajectoryList[o->oBehParams2ndByte]);
    f32 x, z, y;
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_on_platform()) {
                o->oAction++;
                o->oBobombBuddyRole = o->oMoveAngleYaw;
                o->oOpacity += 4;
            }
            break;
        case 1:
            cur_obj_play_sound_1(SOUND_ENV_ELEVATOR1);
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 45.f, 2.f);
            x = curPath[1 + o->oOpacity] - o->oPosX;
            y = curPath[2 + o->oOpacity] - o->oPosY;
            z = curPath[3 + o->oOpacity] - o->oPosZ;
            o->oMoveAngleYaw = atan2s(z, x);
            if (sqrtf(z * z + x * x) < o->oForwardVel + 5.f) {
                o->oFaceAngleYaw = atan2s(z, x);
            }
            if (!o->oTimer) {
                o->oFaceAngleYaw = o->oMoveAngleYaw;
            }
            o->oMoveAnglePitch = -atan2s(sqrtf(z * z + x * x), y);
            if (sqrtf(x * x + y * y + z * z) < o->oForwardVel) {
                o->oOpacity += 4;
            }
            if (curPath[o->oOpacity] == -1) {
                o->oAction++;
            }
            break;
        case 2:
            o->oVelY -= 2.f;
            if (o->oVelY < -75.f) {
                o->oVelY = -75.f;
            }
            if (o->oTimer > 400) {
                o->oAction = 4;
            }
            if (o->oTimer > 20.f) {
                if (absf(o->oPosY - curPath[2 + o->oOpacity - 4]) < 75.f) {
                    o->oAction++;
                    o->oForwardVel = -o->oVelY;
                    goto keepGOING;
                }
            }
            break;
        case 3:
        keepGOING:
            cur_obj_play_sound_1(SOUND_ENV_ELEVATOR1);
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 45.f, 2.f);
            x = curPath[1 + o->oOpacity - 4] - o->oPosX;
            y = curPath[2 + o->oOpacity - 4] - o->oPosY;
            z = curPath[3 + o->oOpacity - 4] - o->oPosZ;
            o->oMoveAngleYaw = atan2s(z, x);
            if (sqrtf(z * z + x * x) < o->oForwardVel + 5.f) {
                o->oFaceAngleYaw = atan2s(z, x);
            }
            if (!o->oTimer) {
                o->oFaceAngleYaw = o->oMoveAngleYaw;
            }
            o->oMoveAnglePitch = -atan2s(sqrtf(z * z + x * x), y);
            if (sqrtf(x * x + y * y + z * z) < o->oForwardVel) {
                o->oOpacity -= 4;
            }
            if (o->oOpacity == 0) {
                o->oAction++;
            }
            break;
        case 4:
            o->oForwardVel = 0;
            o->oMoveAngleYaw = o->oBobombBuddyRole;
            o->oFaceAngleYaw = o->oMoveAngleYaw;
            o->oMoveAnglePitch = 0;
            o->oAction = 0;
            o->oPosX = o->oHomeX;
            o->oPosY = o->oHomeY;
            o->oPosZ = o->oHomeZ;
            o->oOpacity = 0;
            break;
    }
    o->oAngleVelYaw = (s16) (o->oMoveAngleYaw - o->oFaceAngleYaw);
    o->oFaceAngleYaw = o->oMoveAngleYaw;
    if (o->oAction != 2) {
        o->oVelY = sins(-o->oMoveAnglePitch) * o->oForwardVel;
        o->oVelX = coss(-o->oMoveAnglePitch) * sins(o->oMoveAngleYaw) * o->oForwardVel;
        o->oVelZ = coss(-o->oMoveAnglePitch) * coss(o->oMoveAngleYaw) * o->oForwardVel;
    }
    cur_obj_move_using_vel();
    load_object_collision_model();
    load_object_collision_model();
}

s16 maxVolumes[4][3] = {
    { -10231, 1950, -2144 }, { -11581, 1950, -1249 }, { -12931, 1950, -1249 }, { -14281, 1950, -1249 }
};
extern void func_8031D838(s32 player, s32 fadeInTime, u8 targetVolume);
f32 fadeVolToPos(s16 *pos) {
    f32 dist;
    f32 x, y, z;
    if (!o->oAction) {
        play_secondary_music(SEQ_EVENT_SHYGUYBOSS, 20, 255, 1000);
        o->oAction = 1;
    }
    x = gMarioState->pos[0] - pos[0];
    z = gMarioState->pos[2] - pos[2];
    dist = sqrtf(x * x + z * z);
    y = (3500.f - dist) / 5000.f;
    func_8031D838(SEQ_PLAYER_ENV, 20, (y * y * y) * 255);
    muteCptInstruments();
}
void cptlabyrinth(void) {
    switch (curRoom) {
        case 6:
            fadeVolToPos(&maxVolumes[0][0]);
            break;
        case 7:
            fadeVolToPos(&maxVolumes[1][0]);
            break;
        case 8:
            fadeVolToPos(&maxVolumes[2][0]);
            break;
        case 9:
            fadeVolToPos(&maxVolumes[3][0]);
            break;
        default:
            if (o->oAction) {
                func_80321080(50);
                o->oAction = 0;
            }
            break;
    }
}