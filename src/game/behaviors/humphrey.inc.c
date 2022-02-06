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

#define m gMarioState
#define o gCurrentObject
extern u8 openDoors;

#define opacity o->oOpacity
// animation swim happy
// animation suck mario in
void humphrey(void) {
    f32 x, z, y;
    f32 suckspeed;
    f32 total;
    x = ((s16 *) segmented_to_virtual(wf_area_1_spline_turtlepath))[1 + opacity];
    z = ((s16 *) segmented_to_virtual(wf_area_1_spline_turtlepath))[3 + opacity];
    y = ((s16 *) segmented_to_virtual(wf_area_1_spline_turtlepath))[2 + opacity];
    o->oMoveAngleYaw =
        approach_s16_symmetric(o->oMoveAngleYaw, atan2s(z - o->oPosZ, x - o->oPosX), 0x0040);
    if (sqrtf((x - o->oPosX) * (x - o->oPosX) + (z - o->oPosZ) * (z - o->oPosZ)) < 700.f) {
        opacity += 4;
        if (opacity == 28) {
            opacity = 0;
        }
    }
    if (o->oTimer > 50) {
        if (absf(o->oPosY - y) > 10.f) {
            o->oVelY += (y - o->oPosY) / absf(o->oPosY - y) * 2.f;
        } else {
            o->oVelY *= 0.97f;
        }
        if (o->oVelY > 30.f) {
            o->oVelY = 30.f;
        }
        if (o->oVelY < -30.f) {
            o->oVelY = -30.f;
        }
    }
    o->oHomeX = o->oPosX + sins(o->oMoveAngleYaw) * 1612.f;
    o->oHomeZ = o->oPosZ + coss(o->oMoveAngleYaw) * 1612.f;
    o->oHomeY = o->oPosY + 208.f;
#define SUCKYRANGE 2500.f

    if ((cur_obj_lateral_dist_from_mario_to_home() < SUCKYRANGE)
        && (absf(o->oPosY - gMarioState->pos[1]) < 1000.f)) {
        if (o->oTimer > 200) {
            suckspeed = (SUCKYRANGE - cur_obj_lateral_dist_from_mario_to_home()) / SUCKYRANGE * 100.f;
            x = o->oHomeX - gMarioState->pos[0];
            y = o->oHomeY - gMarioState->pos[1];
            z = o->oHomeZ - gMarioState->pos[2];
            total = sqrtf(x * x + y * y + z * z);
            if (total == 0) {
                total = 1;
            }
            x = x / total * suckspeed;
            y = y / total * suckspeed;
            z = z / total * suckspeed;
            gMarioState->pos[0] += x;
            gMarioState->pos[1] += y;
            gMarioState->pos[2] += z;
            if (total < 200.f) {
                if (o->oSubAction != 88) {
                    o->oSubAction = 88;
                    m->interactObj = o;
                    m->usedObj = o;
                    play_sound(SOUND_MENU_ENTER_HOLE, m->marioObj->header.gfx.cameraToObject);
                    mario_stop_riding_object(m);
                    set_mario_action(m, ACT_DISAPPEARED, (WARP_OP_WARP_OBJECT << 16) + 2);
                }
            }
            cur_obj_init_animation(1);
            if (o->header.gfx.unk38.animFrame > 57) {
                o->header.gfx.unk38.animFrame = 8;
            }
            if (!o->oHiddenBlueCoinSwitch) {
                // MODEL_DL_WHIRLPOOL
                o->oHiddenBlueCoinSwitch = spawn_object(o, MODEL_DL_WHIRLPOOL, bhvWhirlpool);
            }
            o->oHiddenBlueCoinSwitch->oFaceAngleRoll = 0x4000;
            o->oHiddenBlueCoinSwitch->oFaceAngleYaw = o->oFaceAngleYaw + 0x4000;
            //    o->oHiddenBlueCoinSwitch->oFaceAnglePitch = 0x4000;
            o->oHiddenBlueCoinSwitch->oPosX = o->oHomeX;
            o->oHiddenBlueCoinSwitch->oPosY = o->oHomeY;
            o->oHiddenBlueCoinSwitch->oPosZ = o->oHomeZ;
        }
    } else {
        cur_obj_init_animation(0);
        if (o->oHiddenBlueCoinSwitch) {
            // MODEL_DL_WHIRLPOOL
            obj_mark_for_deletion(o->oHiddenBlueCoinSwitch);
            o->oHiddenBlueCoinSwitch = 0;
        }
    }
    o->oForwardVel = 12.0f;
    // cur_obj_move_standard(-78);
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oPosY += o->oVelY;
    load_object_collision_model();
}

void arrowgate_code(void) {
    if (o->oAction != 2) {
        if (o->oAction) {
            if (o->oTimer < 70) {
                o->oPosY -= 10.f;
            } else {
                gObjCutsceneDone = TRUE;
                if (cutscene_object(CUTSCENE_STAR_SPAWN, o) == -1) {
                    set_mario_npc_dialog(0);
                    o->oAction = 2;
                }
            }
        } else {
            if ((openDoors & (o->oBehParams & 0xff)) || !(o->oBehParams & 0xff)) {
                if (set_mario_npc_dialog(2) == 2 && cutscene_object(CUTSCENE_STAR_SPAWN, o) == 1) {
                    o->oAction = 1;
                    play_sound(SOUND_GENERAL_WATER_LEVEL_TRIG, gDefaultSoundArgs);
                }
            }
        }
        load_object_collision_model();
    }
}

