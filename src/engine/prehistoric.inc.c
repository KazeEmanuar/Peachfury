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
#include "audio/data.h"
#include <PR/gbi.h>
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject
#define objAnimID (o->header.gfx.unk38.animID)
#define objAnimFrame (o->header.gfx.unk38.animFrame)
#define marioObject gMarioState->marioObj
#include "levels/hmc/header.h"
#include "game/OPT_FOR_SIZE.h"

u16 *collPointers[] = { &lavarock1_collision, &lavarock2_collision, &lavarock3_collision,
                        &lavarock4_collision };
void lavarockCode(void) {
    f32 x, z;
    struct Object *spawn;
    s16 prevAngle;
    s16 curAngle;
    s16 prior = o->oMoveAngleYaw;
    u8 bParam;
    s16 *path = (o->oBehParams & 0xFF000000) ? segmented_to_virtual(0) : segmented_to_virtual(0);
    if (o->oBehParams & 0xFF) { // spawner, set scale of spawned rocks
        if (!o->oOpacity) {
            o->oOpacity = (random_u16() % 0x40) + 40;
            // spawn a rock
            x = o->oPosX;
            z = o->oPosZ + random_f32_around_zero(1100.f);
            bParam = (random_u16() % 3);
            spawn = spawn_object(o, 0x35 + bParam, bhvLavaRock);
            spawn->oPosX = x;
            spawn->oPosZ = z;
            spawn->oPosY = find_floor_height(x, o->oPosY + 2000.f, z);
            spawn->oAngleVelYaw = (random_u16() % 100) - 50;
            spawn->hitboxDownOffset = -350.f;
            spawn->oMoveAngleYaw = atan2s(path[7] - o->oPosZ, path[5] - o->oPosX);
            spawn->oOpacity = 1;
            spawn->oBehParams2ndByte = bParam;
            spawn->oBehParams = o->oBehParams & 0xFF000000;
            spawn->collisionData = segmented_to_virtual(collPointers[spawn->oBehParams2ndByte]);
            spawn->header.gfx.scale[0] = 1.f + random_f32_around_zero(0.4f);
            spawn->header.gfx.scale[1] = 1.f + random_f32_around_zero(0.4f);
            spawn->header.gfx.scale[2] = 1.f + random_f32_around_zero(0.4f);
        }
        o->oOpacity--;
    } else {
        o->oForwardVel = 14.f;
        switch (o->oAction) {
            case 0:
                // come out of lava
                o->hitboxDownOffset = approach_f32_symmetric(o->hitboxDownOffset, 0, 4.f);
                if (o->hitboxDownOffset == 0) {
                    o->oAction = 1;
                }
                break;
            case 1:
                // be in lava
                if (cur_obj_is_mario_on_platform()) {
                    o->oAction = 2;
                }
                break;
            case 2:
                // back into lava
                if (o->oTimer > 150) {
                    obj_mark_for_deletion(o);
                }
                o->hitboxDownOffset -= 2.f;
                break;
        }
        x = path[1 + o->oOpacity * 4] - o->oPosX;
        z = path[3 + o->oOpacity * 4] - o->oPosZ;
        prevAngle = atan2s(path[3 + o->oOpacity * 4] - path[3 - 4 + o->oOpacity * 4],
                           path[1 + o->oOpacity * 4] - path[1 - 4 + o->oOpacity * 4]);
        curAngle = atan2s(z, x);
        if (coss(abs_angle_diff(prevAngle, curAngle)) > 0) {
            if ((sqrtf(x * x + z * z) / coss(abs_angle_diff(prevAngle, curAngle)) < 50.f)) {
                o->oOpacity++;
                o->oMoveAngleYaw += (atan2s(path[3 + o->oOpacity * 4] - path[3 - 4 + o->oOpacity * 4],
                                            path[1 + o->oOpacity * 4] - path[1 - 4 + o->oOpacity * 4])
                                     - prevAngle);
            }
        } else {
            o->oOpacity++;
            o->oMoveAngleYaw += (atan2s(path[3 + o->oOpacity * 4] - path[3 - 4 + o->oOpacity * 4],
                                        path[1 + o->oOpacity * 4] - path[1 - 4 + o->oOpacity * 4])
                                 - prevAngle);
        }
        if (path[o->oOpacity * 4] == -1) {
            o->oAction = 2;
            o->oMoveAngleYaw = prior;
        }
        o->oFloorHeight = find_floor(o->oPosX, o->oPosY + 1000.f, o->oPosZ, &o->oFloorPointer);
        if (o->oFloorPointer && (o->oFloorPointer->object)) {
            o->oPosY -= 2.f;
        } else {
            o->oPosY = o->oFloorHeight + o->hitboxDownOffset;
        }
        o->oFaceAngleYaw += o->oAngleVelYaw;
        if (o->oFloorPointer) {
            o->oFaceAnglePitch = approach_s16_symmetric(
                o->oFaceAnglePitch,
                (coss(o->oFaceAngleYaw
                      - atan2s(o->oFloorPointer->normal.z, o->oFloorPointer->normal.x)))
                    * atan2s(o->oFloorPointer->normal.y,
                             sqrtf(o->oFloorPointer->normal.x * o->oFloorPointer->normal.x
                                   + o->oFloorPointer->normal.z * o->oFloorPointer->normal.z)),
                0x120 + (o->oTimer == 0) * 0x4000);
            o->oFaceAngleRoll = approach_s16_symmetric(
                o->oFaceAngleRoll,
                (sins(o->oFaceAngleYaw
                      - atan2s(o->oFloorPointer->normal.z, o->oFloorPointer->normal.x)))
                    * atan2s(o->oFloorPointer->normal.y,
                             sqrtf(o->oFloorPointer->normal.x * o->oFloorPointer->normal.x
                                   + o->oFloorPointer->normal.z * o->oFloorPointer->normal.z)),
                0x120 + (o->oTimer == 0) * 0x4000);
        }
        obj_move_xyz_using_fvel_and_yaw();
        load_object_collision_model();
    }
}

void seesawRock(void) {
    if (!o->oHiddenBlueCoinSwitch) {
        o->oHiddenBlueCoinSwitch = spawn_object(o, 0x3A, bhvSeeSawPlank);
        o->oHiddenBlueCoinSwitch->oPosY += 140.f;
        o->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch =
            spawn_object_relative(0, 0, 0, -750.f, o, 0x3B, bhvSeeSawSaurus);
    }
    load_object_collision_model();
}

s32 shouldMarioAffectPlank(f32 offsetAdd) {
    if (!cur_obj_is_mario_on_platform()) {
        return 0;
    } else if (coss(atan2s(gMarioObject->oPosZ - o->oPosZ, gMarioObject->oPosX - o->oPosX)
                    + o->oFaceAngleYaw)
               > 0) {
        return 0;
    }
    if (dist_between_objects(o, gMarioState->marioObj) < 300.f + offsetAdd) {
        return 0;
    }
    return 1;
}

#define GOALPITCH 2745
void seesawPlank(void) {
    o->oFaceAnglePitch += o->oAngleVelPitch;
    switch (o->oAction) {
        case 0:
            // fallen over, mario not on it
            o->oFaceAnglePitch = GOALPITCH;
            if (shouldMarioAffectPlank(0.f)) {
                o->oAction = 1;
                o->oHiddenBlueCoinSwitch->oAction = 1;
                o->oHiddenBlueCoinSwitch->oVelY = 58.f;
                o->oHiddenBlueCoinSwitch->oForwardVel = 16.f;
            }
            break;
        case 1:
// mario is on it, yoshi jumps
#define LANDFRAME 20
            if (o->oTimer == LANDFRAME) {
                o->oAction++;
            }
            break;
        case 2:
// yoshi has jumped and the bridge now falls over
#define FLYFRAME 11
            o->oAngleVelPitch -= 0x7E;
            if (o->oTimer == FLYFRAME) {
                o->oAction++;
                o->oFaceAnglePitch = -GOALPITCH;
                o->oAngleVelPitch = 0;
                if (shouldMarioAffectPlank(-205.f)) {
#define SHOTVEL 55.f
                    if (abs_angle_diff(gMarioState->faceAngle[1], o->oFaceAngleYaw) > 0x3FFF) {
                        gMarioState->action = ACT_THROWN_FORWARD;
                        gMarioState->vel[1] = 100.f;
                        gMarioState->forwardVel = SHOTVEL;
                        gMarioState->faceAngle[1] = o->oFaceAngleYaw + 0x8000;
                    } else {
                        gMarioState->action = ACT_THROWN_BACKWARD;
                        gMarioState->vel[1] = 100.f;
                        gMarioState->forwardVel = -SHOTVEL;
                        gMarioState->faceAngle[1] = o->oFaceAngleYaw;
                    }
                }
            }
            break;
        case 3:
            // done jumping, make yoshi jump/fall off again, tip over
            if (shouldMarioAffectPlank(-100.f)) {
                o->oAngleVelPitch += 0x20;
            } else {
                o->oAngleVelPitch += 0x10;
            }
            if (o->oFaceAnglePitch >= GOALPITCH) {
                o->oAction = 0;
                o->oFaceAnglePitch = GOALPITCH;
                o->oAngleVelPitch = 0;
            }
            break;
    }
    load_object_collision_model();
}
void seesawSaurus(void) {
    switch (o->oAction) {
        case 0:
            // do nothing
            break;
        case 1:
            if (!o->oTimer) {
                // set animation
                cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB_JUMP);
                cur_obj_init_animation(1);
            }
            if (o->oTimer == LANDFRAME) {
                o->oVelY = 0.f;
                o->oForwardVel = 0.f;
                o->oAction++;
                cur_obj_play_sound_2(SOUND_GENERAL_SOFT_LANDING);
            }
            break;
        case 2:
            if (o->oTimer == FLYFRAME / 2) {
                cur_obj_play_sound_2(SOUND_GENERAL_SWISH_AIR);
            }
            if (o->oTimer == FLYFRAME) {
                o->oAction++;
                o->oVelY = 20.f;
                o->oForwardVel = -20.f;
                cur_obj_play_sound_2(SOUND_OBJ_DORRIE);
            }
            break;
        case 3:
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 0.75f);
            o->oPosX = approach_f32_symmetric(o->oPosX, o->oHomeX, 2.f);
            o->oPosZ = approach_f32_symmetric(o->oPosZ, o->oHomeZ, 2.f);
            cur_obj_init_animation(0);
            break;
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

