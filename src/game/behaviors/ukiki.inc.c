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
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject

/**
 * Turns ukiki to face towards Mario while moving with slow forward velocity.
 */
void ukiki_act_turn_to_mario(void) {
    s32 facingMario;
    if (o->oTimer == 0) {
        o->oForwardVel = random_float() * 3.0f + 2.0f;
    }
    cur_obj_init_animation_with_sound(UKIKI_ANIM_TURN);
    facingMario = cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x800);
    if (facingMario) {
        o->oAction = UKIKI_ACT_IDLE;
    }
    if (o->oDistanceToMario < 300.0f) {
        o->oAction = UKIKI_ACT_RUN;
    }
}

void ukiki_act_run(void) {
    s16 goalYaw = o->oAngleToMario + 0x8000;
    if (o->oTimer == 0) {
        o->oUkikiChaseFleeRange = random_float() * 100.0f + 350.0f;
    }
    cur_obj_init_animation_with_sound(UKIKI_ANIM_RUN);
    cur_obj_rotate_yaw_toward(goalYaw, 0x800);
    cur_obj_set_vel_from_mario_vel(20.0f, 0.9f);
    if (o->oDistanceToMario > o->oUkikiChaseFleeRange) {
        o->oAction = UKIKI_ACT_TURN_TO_MARIO;
    }
    if (o->oDistanceToMario < 200.0f) {
        if ((o->oMoveFlags & OBJ_MOVE_HIT_WALL) && is_mario_moving_fast_or_in_air(10)) {
            o->oAction = UKIKI_ACT_JUMP;
            o->oMoveAngleYaw = o->oWallAngle;
        } else if ((o->oMoveFlags & OBJ_MOVE_HIT_EDGE)) {
            if (is_mario_moving_fast_or_in_air(10)) {
                o->oAction = UKIKI_ACT_JUMP;
                o->oMoveAngleYaw += 0x8000;
            }
        }
    }
}

f32 distToGoal() {
    f32 x, y, z;
    x = o->oPosX - o->oBobombBuddyPosXCopy;
    y = o->oPosY - o->oBobombBuddyPosYCopy;
    z = o->oPosZ - o->oBobombBuddyPosZCopy;
    return sqrtf(x * x + y * y + z * z);
}
f32 lateraldistToGoal() {
    f32 x, z;
    x = o->oPosX - o->oBobombBuddyPosXCopy;
    z = o->oPosZ - o->oBobombBuddyPosZCopy;
    return sqrtf(x * x + z * z);
}
u16 angleToGoal() {
    return atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
}
u32 checkMonkFloor() {
    struct Surface *floor;
#define monkedist o->oForwardVel * 10.f
    f32 floorHeight = find_floor(o->oPosX + sins(o->oMoveAngleYaw) * monkedist, o->oPosY + 50.f,
                                 o->oPosZ + coss(o->oMoveAngleYaw) * monkedist, &floor);
    if (absf(floorHeight - o->oPosY) > 300.f) {
        return 1;
    } else {
        return 0;
    }
}