void jellyfish_code(void) {
    if (!o->oTimer) {
        o->oTimer = random_u16();
    }
    o->oOpacity = 0x3f + sins(o->oTimer * 0x30) * 25.f;
    o->oPosY = sins(o->oTimer * 0x100) * 25.f + o->oHomeY;
    o->header.gfx.scale[1] = 1.f + sins(o->oTimer * 0x70) * .1f;

    o->header.gfx.scale[0] = sqrtf(1.f / o->header.gfx.scale[1]);
    o->header.gfx.scale[2] = o->header.gfx.scale[0];
    o->oFaceAngleYaw =
        atan2s(o->oPosZ - gCurrentArea->camera->pos[2], o->oPosX - gCurrentArea->camera->pos[0])
        + 0x8000;
    /*  if (o->oTimer > 15) {
          o->oTimer = 0;*/
   // o->oAnimState++;
    //   }
}

extern const Trajectory wf_area_3_spline_EELPATH[];
void bigunagiCode(void) {
    f32 x, z, y;
    f32 suckspeed;
    f32 total;
    x = ((s16 *) segmented_to_virtual(wf_area_3_spline_EELPATH))[1 + o->oBehParams2ndByte];
    z = ((s16 *) segmented_to_virtual(wf_area_3_spline_EELPATH))[3 + o->oBehParams2ndByte];
    y = ((s16 *) segmented_to_virtual(wf_area_3_spline_EELPATH))[2 + o->oBehParams2ndByte];
    o->oMoveAngleYaw =
        approach_s16_symmetric(o->oMoveAngleYaw, atan2s(z - o->oPosZ, x - o->oPosX), 0x0040);
    total = sqrtf((x - o->oPosX) * (x - o->oPosX) + (z - o->oPosZ) * (z - o->oPosZ));
    if (total < 400.f) {
        o->oBehParams2ndByte += 4;
        if (o->oBehParams2ndByte == 44) {
            o->oBehParams2ndByte = 0;
        }
    }
    o->oVelY =
        approach_f32_symmetric(o->oVelY, (y - o->oPosY) / ((total + 50.f) / o->oForwardVel), 1.f);
    o->oForwardVel = 12.0f;
    // cur_obj_move_standard(-78);
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oPosY += o->oVelY;
    cur_obj_init_animation(0);
    total = sqrtf(sqr(o->oPosX) + sqr(o->oPosZ));
    total -= 1750.f;
    if (total < 0.f) {
        total = 0.f;
    } else if (total > 8000.f) {
        total = 8000.f;
    }
    o->oOpacity = 255 - (total * (255.f/8000.f));
}

void flameturret(void) {
    struct Object *flame;
    f32 x, y, z;
    u16 oldAngle = o->oMoveAngleYaw;
    if (!o->oHiddenBlueCoinSwitch) {
        o->oHiddenBlueCoinSwitch = spawn_object(o, 0x3D, bhvTurretBase);
    }
    switch (o->oAction) {
        case 0:
            o->oMoveAngleYaw += 0x0100;
            if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < 0x1400) {
                if ((o->oDistanceToMario < 1500.f)
                    && (cur_obj_lateral_dist_from_mario_to_home() > 120.f)) {
                    o->oAction = 1;
                }
            }
            break;
        case 1:
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            if ((o->oDistanceToMario > 1500.f) || (cur_obj_lateral_dist_from_mario_to_home() < 120.f)) {
                o->oAction = 0;
            } else {
                if (!(o->oTimer & 3)) {

                    o->oFlameThowerUnk110 = 32;
                    flame = spawn_object_relative(2, 0, 110.f, 125.f, o, MODEL_RED_FLAME,
                                                  bhvFlamethrowerFlame);
                    flame->oForwardVel = 25.f;
                    flame->oVelY = 0.f;
                    cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
                }
            }
            break;
    }
    if (flame = cur_obj_nearest_object_with_behavior(bhvExplosion)) {
        x = o->oPosX - flame->oPosX;
        y = o->oPosY + 50.f - flame->oPosY;
        z = o->oPosZ - flame->oPosZ;
        if (sqrtf(x * x + y * y + z * z) < 400.f) {
            spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
            spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
            create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
            spawn_mist_particles_variable(0, 0, 46.f);
            spawn_triangle_break_particles(30, 138, 3.0f, 4);
            mark_obj_for_deletion(o);
        }
    }
    o->oAngleVelYaw = o->oMoveAngleYaw - oldAngle;
    load_object_collision_model();
}

void exploderock(void) {
    struct Object *flame;
    if (flame = cur_obj_nearest_object_with_behavior(bhvExplosion)) { // untested tbh
        if (lateral_dist_between_objects(flame, o) < 600.f) {
            if (absf(flame->oPosY - o->oPosY < 600.f)) {
                spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
                create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                mark_obj_for_deletion(o);
            }
        }
    } else if (flame = cur_obj_nearest_object_with_behavior(bhvBobomb)) { // untested tbh
        if (absf(flame->oPosY - o->oPosY < 600.f)) {
            if (lateral_dist_between_objects(flame, o) < 600.f) {
                spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
                create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                mark_obj_for_deletion(o);
                spawn_object(flame, MODEL_EXPLOSION, bhvExplosion);
                mark_obj_for_deletion(flame);
            }
        }
    }
    load_object_collision_model();
}