#include "levels/hmc/header.h"
void nessieBlink() {
    u8 *copyThis = 0;
    u8 *copyThis2 = 0;
    switch (o->oAnimState) {
        case 0:
            copyThis = (nessie_RaidonEye_alb_ci4_pal_rgba16);
            copyThis2 = (nessie_RaidonEye_alb_ci4);
            break;
        case 1:
            copyThis = (dorrieeyes_unkno2wn_ci4_pal_rgba16);
            copyThis2 = (dorrieeyes_unkno2wn_ci4);
            break;
        case 2:
            copyThis = (dorrieeyes_eyeframenessie2_ci4_pal_rgba16);
            copyThis2 = (dorrieeyes_eyeframenessie2_ci4);
            break;
    }
    ((Gfx *) segmented_to_virtual(&mat_nessie_f3d_material_003[5]))->words.w1 = copyThis;
    ((Gfx *) segmented_to_virtual(&mat_nessie_f3d_material_003[12]))->words.w1 = copyThis2;
}

// animations:
// get out of lava, make happy noise
// idle bop up and down, mouth movement only
// happy wave "jump on me pls"
// go back in with happy noise
void nessielava(void) {
    if (!o->oOpacity) {
        o->oOpacity = o->oMoveAngleYaw;
        cur_obj_update_floor_height();
        o->oBobombBuddyPosXCopy = o->oHomeY - o->oFloorHeight;
    }
    switch (o->oAction) {
        case 0:
            // wait mario close
            o->oPosY = o->oHomeY;
            o->oMoveAngleYaw = o->oOpacity;
            cur_obj_init_animation(0);
            o->header.gfx.unk38.animFrame = 0;
            cur_obj_set_model(0);
            if (o->oDistanceToMario < 2500.f) {
                o->oAction++;
                cur_obj_set_model(0x3C);
                cur_obj_play_sound_2(SOUND_OBJ_DORRIE);
            }
            break;
        case 1:
            // come up and wait, do headbop anim
            if (o->header.gfx.unk38.animID == 0) {
                o->oAnimState = 2;
            }
            bowserBlink(&o->oBobombBlinkTimer);
            if (cur_obj_check_if_at_animation_end()) {
                cur_obj_init_animation(1);
                o->oAnimState = 0;
            }
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            if (o->oDistanceToMario > 3000.f) {
                o->oAction = 4;
                cur_obj_play_sound_2(SOUND_OBJ_DORRIE);
                cur_obj_init_animation(3);
            }
            if (o->oDistanceToMario < 1500.f) {
                o->oAction++;
                o->oAnimState = 0;
            }
            break;
        case 2:
            bowserBlink(&o->oBobombBlinkTimer);
            // look at mario with expecations, stare anim
            if ((o->header.gfx.unk38.animFrame == 19) || (o->header.gfx.unk38.animFrame == 39)) {
                cur_obj_init_animation(2);
            }
            o->oPosY =
                approach_f32_symmetric(o->oPosY, o->oHomeY + sins(o->oTimer * 0x200) * 25.f, 4.f);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            if (o->oDistanceToMario > 1500.f) {
                o->oAction = 1;
            }
            if (cur_obj_is_mario_on_platform()) {
                o->oAction = 3;
            }
            if (o->oDistanceToMario > 2000.f) {
                o->oAction = 4;
                cur_obj_play_sound_2(SOUND_OBJ_DORRIE);
                cur_obj_init_animation(3);
            }
            break;
        case 3:
            bowserBlink(&o->oBobombBlinkTimer);
            // let mario get on and rotate
            cur_obj_update_floor_height();
            o->oPosY = approach_f32_symmetric(
                o->oPosY, o->oFloorHeight + o->oBobombBuddyPosXCopy + sins(o->oTimer * 0x200) * 25.f,
                4.f);
            o->oMoveAngleYaw =
                approach_s16_symmetric(o->oMoveAngleYaw, o->oBehParams2ndByte << 8, 0x200);
            if (abs_angle_diff(o->oMoveAngleYaw, o->oBehParams2ndByte << 8) < 0x1000) {
                if (cur_obj_lateral_dist_to_home() < (o->oBehParams & 0xFF) * 256.f) {
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 12.f, .5f);
                } else {
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.f);
                }
                obj_move_xyz_using_fvel_and_yaw();
            }
            if (o->oDistanceToMario > 1500.f) {
                o->oForwardVel = 0.f;
                o->oAction = 4;
                cur_obj_play_sound_2(SOUND_OBJ_DORRIE);
                cur_obj_init_animation(3);
            }
            if (cur_obj_is_mario_ground_pounding_platform()) {
                o->oAction = 4;
                cur_obj_play_sound_2(SOUND_OBJ_DORRIE);
                cur_obj_init_animation(3);
                o->oForwardVel = 0.f;
            }
            break;
        case 4:
            // say goodbye mario and go underground
            o->oAnimState = 2;
            if (o->oTimer > 50) {
                cur_obj_set_model(0);
                if (o->oDistanceToMario > 2000.f) {
                    o->oAction = 0;
                    o->oPosX = o->oHomeX;
                    o->oPosY = o->oHomeY;
                    o->oPosZ = o->oHomeZ;
                }
            }

            break;
    }
    if (o->oAction) {
        nessieBlink();
    }
    o->oAngleVelYaw = o->oMoveAngleYaw - o->oFaceAngleYaw;
    if ((o->oAction != 0) && (o->oAction != 4)) {
        load_object_collision_model();
    }
}

s32 spawnDust(u8 count) {
    s32 i;
    struct Object *part;
    f32 magn;
    s16 angle;

    for (i = 0; i < count; i++) {
        if (o->oOpacity > 25) {
            o->oOpacity -= 25;
            angle = random_u16();
            magn = random_float() * 600.f;
            part = spawn_object_rel_with_rot(o, MODEL_MIST, bhvBlackSmokeMario, sins(angle) * magn,
                                             225.f - magn / 7.f, coss(angle) * magn, 0, 0, 0);
            part->oBehParams2ndByte = 1;
            obj_scale(part, 2.f);
        }
    }
}
#define GOALSCALE (o->oBehParams2ndByte / 100.f + 1.f)
void diagshroom(void) {
    if (!o->oOpacity) {
        cur_obj_scale(GOALSCALE);
        o->oOpacity++;
    }
    if (cur_obj_is_mario_on_platform()) {
        if (SURFACETYPE(gMarioState->floor) == 0x15) {
            if (!o->oBehParams2ndByte) {
                m->action = ACT_WATER_JUMP;
                m->vel[1] = 90.f;
                m->forwardVel = 48.f;
                m->faceAngle[1] = o->oFaceAngleYaw;
            } else {
                m->action = ACT_TRIPLE_JUMP;
                m->vel[1] = 99.f + o->oBehParams2ndByte;
            }
            o->oForwardVel = .08f;
            cur_obj_play_sound_1(SOUND_OBJ_SNOWMAN_BOUNCE);
            m->marioObj->header.gfx.unk38.animFrame = 0;
            o->oTimer = 0;
            spawnDust(3);
        }
    }
    cur_obj_scale(o->header.gfx.scale[0] + o->oForwardVel);
    o->oForwardVel =
        approach_f32_symmetric(o->oForwardVel, (GOALSCALE - o->header.gfx.scale[0]), 0.01f);
    o->oForwardVel *= 0.96f;
    if ((m->action == ACT_TRIPLE_JUMP) && (o->oTimer < 45)) {
        if (o->oTimer < 14) {
            spawnDust(1);
        }
        if (o->oBehParams2ndByte) {
            m->faceAngle[1] = approach_s16_symmetric(m->faceAngle[1], m->intendedYaw, 0x200);
        }
        m->peakHeight = -100000.f;
    } else {
        load_object_collision_model();
    }
}

void geysircode(void) {
    struct Object *part;
    f32 magn;
    s16 angle;

    switch (o->oAction) {
        case 0:
            // wait to activate geysir
            if (o->oDistanceToMario < 800.f) {
                o->oTimer++;
            }
            if (o->oDistanceToMario < 200.f) {
                o->oTimer += 3;
            }
            if (!o->oTimer) {
                o->oOpacity = random_u16() % 200;
            }
            if (o->oTimer > 400 + o->oOpacity) {
                o->oAction = 1;
                cur_obj_play_sound_2(SOUND_AIR_HOWLING_WIND);
            }
            break;
        case 1:
            // shoot dust
            if (lateral_dist_between_objects(o, gMarioState->marioObj) < 75.f) {
                if (gMarioState->pos[1] < o->oPosY + 100.f) {
                    if (abs_angle_diff(o->oMoveAngleYaw, gMarioState->faceAngle[1]) > 0x3FFF) {
                        gMarioState->action = ACT_THROWN_BACKWARD;
                        gMarioState->forwardVel = -(o->oBehParams & 0xFF) * 1.5f;
                        gMarioState->vel[1] = o->oBehParams2ndByte * 1.5f;
                        gMarioState->faceAngle[1] = o->oMoveAngleYaw + 0x8000;
                    } else {
                        gMarioState->action = ACT_THROWN_FORWARD;
                        gMarioState->forwardVel = (o->oBehParams & 0xFF) * 1.5f;
                        gMarioState->vel[1] = o->oBehParams2ndByte * 1.5f;
                        gMarioState->faceAngle[1] = o->oMoveAngleYaw;
                    }
                }
            }
            angle = random_u16();
            magn = random_float() * 40.f;
            if (random_u16() & 1) {
                part = spawn_object(o, MODEL_SMOKE, bhvBlackSmokeMario);
                part->oPosY -= 125.f;
            } else {
                part = spawn_object(o, MODEL_MIST, bhvBlackSmokeMario);
                part->oPosY -= 75.f;
            }
            part->oBehParams2ndByte = 2;
            part->oVelY = o->oBehParams2ndByte * .75f;
            part->oForwardVel = (o->oBehParams & 0xFF) * .75f;
            part->oPosX += sins(angle) * magn;
            part->oPosZ += coss(angle) * magn;
            obj_scale(part, 1.5f);
            if (o->oTimer > 16) {
                o->oAction = 0;
            }
            break;
    }
}
void leafBendy(void) {
    struct Object *coin;
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_on_platform()) {
                o->oAction = 1;
                if (((o->oBehParams >> 8) & 0xFF) < 5) {
                    o->oBehParams = (((o->oBehParams >> 8) & 0xFF) + 1) << 8;
                    coin = spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
                    coin->oPosY += o->header.gfx.scale[1] * 100.f;
                }
            }
            o->header.gfx.scale[1] = approach_f32_symmetric(o->header.gfx.scale[1], 1.f, 0.01f);
            break;
        case 1:
            o->header.gfx.scale[1] = approach_f32_symmetric(o->header.gfx.scale[1], 0.3f, 0.01f);
            o->header.gfx.scale[1] = approach_f32_asymptotic(o->header.gfx.scale[1], 0.3f, 0.05f);
            if (!cur_obj_is_mario_on_platform()) {
                o->oAction = 0;
            }
            break;
    }
    o->header.gfx.scale[2] = 1.5f - o->header.gfx.scale[1] / 2.f;
    o->oCollisionDistance = 500.f;
    load_object_collision_model();
}