void ukiki_act_jump(void) {
    struct Object *vines = cur_obj_nearest_object_with_behavior(bhvPoleGrabbing);
    if (lateral_dist_between_objects(vines, o) < 100.f) {
        if (vines->oPosY > o->oPosY) {
            cur_obj_set_y_vel_and_animation(80.0f, UKIKI_ANIM_JUMP); // calculate from goalpos
            o->oForwardVel *= 1.25f;
            o->header.gfx.unk38.animFrame = 0;
        }
    }
    if (lateral_dist_between_objects(vines, o) < 600.f) {
        o->oVelY += 1.5f;
    }
    cur_obj_become_intangible();
    if (o->oSubAction == 0) {
        if (o->oTimer == 0) {
            if (o->oBehParams2ndByte) {
                o->oBehParams2ndByte = 0;
                cur_obj_set_y_vel_and_animation(80.0f, UKIKI_ANIM_JUMP); // calculate from goalpos
            } else {
                cur_obj_set_y_vel_and_animation(45.0f, UKIKI_ANIM_JUMP);
                o->oForwardVel = 20.0f;
            }
        } else if (o->oMoveFlags & OBJ_MOVE_MASK_NOT_AIR) {
            o->oSubAction++;
            o->oVelY = 0.0f;
        }
    } else {
        o->oForwardVel = 0.0f;
        cur_obj_init_animation_with_sound(UKIKI_ANIM_LAND);
        cur_obj_become_tangible();
        if (cur_obj_check_if_near_animation_end()) {
            o->oAction = o->oBobombBuddyRole;
        }
    }
}
struct SoundState sUkikiSoundStates[] = {
    { 1, 1, 10, SOUND_OBJ_UKIKI_STEP_DEFAULT },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 0, -1, SOUND_OBJ_UKIKI_CHATTER_SHORT },
    { 1, 0, -1, SOUND_OBJ_UKIKI_CHATTER_LONG },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 0, -1, SOUND_OBJ_UKIKI_CHATTER_LONG },
    { 1, 0, -1, SOUND_OBJ_UKIKI_STEP_LEAVES },
    { 1, 0, -1, SOUND_OBJ_UKIKI_CHATTER_IDLE },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
};
u8 animIDs[] = { 0, UKIKI_ANIM_ITCH, UKIKI_ANIM_SCREECH, UKIKI_ANIM_JUMP_CLAP, UKIKI_ANIM_HANDSTAND };
void idle_ukiki_taunt(void) {
    if (o->oSubAction == UKIKI_SUB_ACT_TAUNT_NONE) {
        o->oSubAction = (s32)(random_float() * 4.0f + 1.0f);
        o->oUkikiTauntCounter = 0;
        o->oUkikiTauntsToBeDone = (s16)(random_float() * 4.0f + 2.0f);
    }
    switch (o->oSubAction) {
        case UKIKI_SUB_ACT_TAUNT_SCREECH:
            if (o->oDistanceToMario < 1000.f) {
                cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), 0x200);
            }
            if (cur_obj_check_if_near_animation_end()) {
                o->oUkikiTauntCounter++;
            }
            if (o->oUkikiTauntCounter >= o->oUkikiTauntsToBeDone * 2) {
                o->oSubAction = UKIKI_SUB_ACT_TAUNT_NONE;
            }
            break;
        case UKIKI_SUB_ACT_TAUNT_JUMP_CLAP:
            if (cur_obj_check_if_near_animation_end()) {
                o->oUkikiTauntCounter++;
            }
            if (o->oUkikiTauntCounter >= o->oUkikiTauntsToBeDone) {
                o->oSubAction = UKIKI_SUB_ACT_TAUNT_NONE;
            }
            break;
        case UKIKI_SUB_ACT_TAUNT_ITCH:
        case UKIKI_SUB_ACT_TAUNT_HANDSTAND:
            if (cur_obj_check_if_near_animation_end()) {
                o->oSubAction = UKIKI_SUB_ACT_TAUNT_NONE;
            }
            break;
    }
    cur_obj_init_animation_with_sound(animIDs[o->oSubAction]);
}
void ukiki_act_idle(void) {
    o->oForwardVel = approach_f32(o->oForwardVel, 0.f, 0.5f, 2.f);
    idle_ukiki_taunt();
    o->oAction = (o->oDistanceToMario < 300.0f);
    if (o->oTimer > 100) {
        if (distToGoal() > 310.f) {
            o->oAction = 4;
        }
        if (o->oPosY < -535.f) {
            if (!(random_u16() & 0x7f)) {
                o->oAction = 5;
            }
        }
    }
}
extern struct WaterDropletParams sWaterSplashDropletParams;
void ukiki_act_play(void) {
    u32 i;
    if (!o->oSubAction) {
        cur_obj_init_animation_with_sound(UKIKI_ANIM_JUMP);
        if (o->header.gfx.unk38.animFrame == 0) {
            o->oVelY = 50.f;
            spawn_object(o, MODEL_WATER_SPLASH, bhvWaterSplash);
            for (i = 0; i < 3; i++) {
                spawn_water_droplet(o, &sWaterSplashDropletParams);
            }
            o->oSubAction++;
        }
    } else {
        if (o->oMoveFlags & OBJ_MOVE_MASK_NOT_AIR) {
            o->oVelY = 0.0f;
            o->oForwardVel = 0.0f;
            cur_obj_init_animation_with_sound(UKIKI_ANIM_LAND);
            if (cur_obj_check_if_near_animation_end()) {
                o->oSubAction = 0;
                if (!(random_u16()&7)){
                    o->oAction = 0;
                }
            }
        }
    }
}
#define HIGHPOINTCOUNT 3
#define MIDPOINTCOUNT 4
#define LOWPOINTCOUNT 9
u8 pointcounts[] = { HIGHPOINTCOUNT, MIDPOINTCOUNT, LOWPOINTCOUNT };
extern const Vec4s ccm_area_1_spline_MonkeyHighPoints[];
extern const Vec4s ccm_area_1_spline_MonkeyLowPoints[];
extern const Vec4s ccm_area_1_spline_MonkeyMidPoints[];
Vec4s *pointarrays[] = { &ccm_area_1_spline_MonkeyHighPoints, &ccm_area_1_spline_MonkeyMidPoints,
                         &ccm_area_1_spline_MonkeyLowPoints };