#define SEGMENTLENGTH -200.f
void calcMarioVinePos() {
    s16 rotation = o->oMoveAnglePitch / 5;
    f32 xc = sins(o->oFaceAngleYaw);
    f32 zc = coss(o->oFaceAngleYaw);
    f32 currX = o->oPosX;
    f32 currY = o->oPosY + 1000.f;
    f32 currZ = o->oPosZ;
    s32 i;

    for (i = 1; i < 5; i++) {
        currX += xc * sins(rotation * i) * SEGMENTLENGTH;
        currY += coss(rotation * i) * SEGMENTLENGTH;
        currZ += zc * sins(rotation * i) * SEGMENTLENGTH;
    }
#define VISUALOFFSET -50.f
    currX += xc * sins(0X4000 + rotation * 4) * VISUALOFFSET;
    currY += coss(0X4000 + rotation * 4) * VISUALOFFSET;
    currZ += zc * sins(0X4000 + rotation * 4) * VISUALOFFSET;
    if (o->oTimer < 15) {
        gMarioState->pos[0] =
            approach_f32_asymptotic(gMarioState->pos[0], currX, 0.06666f * (o->oTimer + 1));
        gMarioState->pos[1] =
            approach_f32_asymptotic(gMarioState->pos[1], currY, 0.06666f * (o->oTimer + 1));
        gMarioState->pos[2] =
            approach_f32_asymptotic(gMarioState->pos[2], currZ, 0.06666f * (o->oTimer + 1));
        gMarioState->pos[0] = approach_f32_symmetric(gMarioState->pos[0], currX, 10.f);
        gMarioState->pos[1] = approach_f32_symmetric(gMarioState->pos[1], currY, 10.f);
        gMarioState->pos[2] = approach_f32_symmetric(gMarioState->pos[2], currZ, 10.f);
    } else {
        gMarioState->pos[0] = currX;
        gMarioState->pos[1] = currY;
        gMarioState->pos[2] = currZ;
    }
}
/*
    s32 actionId = m->action & ACT_ID_MASK;
    if ((gMarioState->pos[1] + 80.f) > o->oPosY) { // polefix
        o->oInteractStatus |= INT_STATUS_INTERACTED;
        if ((m->action & ACT_ID_MASK) >= 0x080 && (m->action & ACT_ID_MASK) < 0x0A0) {*/
extern Vec3f joinPosition[6][4];
void swingVein(void) {
    s32 i;
    s16 *transformers;
    f32 speedScale = 1.f;
    o->oAnimState = o->oBehParams2ndByte;
    switch (o->oAction) {
        case 0:
            if ((!random_u16() & 0x3F)) {
                o->oAngleVelPitch += (random_u16() & 200) - 100;
            }
            if (o->oTimer > 20) {
                if ((lateral_dist_between_objects(o, gMarioState->marioObj) < 100.f)
                    && (gMarioState->pos[1] + 100.f > o->oPosY)
                    && (gMarioState->pos[1] < o->oPosY + 1000.f)) {
                    if ((m->action & ACT_ID_MASK) >= 0x080 && (m->action & ACT_ID_MASK) < 0x0A0) {
                        o->oAction++;
                        gMarioState->action = ACT_HANG_VINE;
                        gMarioState->usedObj = o;
                        o->oAngleVelPitch = -gMarioState->forwardVel / 0.01581917687f / 2.f;
                        play_sound(SOUND_MARIO_WHOA, m->marioObj->header.gfx.cameraToObject);
                    }
                }
            }
            break;
        case 1:
#define OFFSET -1000.f
            speedScale += 4.f - absf((o->oAngleVelPitch - (o->oMoveAnglePitch / 64)) / 0x1000);
            gMarioState->faceAngle[1] =
                approach_s16_symmetric(gMarioState->faceAngle[1], o->oFaceAngleYaw, 0xC00);
            gMarioState->action = ACT_HANG_VINE;
            gMarioState->usedObj = o;
            /*gMarioState->pos[0] = joinPosition[1][o->oAnimState][0];
            gMarioState->pos[1] = joinPosition[1][o->oAnimState][1];
            gMarioState->pos[2] = joinPosition[1][o->oAnimState][2];*/
            calcMarioVinePos();
            o->oAngleVelPitch -= coss(gMarioState->intendedYaw - o->oFaceAngleYaw)
                                 * gMarioState->intendedMag * speedScale * 0.5f;
            o->oAngleVelPitch *= .975f;
            if (gMarioState->controller->buttonPressed & A_BUTTON) {
                o->oAction = 0;
                gMarioState->action = ACT_TRIPLE_JUMP;
                play_sound(SOUND_MARIO_YAHOO_WAHA_YIPPEE + ((gAudioRandom % 5) << 16),
                           m->marioObj->header.gfx.cameraToObject);
                gMarioState->vel[1] =
                    o->oAngleVelPitch * coss(o->oMoveAnglePitch - 0x4000) * 0.01581917687f * 2.5f
                    + 25.f;
                gMarioState->forwardVel = o->oAngleVelPitch * sins(o->oMoveAnglePitch - 0x4000)
                                          * 0.01581917687f * 2.5f * 1.2f;
            }
            switch (o->oOpacity) {
                case 0:
                    if (absi(o->oAngleVelPitch) > 0x400) {
                        o->oOpacity++;
                        cur_obj_play_sound_2(SOUND_GENERAL_SWISH_AIR);
                    }
                    break;
                case 1:
                    if (absi(o->oAngleVelPitch) < 0x200) {
                        o->oOpacity = 0;
                    }
                    break;
            }
            break;
    }

    cur_obj_init_animation(o->oBehParams2ndByte);

    transformers = segmented_to_virtual(o->header.gfx.unk38.curAnim->values);
    for (i = 0; i < 5; i++) {
        transformers[3 + i * 3] = o->oArrowLiftUnk100 / 5;
    }
    o->oArrowLiftUnk100 = o->oMoveAnglePitch;
    o->oAngleVelPitch -= o->oMoveAnglePitch / 64;
    o->oAngleVelPitch *= .99f;
    o->oMoveAnglePitch += o->oAngleVelPitch;
}

void recnor(void) {
    struct Surface *ahead = 0;
    f32 aheadHeight;
    struct Object *dust;
    o->oDamageOrCoinValue = 2;
    o->oInteractStatus = 0;
    if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < 0x2000) {
        o->oInteractType = INTERACT_DAMAGE;
    } else {
        o->oInteractType = INTERACT_BOUNCE_TOP2;
    }
    if (o->oDistanceToMario < 300.f) {
        if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) > 0x7000) {
            if (able_to_grab_object(m, o)) {
                m->interactObj = o;
                m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
                o->oAction = 6;
                m->faceAngle[1] = m->interactObj->oMoveAngleYaw;
                m->usedObj = m->interactObj;
                set_mario_action(m, ACT_PICKING_UP_BOWSER, 0);
            }
        }
    }
    switch (o->oAction) {
        case 0:
            // eating gras, if mario is loud while close to him, go into look around
            cur_obj_init_animation(0);
            if (cur_obj_check_if_at_animation_end()) {
                if (!o->oOpacity) {
                    if (!(random_u16() % 5)) {
                        o->oOpacity = (random_u16() % 50) + 15;
                    } else {
                        o->header.gfx.unk38.animFrame = 0;
                    }
                }
            }
            if (o->oDistanceToMario < 2250.f) {
                if (((gMarioState->forwardVel + gMarioState->vel[1]) > o->oDistanceToMario / 25.f)
                    || ((gMarioState->action == ACT_GROUND_POUND_LAND))
                    || (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < 0x2000)) {
                    o->oAction = 1;
                    cur_obj_play_sound_2(SOUND_OBJ_MR_BLIZZARD_ALERT);
                }
            }
            break;
        case 1:
            // look around
            // if mario is close, charge
            cur_obj_init_animation(1);
            if (cur_obj_check_if_at_animation_end()) {
                o->header.gfx.unk38.animFrame = 9;
            }
            if ((o->header.gfx.unk38.animFrame == 14) || (o->header.gfx.unk38.animFrame == 54)) {
                spawn_object_relative(0, 0, 0, -140, o, MODEL_SMOKE, bhvDustBackwards);
                cur_obj_play_sound_2(SOUND_ENV_SLIDING);
            }
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x400);
            if (o->oDistanceToMario > 2250.f) {
                o->oAction = 0;
            }
            if ((abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < 0x400)
                && (o->oDistanceToMario
                    < (1400.f + abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) / 500.f))) {
                o->oAction = 2;
                cur_obj_play_sound_2(SOUND_GENERAL_PAINTING_EJECT);
            }
            break;
        case 2:
            // load up the charge, stomp feet and stuff
            cur_obj_init_animation(2);
            if (o->oTimer > 18) {
                o->oForwardVel = 40.f;
                spawn_object_relative(0, 0, 0, -140, o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oTimer == 18) {
                cur_obj_play_sound_2(SOUND_OBJ_KOOPA_THE_QUICK_WALK);
            }
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 3;
            }
            break;
        case 3:
            // chargeattack
            cur_obj_init_animation(3);
            if (cur_obj_check_if_at_animation_end()) {
                o->header.gfx.unk38.animFrame = 5;
            }
            if (o->header.gfx.unk38.animFrame == 6) {
                spawn_object_relative(0, 0, 0, -140, o, MODEL_SMOKE, bhvDustBackwards);
                cur_obj_play_sound_2(SOUND_OBJ_KOOPA_THE_QUICK_WALK);
            }
            o->oForwardVel = 40.f;
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x100);
            spawn_object_relative(0, 0, 0, -140, o, MODEL_SMOKE, bhvDustBackwards);
#define AHEAD 13.f
            aheadHeight = find_floor(o->oPosX + o->oVelX * AHEAD, o->oPosY + 400.f,
                                     o->oPosZ + o->oVelZ * AHEAD, &ahead);
            if (ahead) {
                if (o->oPosY > aheadHeight + 200.f) {
                    o->oAction = 4;
                }
            } else if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
                o->oAction = 5;
                o->oVelY = 30.f;
                o->oForwardVel = -20.f;
            }
            if (o->oTimer > 80) {
                o->oAction = 4;
            }
            break;
        case 4:
            // stop charge, near a cliff, have the tail move slowly left/right, otherwise no movement
            cur_obj_init_animation(4);
            o->oForwardVel = approach_s16_symmetric(o->oForwardVel, 0.f, 1.f);
            if (o->oForwardVel < 1.f) {
                if (cur_obj_check_if_at_animation_end()) {
                    o->oAction = 1;
                }
            } else {
                cur_obj_play_sound_2(SOUND_ENV_SLIDING);
                dust = spawn_object_relative(0, 0, 0, 240, o, MODEL_SMOKE, bhvDustBackwards);
                dust->oMoveAngleYaw += 0x8000;
            }
            break;
        case 5:
            // stop charge, bumped a wall, have the tail move slowly left/right, otherwise no movement
            cur_obj_init_animation(5);
            o->oForwardVel = approach_s16_symmetric(o->oForwardVel, 0.f, 1.f);
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 1;
            }
            break;
        case 6:
// tail grabbed
#define SWINGSPEED (abs_angle_diff(m->faceAngle[1], o->oOpacity))
#define MULTIPLIER 0.015f
            cur_obj_init_animation(6);
            if (o->oSubAction) {
                cur_obj_hide();
            }
            if (gMarioState->action == ACT_RELEASING_BOWSER) {
                o->oAction = 7;
                cur_obj_unhide();
                o->oVelY = SWINGSPEED * MULTIPLIER;
                o->oForwardVel = SWINGSPEED * MULTIPLIER * 2.f;
            } else if ((gMarioState->action != ACT_HOLDING_BOWSER)
                       && (gMarioState->action != ACT_PICKING_UP_BOWSER)) {
                cur_obj_unhide();
                o->oAction = 1;
            }
            o->oOpacity = o->oSubAction;
            o->oSubAction = m->faceAngle[1];
            break;
        case 7:
            // thrown, dies if hit a wall (with more than 20 speed)
            // instadies if thrown into lava
            o->oForwardVel = approach_s16_symmetric(o->oForwardVel, 0.f, 1.f);
            if (o->oForwardVel < 1.f) {
                o->oAction = 1;
            }
            if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
                o->oAction = 8;
                o->oVelY = 0.f;
                o->oForwardVel = 0.f;
            }
            break;
        case 8:
            // nose stuck in a wall, fall over backwards, struggle with feet, shrink, die
            if (o->oTimer < 10) {
                o->oGravity = 0;
                o->oVelY = 0;
                cur_obj_play_sound_2(SOUND_GENERAL_PAINTING_EJECT);
            } else {
                o->oGravity = -4.f;
                cur_obj_init_animation(7);
                if (cur_obj_check_if_at_animation_end()) {
                    cur_obj_scale(o->header.gfx.scale[0] - 0.08f);
                    if (o->header.gfx.scale[1] < 0.08f) {
                        spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
                        spawn_mist_particles_variable(0, 0, 46.f);
                        obj_mark_for_deletion(o);
                        cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_DYING);
                    }
                }
            }
            o->header.gfx.scale[2] =
                approach_f32_asymptotic(o->header.gfx.scale[2], o->header.gfx.scale[0] / 2.f, .5f);
            break;
    }
    if (o->oFloorPointer) {
        if (o->oFloorPointer->type & (SPECFLAG_BURNING << 8)) {
            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
                spawn_mist_particles_variable(0, 0, 46.f);
                obj_mark_for_deletion(o);
                cur_obj_play_sound_2(SOUND_GENERAL_PAINTING_EJECT);
            }
        }
    }
    if (o->oOpacity) {
        o->oOpacity--;
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
}
extern Gfx mat_yoshimama_EyesOpen[];
void blinkMama(void) {
    u8 *copyThis = 0;
    switch (o->oAnimState) {
        case 0:
            copyThis = yoshimama_MammaEyeV4_ia8;
            break;
        case 1:
            copyThis = mamaeyes_MammaEyeV4Half_ia8;
            break;
        case 2:
            copyThis = mamaeyes_MammaEyeV3Close_ia8;
            break;
        case 3:
            copyThis = mamaeyes_MammaEyeV3CloseHappy_ia8;
            break;
    }
    ((Gfx *) segmented_to_virtual(&mat_yoshimama_EyesOpen[5]))->words.w1 = copyThis;
}
// sitting with egg, worried about the 3 dipshists
// yoshimama with kid sitting on leg, flips kid up and the kid does spins or something
void yoshiMama(void) {
    if (gCurrActNum <= 3) {
        cur_obj_init_animation(0);
    } else {
    }
    switch (o->oAction) {
        case 0:
            if (o->oDistanceToMario < 400.f) {
                o->oAction = 1;
            }
            break;
        case 1:
            if (talkToMarioNoRotation(o->oBehParams2ndByte, 2)) {
                o->oAction = 2;
            }
            break;
        case 2:
            if (o->oDistanceToMario > 700.f) {
                o->oAction = 0;
            }
            break;
    }
    if (o->oAnimState != 3) {
        bowserBlink(&o->oBobombBlinkTimer);
    }
    blinkMama();
}

void babyegg(void) {
    o->header.gfx.scale[1] = 1.f + sins(o->oTimer * 0x700) * 0.1f;
}
s32 checkMarioAttack() {
    switch (gMarioStates->action) {
        case ACT_SLIDE_KICK:
        case ACT_PUNCHING:
        case ACT_MOVE_PUNCHING:
        case ACT_SLIDE_KICK_SLIDE:
            return 1;
            break;
        case ACT_WALL_KICK_AIR:
        case ACT_JUMP_KICK:
            return 2;
            break;
        default:
            return 0;
            break;
    }
}
u8 yoshisRan = 0;
void fakebush(void) {
    if (LevelTimer < 5) {
        yoshisRan = 0;
    }
    if ((gMarioState->wallPunched && (gMarioState->wallPunched->object == o))
        || ((gMarioState->wall && (gMarioState->wall->object == o)))) {
        if (coss(abs_angle_diff(o->oFaceAngleYaw, gMarioState->faceAngle[1])) > 0) {
            o->oAngleVelPitch = 0x800;
        } else {
            o->oAngleVelPitch = -0x800;
        }
        o->oAction = 1;
        cur_obj_play_sound_2(SOUND_GENERAL_SHAKE_COFFIN);
    }
    o->oFaceAnglePitch += o->oAngleVelPitch;
    if (o->oAction) {
        if (o->oFaceAnglePitch > 0x4000) {
            o->oFaceAnglePitch = 0x4000;
            o->oAngleVelPitch = 0;
            cur_obj_play_sound_2(SOUND_GENERAL_SOFT_LANDING);
            o->oOpacity = 0x77;
        } else if (o->oFaceAnglePitch < -0x4000) {
            o->oAngleVelPitch = 0;
            o->oFaceAnglePitch = -0x4000;
            cur_obj_play_sound_2(SOUND_GENERAL_SOFT_LANDING);
            o->oOpacity = 0x77;
        }
    } else {
        o->oAngleVelPitch -= o->oFaceAnglePitch / 8;
        o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0x20);
    }
    if ((o->oFaceAnglePitch != 0x4000) && (o->oFaceAnglePitch != -0x4000)) {
        load_object_collision_model();
    } else {
        yoshisRan = 1;
    }
}