void ukiki_act_go_to_spot(void) {
    f32 speegoal = 7.5f;
    u16 targetAngle = angleToGoal();
    cur_obj_init_animation_with_sound(UKIKI_ANIM_RUN);
    if (distToGoal() < o->oForwardVel * 40.f) {
        speegoal = 2.f;
    }
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x300);
    o->oForwardVel = approach_f32(o->oForwardVel,
                                  speegoal * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                      * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f),
                                  0.5f, 2.f);
    if (checkMonkFloor()) {
        o->oMoveAngleYaw = angleToGoal();
        o->oAction = UKIKI_ACT_JUMP;
        o->oBehParams2ndByte = 1;
    }
    if (lateraldistToGoal() < o->oForwardVel * 40.f) {
        if (o->oPosY < (o->oBobombBuddyPosYCopy - 250.f)) {
            o->oMoveAngleYaw = angleToGoal();
            o->oAction = UKIKI_ACT_JUMP;
            o->oBehParams2ndByte = 1;
        }
    } else {
        if (lateraldistToGoal() < 300.f) {
            o->oAction = 0;
        }
    }
}
void ukiki_act_climb_vine(void) {
}
void (*sUkikiActions[])(void) = { ukiki_act_idle,      ukiki_act_run,        ukiki_act_turn_to_mario,
                                  ukiki_act_jump,      ukiki_act_go_to_spot, ukiki_act_play,
                                  ukiki_act_climb_vine };

// when free, switch between idle, go to spot, go to flower and
// play, go to water and play, chase monkey

// idle in place
//-sometimes rotate to mario, can do jump here
//-sometimes dont, can do handstand here
//-jump over mario
//-slightly walk towards/away from mario
// play in water
// jump around in flowers
// go to random spot on different floors (there have to be 2 different floors of spots)
//- climb on vine if they hit a vine
//- jump from vine if they are on vine
// chase another monkey, if close
void goalGeneration() {
    u16 i;
    u16 pointgoal;
    if (o->oOpacity) {
        o->oOpacity--;
    }
    if ((o->oOpacity < 1) || (o->oMoveFlags & OBJ_MOVE_HIT_WALL)) {
        o->oOpacity = random_u16() % 150 + 100;
        i = random_u16() % 24; // high chance to keep the goal
        if (i < 6) {
#define MAXHEIGHTLOW -270.f
#define MAXHEIGHTMIDDLE 120.f
            if (o->oPosY < MAXHEIGHTLOW) {
                i = 2;
            } else {
                i = (o->oPosY < MAXHEIGHTMIDDLE);
            }
            pointgoal = random_u16() % pointcounts[i];
            o->oBobombBuddyPosXCopy = ((s16 *) segmented_to_virtual(pointarrays[i]))[1 + pointgoal * 4];
            o->oBobombBuddyPosYCopy = ((s16 *) segmented_to_virtual(pointarrays[i]))[2 + pointgoal * 4];
            o->oBobombBuddyPosZCopy = ((s16 *) segmented_to_virtual(pointarrays[i]))[3 + pointgoal * 4];
            o->oHiddenBlueCoinSwitch = 0;
        } else if (i < 7) {
            o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvMacroUkiki);
        } else if (i < 10) {
            o->oBobombBuddyPosXCopy = o->oPosX;
            o->oBobombBuddyPosYCopy = o->oPosY;
            o->oBobombBuddyPosZCopy = o->oPosZ;
            o->oHiddenBlueCoinSwitch = 0;
        }
    }
    if (o->oHiddenBlueCoinSwitch) {
        o->oBobombBuddyPosXCopy = o->oHiddenBlueCoinSwitch->oPosX;
        o->oBobombBuddyPosYCopy = o->oHiddenBlueCoinSwitch->oPosY;
        o->oBobombBuddyPosZCopy = o->oHiddenBlueCoinSwitch->oPosZ;
    }
}
extern u32 deactivateOnDrawDist(f32 multiplier);
// run away from mario, if mario gets close. jump over mario if mario gets too close
void bhv_ukiki_loop(void) {
    if (deactivateOnDrawDist(1.2f)){
        return;
    }
    goalGeneration();
    if (o->oAction != UKIKI_ACT_JUMP) {
        o->oBobombBuddyRole = o->oPrevAction;
    }
    cur_obj_call_action_function(sUkikiActions);
    if (o->oAction > 3) {
        if (o->oDistanceToMario < 500.f) {
            o->oAction = 0;
        }
    }

    o->oAnimState = (gGlobalTimer % 50 < 7);
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);

    if (!(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)) {
        exec_anim_sound_state(sUkikiSoundStates);
    }
}