void dipshitFollowPath(s16 *path, f32 runSpeed, f32 animMultiplier) {
    s16 *curPoint = segmented_to_virtual(path);
    s16 targetAngle;
    struct Object *dust;
    if (!yoshisRan) {
        cur_obj_set_model(0);
        o->oIntangibleTimer = -1;
        return;
    }
    if (o->oAction != 3) {
        if (o->oPosY < -800.f) {
            cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WAAH);
            o->oAction = 3;
            play_puzzle_jingle();
        }
    }
    switch (o->oAction) {
        case 0:
            // stand and wait
            cur_obj_init_animation(0);
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0, 5.f);
            o->oMoveAngleYaw =
                approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x0080 * runSpeed);
            if (o->oDistanceToMario < 500.f) {
                o->oAction = 1;
                if (o->oBehParams & 2) {
                    o->oAction = 4;
                    cur_obj_init_animation(1);
                    cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WAAH);
                }
            }
            break;
        case 1:
            // walk away
            cur_obj_init_animation(2);
            o->oBobombBuddyPosZCopy = curPoint[3 - o->oOpacity];
            o->oBobombBuddyPosXCopy = curPoint[1 - o->oOpacity];
            targetAngle =
                atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0080 * runSpeed);
            o->oForwardVel = approach_f32_symmetric(
                o->oForwardVel, runSpeed * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 2.f);
            o->header.gfx.unk38.animFrame += (o->oForwardVel * animMultiplier) / 7.f;
            if (sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                      + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX))
                < o->oForwardVel * 2.f) {
                o->oOpacity += 4;
                if (curPoint[2 - o->oOpacity] > o->oPosY + 200.f) {
                    o->oAction = 2;
                    o->oVelY = 60.f;
                    o->oOpacity += 4;
                    o->oBobombBuddyPosZCopy = curPoint[3 - o->oOpacity];
                    o->oBobombBuddyPosXCopy = curPoint[1 - o->oOpacity];
                } else {
                    if (o->oDistanceToMario > 1000.f) {
                        o->oAction = 0;
                    }
                }
            }
            break;
        case 2:
            // jump
            cur_obj_init_animation(2);
            targetAngle =
                atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0080 * runSpeed);
            o->oForwardVel = approach_f32_symmetric(
                o->oForwardVel, 1.2f * runSpeed * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 2.f);
            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                o->oAction = 1;
            }
            break;
        case 3:
            // fall and wait in the pit
            cur_obj_init_animation(3);
            if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
                if (o->oForwardVel > 1.f) {
                    cur_obj_play_sound_2(SOUND_ENV_SLIDING);
                    dust = spawn_object_relative(0, 0, 0, 240, o, MODEL_SMOKE, bhvDustBackwards);
                    dust->oMoveAngleYaw += 0x800;
                }
                o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0, 1.f);
            }
            break;
        case 4:
            // surprised
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 1;
            }
    }
}
#include "levels/hmc/header.h"
void yoshiDipshit1(void) {
    s16 targetAngle;
    o->oGraphYOffset = -105.f;
    if (!(o->oBehParams & 0x01)) {
        if (gCurrActNum == 1) {
            if (!o->oHiddenBlueCoinSwitch) {
                o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvFakeBush);
            }
            switch (o->oAction) {
                case 0:
                    cur_obj_init_animation(0);
                    if (o->oHiddenBlueCoinSwitch->oOpacity == 0x77) {
                        o->oAction = 3;
                        cur_obj_init_animation(1);
                        cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WAAH);
                    }
                    break;
                case 3:
                    // surprised
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oAction = 4;
                    }
                    break;
                case 4:
                    // run away
                    o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                    cur_obj_init_animation(2);
                    o->header.gfx.unk38.animFrame += o->oForwardVel / 7.f;
                    targetAngle = 0x9000;
                    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0xC00);
                    o->oForwardVel = approach_f32(
                        o->oForwardVel, 30.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 2.f, 2.f);
                    if (cur_obj_dist_to_nearest_object_with_behavior(bhvWarp) < 400.f) {
                        o->oOpacity++;
                    }
                    if (o->oOpacity > 8) {
                        obj_mark_for_deletion(o);
                    }
                    break;
            }
        } else {
            cur_obj_init_animation(0);
        }
    } else {
        dipshitFollowPath(0 + 18 * 4, 20.f, 1.f);
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

void yoshiDipshit2(void) {
    s16 targetAngle;
    if (!(o->oBehParams & 0x01)) {
        if (gCurrActNum == 1) {
            if (!o->oHiddenBlueCoinSwitch) {
                o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvFakeBush);
            }
            if (o->oAction < 3) {
                if (o->oHiddenBlueCoinSwitch->oOpacity == 0x77) {
                    o->oAction = 3;
                    cur_obj_init_animation(1);
                }
            }
            switch (o->oAction) {
                case 0:
                    cur_obj_init_animation(0);
                    if (o->oDistanceToMario < 1000.f) {
                        o->oAction = 1;
                    }
                    break;
                case 1:
                    if (talkToMario(115, 2)) {
                        o->oAction = 2;
                    }
                    break;
                case 2:
                    if (o->oDistanceToMario > 2000.f) {
                        o->oAction = 0;
                    }
                    break;
                case 3:
                    // surprised
                    set_mario_npc_dialog(1);
                    if (o->header.gfx.unk38.animFrame > 39) {
                        o->oAction = 4;
                    }
                    break;
                case 4:
                    // talk
                    if (o->header.gfx.unk38.animFrame > 59) {
                        o->header.gfx.unk38.animFrame -= 20;
                    }
                    if (talkToMario(116, 2)) {
                        o->oAction = 5;
                    }
                    set_mario_npc_dialog(1);
                    break;
                case 5:
                    // run away
                    set_mario_npc_dialog(1);
                    if (cur_obj_check_if_at_animation_end()) {
                        cur_obj_init_animation(2);
                    }
                    o->header.gfx.unk38.animFrame += o->oForwardVel / 7.f;
                    targetAngle = 0x9000;
                    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x800);
                    o->oForwardVel = approach_f32(
                        o->oForwardVel, 20.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 2.f, 2.f);
                    if (cur_obj_dist_to_nearest_object_with_behavior(bhvWarp) < 400.f) {
                        o->oOpacity++;
                    }
                    if (o->oOpacity > 8) {
                        obj_mark_for_deletion(o);
                    }
                    break;
            }
        } else {
            cur_obj_init_animation(0);
        }
    } else {
        if (!o->oBobombBuddyCannonStatus && yoshisRan) {
            cur_obj_init_animation(0);
            if (talkToMario(124, 1)) {
                o->oBobombBuddyCannonStatus = 1;
            }
        } else {
            dipshitFollowPath(0 + 16 * 4, 17.f, 1.2f);
        }
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

void yoshiDipshit3(void) {
    s16 targetAngle;
    if (!(o->oBehParams & 0x01)) {
        if (gCurrActNum == 1) {
            if (!o->oHiddenBlueCoinSwitch) {
                o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvDipshit2);
            }
            switch (o->oAction) {
                case 0:
                    if (o->oHiddenBlueCoinSwitch->oAction == 5) {
                        o->oAction = 3;
                    }
                    cur_obj_init_animation(0);
                    break;
                case 3:
                    // look at the boss, advance once he starts running
                    o->oMoveAngleYaw = obj_angle_to_object(o, o->oHiddenBlueCoinSwitch);
                    cur_obj_init_animation(1);
                    if (o->oTimer > 40) {
                        o->oAction = 4;
                    }
                    break;
                case 4:
                    // walk slowly
                    if (cur_obj_check_if_at_animation_end()) {
                        cur_obj_init_animation(2);
                    }
                    o->header.gfx.unk38.animFrame += o->oForwardVel / 7.f;
                    targetAngle = 0x9000;
                    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x400);
                    o->oForwardVel = approach_f32(
                        o->oForwardVel, 9.5f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 2.f, 2.f);
                    if (o->oTimer > 40) {
                        set_mario_npc_dialog(0);
                    }
                    if (cur_obj_dist_to_nearest_object_with_behavior(bhvWarp) < 400.f) {
                        o->oOpacity++;
                    }
                    if (o->oOpacity > 8) {
                        obj_mark_for_deletion(o);
                    }
                    break;
            }
        } else {
            cur_obj_init_animation(0);
        }
    } else {
        dipshitFollowPath(0 + 3 * 4, 9.5f, 1.1f);
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

void crackedEgg(void) {
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        create_sound_spawner(SOUND_GENERAL_YOSHI_WALK);
        spawn_mist_particles_variable(0, 0, 23.f);
        spawn_triangle_break_particles(15, 138, 2.0f, 4);
        spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
        obj_mark_for_deletion(o);
    }
}

void spawn_default_star(f32 sp20, f32 sp24, f32 sp28);
void dipshitStar(void) {
    if (!o->oHiddenBlueCoinSwitch) {
        o->oObjPointer1 = cur_obj_nearest_object_with_behavior(bhvDipshit1);
        o->oObjPointer2 = cur_obj_nearest_object_with_behavior(bhvDipshit2);
        o->oObjPointer3 = cur_obj_nearest_object_with_behavior(bhvDipshit3);
    }
    if (o->oAction == 1) {
        if (o->oTimer == 30) {
            o->oAction = 2;
        }
    } else if (o->oAction == 2) {
        adjustBankPitch(3, -30.f);
        if (talkToMario(123, 1)) {
            o->oAction = 3;
        }
    } else if (o->oAction == 3) {
        adjustBankPitch(3, 0);
        spawn_default_star(574.f, 0.f, 5000.f);
        obj_mark_for_deletion(o);
    } else {
        if (o->oObjPointer1->oPosY < -1000.f) {
            if (o->oObjPointer2->oPosY < -1000.f) {
                if (o->oObjPointer3->oPosY < -1000.f) {
                    o->oAction = 1;
                }
            }
        }
    }
}

extern Gfx mat_bronchiosaurus_f3d_material_012_layer1[];
void blinkBronto(void) {
    u8 *copyThis = 0;
    u8 *copyThisTex = 0;
    switch (o->oAnimState) {
        case 0:
            copyThis = bronchiosaurus__37a6c000_fmt0431_slice0_mip00_128x128_tm04_ci4_pal_rgba16;
            copyThisTex = bronchiosaurus__37a6c000_fmt0431_slice0_mip00_128x128_tm04_ci4;
            break;
        case 1:
            copyThis = bronchioeyes__37a78000_fmt0431_slice0_mip00_128x128_tm04_ci4_pal_rgba16;
            copyThisTex = bronchioeyes__37a78000_fmt0431_slice0_mip00_128x128_tm04_ci4;
            break;
        case 2:
            copyThis = bronchioeyes__37a70000_fmt0431_slice0_mip00_128x128_tm04_ci4_pal_rgba16;
            copyThisTex = bronchioeyes__37a70000_fmt0431_slice0_mip00_128x128_tm04_ci4;
            break;
    }
    ((Gfx *) segmented_to_virtual(&mat_bronchiosaurus_f3d_material_012_layer1[7]))->words.w1 = copyThis;
    ((Gfx *) segmented_to_virtual(&mat_bronchiosaurus_f3d_material_012_layer1[14]))->words.w1 =
        copyThisTex;
}

void bronchiocode(void) {
    s16 x, y, z;
    cur_obj_init_animation(0);
    if (!o->oTimer) {
        o->header.gfx.unk38.animFrame = random_u16() % 600;
        o->oSubAction = o->oFaceAngleYaw;
    }
    bowserBlink(&o->oBobombBlinkTimer);
    x = o->oFaceAnglePitch;
    y = o->oFaceAngleYaw;
    z = o->oFaceAngleRoll;
    o->oFaceAnglePitch = o->header.gfx.unk38.animRotStack[1][0];
    o->oFaceAngleYaw = o->header.gfx.unk38.animRotStack[1][1] + o->oSubAction;
    o->oFaceAngleRoll = o->header.gfx.unk38.animRotStack[1][2];
    o->oAngleVelPitch = o->oFaceAnglePitch - x;
    o->oAngleVelYaw = o->oFaceAngleYaw - y;
    o->oAngleVelRoll = o->oFaceAngleRoll - z;
    load_object_collision_model();
    blinkBronto();
    if (SURFACETYPE(gMarioState->floor) == 0x0021) {
        if (absf(gMarioState->pos[1] - gMarioState->floorHeight) < 100.f) {
            gMarioState->health--;
        }
    }
}

void sinkRocks(void) {
    if (!o->oOpacity) {
        o->oOpacity = 1;
        // spawn a rock
        o->oBehParams2ndByte = (random_u16() % 3);
        cur_obj_set_model(0x35 + o->oBehParams2ndByte);
        o->collisionData = segmented_to_virtual(collPointers[o->oBehParams2ndByte]);
        o->oFaceAngleYaw = random_u16();
        o->header.gfx.scale[0] = 1.5f + random_f32_around_zero(0.4f);
        o->header.gfx.scale[1] = 1.f + random_f32_around_zero(0.4f);
        o->header.gfx.scale[2] = 1.5f + random_f32_around_zero(0.4f);
        o->oPosY -= 25.f;
        o->oTimer = random_u16() & 0x7FFF;
    }
    o->oPosY += sins(o->oTimer * 0x400) * 2.f;
    load_object_collision_model();
}

void spawn_boulder_particles() {
    struct Object *particle;
    s32 i;
    f32 scale;
    s32 curAngle = 0;

    for (i = 0; i < 8; i++) {
        scale = random_float() * (.1f) + 1.f;

        particle = spawn_object(o, 0x4B, bhvParticleEffect);
        particle->oMoveAngleYaw = o->oMoveAngleYaw;
        particle->oFaceAngleYaw = random_u16();
        particle->oFaceAnglePitch = random_u16();
        particle->oGravity = -4.f;

        particle->oPosY += 250.f + random_float() * 150.f;
        particle->oPosX += random_float() * 150.f;
        particle->oPosZ += random_float() * 150.f;

        particle->oForwardVel = random_float() * 6.f + 9.f;
        particle->oVelY = random_float() * 15.f + 10.f;
        obj_scale_xyz(particle, scale, scale, scale);
    }
}
struct Object *TREX;
void boulderbulkan(void) {
    s16 targetAngle;
    switch (o->oAction) {
        case 0:
            if (o->oBehParams & 1) {
                if (o->oOpacity == 0) {
                    spawn_object_relative(0, random_f32_around_zero(1000.f), 0, 0, o, 0x4A,
                                          bhvVulcanBoulder);
                    o->oOpacity = 50 + random_u16() % 60;
                } else {
                    o->oOpacity--;
                }
            } else {
                if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                    o->oAction = 1;
                    o->oForwardVel = 20.f;
                    targetAngle = (random_u16() & 0x1FFF) - 0x1000;
                    o->oMoveAngleYaw += targetAngle;
                    o->oAngleVelPitch = 834;
                    o->oAngleVelYaw = -targetAngle / 512;
                }
            }
            break;
        case 1:
            if (o->oTimer > 200) {
                if ((o->oMoveFlags & OBJ_MOVE_HIT_WALL)
                    || (lateral_dist_between_objects(o, TREX) < 750.f)) {
                    obj_mark_for_deletion(o);
                    spawn_boulder_particles();
                    spawn_triangle_break_particlesOffset(30, 138, 4.0f, 4, 0.f);
                    cur_obj_play_sound_2(SOUND_GENERAL_POUND_ROCK2);
                }
            }
            break;
        case 2:

            break;
    }
    if (!(o->oBehParams & 1)) {
        if (o->oFloorPointer) {
            if (SURFACETYPE(o->oFloorPointer) == 1) {
                o->oGraphYOffset = approach_f32_symmetric(o->oGraphYOffset, 200.f, 2.f);
            } else {
                o->oGraphYOffset = approach_f32_symmetric(o->oGraphYOffset, 250.f, 4.f);
            }
        }
        o->oFaceAnglePitch += o->oAngleVelPitch;
        o->oMoveAngleYaw += o->oAngleVelYaw;
        o->oMoveFlags &= ~(OBJ_MOVE_ABOVE_LAVA | OBJ_MOVE_ABOVE_DEATH_BARRIER);

        if (o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
            cur_obj_update_floor();
            o->oMoveFlags &= ~OBJ_MOVE_MASK_HIT_WALL_OR_IN_WATER;

            if (o->oPosY > o->oFloorHeight) {
                o->oMoveFlags |= OBJ_MOVE_IN_AIR;
            }
        } else {
            o->oMoveFlags &= ~OBJ_MOVE_HIT_WALL;
            if (f32_find_wall_collision(&o->oPosX, &o->oPosY, &o->oPosZ, 100.f, 250.f)) {
                o->oMoveFlags |= OBJ_MOVE_HIT_WALL;
            }

            cur_obj_update_floor();

            if (o->oPosY > o->oFloorHeight) {
                o->oMoveFlags |= OBJ_MOVE_IN_AIR;
            }
        }
        cur_obj_move_standard(78);
    }
}
extern Gfx mat_bronchiosaurus_f3d_material_012_layer1[];
void blinkTrex(void) {
    u8 *copyThis = 0;
    switch (o->oAnimState) {
        case 0:
            copyThis = trex_dinoeyeeeeee_rgba16;
            break;
        case 1:
            copyThis = trexEyes_dinoeyeeeeeehalf_rgba16;
            break;
        case 2:
            copyThis = trexEyes_dinoeyeeeeeeclosed_rgba16;
            break;
    }
    ((Gfx *) segmented_to_virtual(&mat_trex_EyeOpen_layer1[7]))->words.w1 = copyThis;
}

Gfx *geo_trex_head_movement(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c) {
    struct Object *obj = gCurGraphNodeObject;
    struct GraphNodeGenerated *asGenerated = (struct GraphNodeGenerated *) node;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct GraphNodeTranslationRotation *rotNode =
            (struct GraphNodeTranslationRotation *) node->next;
        if (asGenerated->parameter) {
            rotNode->rotation[0] = obj->oBubbaUnkF8;
            rotNode->rotation[1] = obj->oBubbaUnkFC;
            rotNode->rotation[2] = 0;
        } else {
            rotNode->rotation[0] = obj->oBubbaUnk100;
            rotNode->rotation[1] = 0;
            rotNode->rotation[2] = 0;
        }
    }
    return NULL;
}
extern u16 newcam_distance;
extern u16 newcam_distance_target;
extern int timerbeforerotation;
extern int timerbeforeminmovement;
extern u8 newcam_centering;
extern s16 newcam_pitch;
extern s16 newcam_yaw_target;
struct Surface *resolve_and_return_wall_collisions(Vec3f pos, f32 offset, f32 radius);
s16 trexPrevTilt;

s32 cur_obj_play_footstep_sounds(s32 animFrame1, s32 animFrame2, s32 sound) {
    if (o->header.gfx.unk38.animFrame > animFrame1 && !(o->header.gfx.unk38.curAnimFlags & 1)) {
        o->header.gfx.unk38.curAnimFlags |= 1;
        cur_obj_play_sound_2(sound);
        return 1;
    } else if (o->header.gfx.unk38.animFrame > animFrame2 && !(o->header.gfx.unk38.curAnimFlags & 2)) {
        o->header.gfx.unk38.curAnimFlags |= 2;
        cur_obj_play_sound_2(sound);
        return 1;
    } else if (o->header.gfx.unk38.animFrame < animFrame1) {
        o->header.gfx.unk38.curAnimFlags = 0;
        return 0;
    }
}

extern const Trajectory hmc_area_4_spline_TREXPOINTS[];
void trexcode(void) {
    s16 watchYaw = 0;
    s16 watchPitch = 0;
    s16 openNess = 0;
    f32 goalSpeed;
    f32 castDir[3];
    f32 castPost[3];
    struct Surface *hit_surface;
    f32 hit_pos[3];
    s16 camGoal = newcam_yaw;
    s16 goalYaw;
    f32 x, z;
    struct Object *spawn;
    struct WallCollisionData collisionData;
    bowserBlink(&o->oBobombBlinkTimer);
    switch (o->oAction) {
        case 0:
            TREX = o;
            // stand around
            // watch left and right
            o->oForwardVel = approach_f32(o->oForwardVel, 0, 1.f, 2.f);
            cur_obj_init_animation(0);
            watchYaw = approach_s16_symmetric(o->oBubbaUnkFC, sins(o->oTimer * 0x200) * 0x1700, 0x800);
            if (o->oBubbaUnk104) {
                if (random_u16() > 0xFD40) {
                    o->oAction = 1;
                }
            }
            break;
        case 1:
            // walk to random place on spline
            // move watchyaw into movement goal direction ahead
            cur_obj_init_animation(3);
            if (!o->oTimer) {
                o->oSubAction = random_u16() % 5;
            }
            x = ((s16 *) segmented_to_virtual(0))[o->oSubAction * 4 + 1];
            z = ((s16 *) segmented_to_virtual(0))[o->oSubAction * 4 + 3];
            goalYaw = atan2s(o->oPosZ - z, o->oPosX - x) + 0x8000;
#define TREXSPEED (2.f * (sqrtf(sqr(o->oPosZ - z) + sqr(o->oPosX - x)) + 2000.f) / 1000.f)
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, goalYaw, 0x300);
            if (sqrtf(sqr(o->oPosZ - z) + sqr(o->oPosX - x))
                > 500.f + o->oForwardVel / 8.f * (o->oForwardVel + 1)) {
                o->oForwardVel = approach_f32(
                    o->oForwardVel, TREXSPEED * (coss(o->oMoveAngleYaw - goalYaw) + 1.0f), 1.f, 2.5f);
                o->header.gfx.unk38.animFrame += o->oForwardVel / 10.f;
#define TREXSTEPSOUND SOUND_OBJ_BOWSER_WALK
                if (cur_obj_play_footstep_sounds(114, 213, TREXSTEPSOUND)) {
                    set_environmental_camera_shake(SHAKE_ENV_UNUSED_5);
                }
                if (o->header.gfx.unk38.animFrame >= 214) {
                    o->header.gfx.unk38.animFrame -= 200;
                }
            } else {
                o->oForwardVel = approach_f32(o->oForwardVel, 0, 1.f, 2.f);
                if (cur_obj_check_if_at_animation_end()) {
                    o->oAction = 0;
                }
            }
            break;
        case 2:
            // notice mario. (happens after mario has been out of a hole for X frames. he will turn
            // towards mario, make an animation, then go after him)
            // if (!o->oBubbaUnk104) {
            watchPitch = -0x2000;
            openNess = 0x4000;
            watchYaw = sins(o->oTimer * 1100) * 0x1500;
            o->oForwardVel = 0;
            //  }
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x800);
            if (o->oTimer > 40) {
                if (!o->oBubbaUnk104) {
                    newcam_pitch = approach_s16_symmetric(newcam_pitch, trexPrevTilt, 0x800);
                }
                watchYaw = o->oAngleToMario - o->oMoveAngleYaw;
                watchPitch =
                    atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario) - 0x4000;
                if (o->oTimer == 50) {
                    o->oAction = 3;
                    cur_obj_init_animation(3);
                    o->oBubbaUnk104 = 1;
                }
            } else {
                set_environmental_camera_shake(SHAKE_ENV_FALLING_BITS_PLAT);
            }
            break;
        case 3:
            // chase after mario
            // if close, lean low, bite at mario
            // if mario in hole, walk up to hole, look inside the whole, sniff it
            //       cur_obj_init_animation(2);
            cur_obj_init_animation(3);
#define TREXSPEED 10.f * ((o->oTimer + 100.f) / 100.f)
            o->oForwardVel =
                approach_f32(o->oForwardVel,
                             TREXSPEED * (coss(o->oAngleToMario - o->oMoveAngleYaw) + 1.0f), 1.f, 2.5f);
            o->header.gfx.unk38.animFrame += o->oForwardVel / 10.f;
            if (cur_obj_play_footstep_sounds(114, 213, TREXSTEPSOUND)) {
                set_environmental_camera_shake(SHAKE_ENV_UNUSED_5);
            }
            if (o->header.gfx.unk38.animFrame >= 214) {
                o->header.gfx.unk38.animFrame -= 200;
            }
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario,
                                                      0x200 * ((o->oTimer + 100.f) / 100.f));
            watchYaw = o->oAngleToMario - o->oMoveAngleYaw;
            watchPitch = atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario) - 0x4000;
            if (gMarioState->ceilHeight < gMarioState->pos[1] + 500.f) {
                o->oAction = 4;
            }
            if (lateral_dist_between_objects(o, gMarioState->marioObj)
                < 2000.f + o->oForwardVel * 4.f) {
                watchPitch =
                    atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario) - 0x7000;
                openNess = 0x3000;
                if (lateral_dist_between_objects(o, gMarioState->marioObj)
                    < 1100.f + o->oForwardVel * 4.f) {
                    o->oAction = 6;
                    // calculate jump, animate
                    cur_obj_init_animation(5);
                    cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_BITE);
                    o->oMoveAngleYaw = o->oAngleToMario;
                }
            }
            if ((gMarioState->floorHeight > 1115.f)
                || (gMarioState->floorHeight > 725.f && gMarioState->pos[2] < -8000.f)) {
                o->oAction = 5;
            }
            break;
        case 4:
            // sniff hole, go back home after a bit
            newcam_centering = 1;
            camGoal = o->oAngleToMario + 0x8000;
            if (gMarioState->pos[0] < 0.f) {
                camGoal += 0x2000;
            } else {
                camGoal -= 0x2000;
            }
            newcam_yaw = approach_s16_symmetric(newcam_yaw, camGoal, 0xC00);
            newcam_yaw_target = newcam_yaw;
            timerbeforerotation = -10;
            timerbeforeminmovement = -10;
            newcam_distance = 2500;
            newcam_distance_target = 2500;

            switch (o->oSubAction) {
                case 0:
                    goalSpeed =
                        (5.f * ((lateral_dist_between_objects(o, gMarioState->marioObj)) / 1000.f)
                         * (lateral_dist_between_objects(o, gMarioState->marioObj)) / 1000.f);
                    o->oForwardVel = approach_f32(
                        o->oForwardVel, goalSpeed * (coss(o->oAngleToMario - o->oMoveAngleYaw) + 1.0f),
                        1.f, 2.5f);
                    o->header.gfx.unk38.animFrame += o->oForwardVel / 10.f;
                    if (o->header.gfx.unk38.animFrame >= 214) {
                        o->header.gfx.unk38.animFrame -= 200;
                    }
                    o->oMoveAngleYaw =
                        approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
                    watchYaw = o->oAngleToMario - o->oMoveAngleYaw;
                    watchPitch =
                        atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario) - 0x4000;
                    if (lateral_dist_between_objects(o, gMarioState->marioObj)
                        < 2000.f + o->oForwardVel / 8.f * (o->oForwardVel + 1)) {
                        o->oSubAction = 1;
                    }
                    if ((m->ceilHeight - m->pos[1]) > 500.f) {
                        o->oAction = 3;
                    }
                    break;
                case 1:
                    o->oForwardVel = approach_f32(o->oForwardVel, 0, 1.f, 2.f);
                    o->header.gfx.unk38.animFrame += o->oForwardVel / 10.f;
                    if (o->header.gfx.unk38.animFrame < 200) {
                        o->header.gfx.unk38.animFrame += 1;
                    }
                    if (cur_obj_check_if_at_animation_end()) {
                        o->oSubAction = 2;
                    }
                    o->oMoveAngleYaw =
                        approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
                    watchYaw = o->oAngleToMario - o->oMoveAngleYaw;
                    watchPitch =
                        atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario) - 0x4000;
                    if ((m->ceilHeight - m->pos[1]) > 500.f) {
                        o->oAction = 3;
                    }
                    break;
                case 2:
                    o->oForwardVel = approach_f32(o->oForwardVel, 0, 1.f, 2.5f);
                    cur_obj_init_animation(4);
                    if (o->header.gfx.unk38.animFrame < 225) {
                        watchYaw = o->oAngleToMario - o->oMoveAngleYaw;
                        watchPitch =
                            atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario)
                            - 0x6400;
                        if ((m->ceilHeight - m->pos[1]) > 500.f) {
                            o->header.gfx.unk38.animFrame = 225;
                        }
                    }
                    if (o->header.gfx.unk38.animFrame < 210) {
                        if (((o->header.gfx.unk38.animFrame % 0x3F > 0x30)
                             && (o->header.gfx.unk38.animFrame % 0x3F < 0x35))
                            || (o->header.gfx.unk38.animFrame % 0x3F > 0x3A)) {
                            watchPitch =
                                atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario)
                                - 0x7400;
                            spawn = spawn_object_relative_with_scale(17, 0.f, 0.f, 1800.f, 0.5f, o,
                                                                     MODEL_WHITE_PARTICLE_DL,
                                                                     bhvDinoNoseParticle);
                            spawn->oMoveAngleYaw += 0x8000;
                            spawn->oMoveAnglePitch = watchPitch + 0x1000;
                            spawn->oFaceAnglePitch = watchPitch + 0x1000;
                            cur_obj_play_sound_2(SOUND_MOVING_TERRAIN_RIDING_SHELL);
                        }
                    }
                    if (cur_obj_check_if_at_animation_end()) {
                        if ((m->ceilHeight - m->pos[1]) > 500.f) {
                            o->oAction = 3;
                        } else {
                            o->oAction = 1;
                        }
                    }
                    break;
            }
            break;
        case 5:
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            o->oForwardVel = approach_f32(o->oForwardVel, 0, 1.f, 2.f);
            o->header.gfx.unk38.animFrame += o->oForwardVel / 10.f;
            if (o->header.gfx.unk38.animFrame < 200) {
                o->header.gfx.unk38.animFrame += 1;
            }
            if (cur_obj_check_if_at_animation_end()) {
                cur_obj_init_animation(0);
            }
            break;
        case 6:
            gMarioState->pos[0] = o->oPosX + sins(o->oMoveAngleYaw) * 1100.f;
            gMarioState->pos[1] = o->oPosY + 150.f;
            gMarioState->pos[2] = o->oPosZ + coss(o->oMoveAngleYaw) * 1100.f;
            if (o->oTimer < 30) {
                newcam_distance = 750 + o->oTimer * 30;
                newcam_distance_target = 750 + o->oTimer * 30;
            } else {
                newcam_distance = 1650;
                newcam_distance_target = 1650;
            }
            if (o->oForwardVel > .5f) {
                cur_obj_play_sound_2(SOUND_ENV_SLIDING);
                spawn = spawn_object_relative(0, 0, 0, 240, o, MODEL_SMOKE, bhvDustBackwards);
                spawn->oMoveAngleYaw += 0x8000;
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0, .5f);
            if (o->oTimer < 5) {
                o->oFaceAnglePitch = approach_s16_symmetric(
                    o->oFaceAnglePitch,
                    atan2s(-o->oPosY + gMarioState->floorHeight,
                           lateral_dist_between_objects(o, gMarioState->marioObj))
                        - 0x4000,
                    0x800);
            } else {
                openNess = (sins(o->oTimer * 0xB00) * 0x0800 + 0x500) * (200 - o->oTimer) / 200.f;
                watchPitch = -0x0C00;
            }
            if (o->oTimer == 4) {
                cur_obj_play_sound_2(TREXSTEPSOUND);
                set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
                m->interactObj = o;
                set_mario_action(m, ACT_EATEN_BY_BUBBA, 0);
            }
            break;
    }
#define NOTICETIME 120
    if ((o->oAction < 2) || (o->oAction == 5)) {
        if ((m->ceilHeight - m->pos[1]) > 500.f) {
            o->oOpacity++;
        }
        if (o->oOpacity > NOTICETIME) {
            if ((!o->oBubbaUnk104)
                || (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < (0x3000 + o->oTimer * 0x40))) {
                if ((gMarioState->floorHeight < 1115.f)
                    && !(gMarioState->floorHeight > 725.f && gMarioState->pos[2] < -8000.f)) {
                    o->oAction = 2;
                    cur_obj_play_sound_2(SOUND_TREX_ROAT);
                    set_environmental_camera_shake(SHAKE_ENV_FALLING_BITS_PLAT);
                    if (!o->oBubbaUnk104) {
                        newcam_centering = 1;
                        newcam_yaw = o->oAngleToMario;
                        newcam_yaw_target = o->oAngleToMario;
                        trexPrevTilt = newcam_pitch;
                        timerbeforerotation = -10;
                        timerbeforeminmovement = -10;
                        newcam_pitch =
                            atan2s(-o->oPosY + gMarioState->pos[1] - 1000.f, o->oDistanceToMario)
                            + 0x4000;
                    }
                    cur_obj_init_animation(1);
                }
            }
        }
    } else {
        o->oOpacity = 0;
    }
    o->oBubbaUnkFC = approach_s16_symmetric(o->oBubbaUnkFC, watchYaw, 0x200);
    if (o->oBubbaUnkFC > 0x2800) {
        o->oBubbaUnkFC = 0x2800;
    } else if (o->oBubbaUnkFC < -0x2800) {
        o->oBubbaUnkFC = -0x2800;
    }
    o->oBubbaUnkF8 = approach_s16_symmetric(o->oBubbaUnkF8, watchPitch, 0x200);
    if (o->oBubbaUnkF8 > 0x4000) {
        o->oBubbaUnkF8 = 0x4000;
    } else if (o->oBubbaUnkF8 < -0x4000) {
        o->oBubbaUnkF8 = -0x4000;
    }
    o->oBubbaUnk100 = approach_s16_symmetric(o->oBubbaUnk100, openNess, 0x400);
    collisionData.x = o->oPosX;
    collisionData.y = o->oPosY;
    collisionData.z = o->oPosZ;
    collisionData.radius = 1250.f;
    collisionData.numWalls = 0;
    collisionData.offsetY = 400.f;
    find_wall_collisions(&collisionData);
    o->oPosX = collisionData.x;
    o->oPosZ = collisionData.z;
    o->oFloorHeight = find_floor(o->oPosX, o->oPosY + 100.f, o->oPosZ, &o->oFloorPointer);
    o->oPosY = o->oFloorHeight;
    cur_obj_move_xz_using_fvel_and_yaw();
    // cur_obj_move_standard(78);
    blinkTrex();
}

void splashCode(void) {
    struct Surface *cur;
    struct Object *a;
    s32 i;
    for (i = 0; i < 5; i++) {
        cur = 0;
        o->oPosZ = o->oHomeZ - random_float() * 16358.f;
        o->oPosX = o->oHomeX - random_float() * 14435.f;
        o->oPosY = find_floor(o->oPosX, 2000.f, o->oPosZ, &cur);
        if (cur) {
            if (SURFACETYPE(cur) == 1) {
                a = spawn_object(o, MODEL_BURN_SMOKE, bhvBlackSmokeMario);
                a->oBehParams2ndByte = 2;
                a->oVelY = 8.f + random_float() * 5.f;
            }
        }
    }
}

/*
                OBJECT(MODEL_STAR, 6415, 1361, -8989, 0, -180, 0, (17 << 24), 1111),
                OBJECT(MODEL_STAR, 5962, 861, -10047, 0, -180, 0, (17 << 24), 222),
                OBJECT(MODEL_STAR, 7104, 1111, -9910, 0, -180, 0, (17 << 24), 333),
                OBJECT(MODEL_STAR, 5509, 361, -11105, 0, -180, 0, (17 << 24), 444),
                OBJECT(MODEL_STAR, 6652, 611, -10968, 0, -180, 0, (17 << 24), 555),
                OBJECT(MODEL_STAR, 5272, -139, -9126, 0, -180, 0, (17 << 24), 666),
                OBJECT(MODEL_STAR, 4820, 111, -10184, 0, -180, 0, (17 << 24), 777),*/

f32 towerPos[7] = { 1361.f, 861.f, 1111.f, 361.f, 611.f, -139.f, 111.f

};
void goombastar(void) {
    if ((o->oBehParams & 0xFF) == 7) {
        if (gMarioState->pos[1] == gMarioState->floorHeight) {
            if (SURFACETYPE(gMarioState->floor) == 0xE0) {
                if (absf(gMarioState->floorHeight - towerPos[o->oOpacity]) < 50.f) {
                    o->oOpacity++;
                    if (o->oOpacity == 7) {
                        obj_mark_for_deletion(o);
                        spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f,
                                           gMarioState->pos[2]);
                    }
                } else if (absf(gMarioState->pos[1] - towerPos[o->oOpacity - 1]) > 50.f) {
                    o->oOpacity = 0;
                }
            } else {
                if (!gMarioState->floor->object) {
                    o->oOpacity = 0;
                }
            }
        }
    } else {
        if (!o->oBehParams2ndByte) {
            if (!cur_obj_nearest_object_with_behavior(bhvGoomba)
                && (!cur_obj_nearest_object_with_behavior(bhvGoombaTripletSpawner))) {
                obj_mark_for_deletion(o);
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f,
                                   gMarioState->pos[2]);
            }
        } else {
            if (!cur_obj_nearest_object_with_behavior(bhvChuckya)) {
                obj_mark_for_deletion(o);
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f,
                                   gMarioState->pos[2]);
            }
        }
    }
}

void scaleByParam2(void) {
    cur_obj_scale(o->oBehParams2ndByte / 100.f + 1.f);
}

void goround(void) {
    if (!o->oAction) {
        o->oOpacity = obj_angle_to_object(o, gMarioState->marioObj);
        if (cur_obj_is_mario_on_platform()) {
            o->oAction = 1;
        }
    } else {
        if ((abs(o->oAngleVelYaw) > 70) && !o->oMacroUnk10C) {
            spawn_object(o, MODEL_SPARKLES, bhvSparkleParticleSpawner);
            if (o->oTimer % 8 == 0) {
                cur_obj_play_sound_1(SOUND_GENERAL_BOAT_ROCK);
            }
        }
        o->oAngleVelYaw -= ((s16) ((obj_angle_to_object(o, gMarioState->marioObj) - o->oOpacity))) / 10;
        o->oOpacity = obj_angle_to_object(o, gMarioState->marioObj);
        if (!cur_obj_is_mario_on_platform()) {
            o->oAction = 0;
        }
        if (abs(o->oMacroUnk108) > 0x8000) {
            if (!o->oMacroUnk10C) {
                o->oMacroUnk10C = 1;
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f,
                                   gMarioState->pos[2]);
            }
        }
    }
    if (abs(o->oMacroUnk108) > 0x8000) {
        o->oPosY = 66.f + o->oHomeY;
    } else {
        o->oPosY = abs(o->oMacroUnk108) / 500.f + o->oHomeY;
    }
    o->oAngleVelYaw = approach_s16_asymptotic(o->oAngleVelYaw, 0, 3);
    o->oFaceAngleYaw += o->oAngleVelYaw;
    o->oMacroUnk108 += o->oAngleVelYaw;
    load_object_collision_model();
}

void bbarrelcode(void) {
    if (o->oAction == 0) {
        if (!o->oTimer) {
            o->oFaceAngleYaw = random_u16();
        }
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

void wingmillcode(void) {
    if (!o->oTimer) {
        cur_obj_scale(o->oBehParams2ndByte / 100.f);
    }
    o->oFaceAnglePitch += 0x100 - o->oBehParams2ndByte / 8;
    load_object_collision_model();
}

extern struct CutsceneJump *currentScene;
void peachcode(void) {
    cur_obj_init_animation(0);
    if (currentScene) {

    } else {
        if (gMarioState->numStars >= 25) {
            o->oBehParams2ndByte = 8;
        }
        o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
        if (o->oAction == 3) {
            if (talkToMario(o->oBehParams2ndByte, 4)) {
                o->oAction = 2;
                if (o->oBehParams2ndByte == 8) {
                    // wapr to credits
                    level_trigger_warp(m, WARP_OP_WARP_FLOOR);
                    play_transition(WARP_TRANSITION_FADE_INTO_STAR, 0x30, 0x00, 0x00, 0x00);
                    sDelayedWarpTimer = 48;
                    sSourceWarpNodeId = 8;
                }
            }
        } else if (o->oAction == 2) {
            if (o->oDistanceToMario > 550.f) {
                o->oAction = 0;
            }
        } else {
            if (o->oDistanceToMario < 400.f) {
                o->oAction = 3;
            }
        }
    }
}

extern f32 gDefaultSoundArgs[3];

s32 lvl_play_the_end_screen_sound(UNUSED s16 arg0, UNUSED s32 arg1) {
    play_sound(SOUND_MENU_THANK_YOU_PLAYING_MY_GAME, gDefaultSoundArgs);
    return 1;
};

void camforce(void) {
    if (gMarioState->pos[2] > o->oPosZ) {
        if (timerbeforeminmovement > 20) {
            newcam_centering = 1;
            newcam_yaw = 0x4000;
            newcam_yaw_target = 0x4000;
        }
    }
}