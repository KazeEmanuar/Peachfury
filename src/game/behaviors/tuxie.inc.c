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

void play_penguin_walking_sound(s32 walk) {
    s32 sound;
    if (o->oSoundStateID == 0) {
        sound = SOUND_OBJ_BABY_PENGUIN_WALK;
        set_obj_anim_with_accel_and_sound(1, 11, sound);
    }
}
// some run away from mario, one runs towards mario (bparam2 not zero)
// stand in place
// go to random place
//- chance to do a bellyslide on land
//- swim if in water
//- jump if terrain in front of them is not too much higher than it and its either in water or grounded
// PENGUIN_WALK_BABY, SOUND_OBJ2_BABY_PENGUIN_YELL,  SOUND_OBJ_BABY_PENGUIN_DIVE
u32 checkPenguFloor() {
    struct Surface *floor;
    f32 floorHeight = find_floor(o->oPosX + sins(o->oMoveAngleYaw) * 150.f, o->oPosY + 500.f,
                                 o->oPosZ + coss(o->oMoveAngleYaw) * 150.f, &floor);
    if (!floor) {
        o->oOpacity = 0;
        return 0;
    }
    if ((SURFACETYPE(floor) != SURFACE_VERY_SLIPPERY) && (SURFACETYPE(floor) != SURFACE_PAINTING_WARP_F8)) {
        o->oOpacity = 0;
        return 0;
    }
    if (((floorHeight - o->oPosY) > 5.f) && ((floorHeight - o->oPosY) < 300.f)) {
        return 1;
    } else {
        return 0;
    }
}
extern u32 deactivateOnDrawDist(f32 multiplier);
extern const Trajectory ccm_area_1_spline_PENGUPOINTS[];
#define PINGUINPOINTS 7
void bhv_small_penguin_loop(void) {
    u16 targetAngle;
    u8 pointgoal;
    struct Surface *floor;
    f32 speedgoal = 0.f;
    f32 x, y, z;
    f32 waterLevel;
    x = o->oPosX;
    y = o->oPosY;
    z = o->oPosZ;
    o->oBuoyancy = -.55f;
    if (deactivateOnDrawDist(1.2f)){
        return;
    }
    if (o->oOpacity) {
        o->oOpacity--;
    }
    if (o->oOpacity < 2) {
        if ((random_u16() & 127) == 0) {
            cur_obj_play_sound_2(SOUND_OBJ2_BABY_PENGUIN_YELL);
        }
        o->oOpacity = random_u16() % 100 + 50;
        pointgoal = random_u16() % ((PINGUINPOINTS + 25 * (o->oSubAction != 3)));
        if (pointgoal < PINGUINPOINTS) {
            o->oBobombBuddyPosXCopy =
                ((s16 *) segmented_to_virtual(ccm_area_1_spline_PENGUPOINTS))[1 + pointgoal * 4];
            o->oBobombBuddyPosYCopy =
                ((s16 *) segmented_to_virtual(ccm_area_1_spline_PENGUPOINTS))[2 + pointgoal * 4];
            o->oBobombBuddyPosZCopy =
                ((s16 *) segmented_to_virtual(ccm_area_1_spline_PENGUPOINTS))[3 + pointgoal * 4];
            o->oAction = 2;
        } else if (pointgoal < (PINGUINPOINTS + 6)) {
            o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvSmallPenguin);
            o->oAction = 3;
        } else {
            o->oAction = 1;
        }
    }
    if (o->oDistanceToMario < 750.f) {
        o->oOpacity = 10;
        o->oAction = 0;
    }
    switch (o->oAction) {
        case 0:
            // run away or twowards mario
            speedgoal = 6.5f;
            if (o->oBehParams2ndByte) {
                targetAngle = o->oAngleToMario;
            } else {
                targetAngle = o->oAngleToMario + 0x8000;
            }
            break;
        case 1:
            // stand around, rotate towards new targetangle
            if (!o->oTimer) {
                o->oBobombBuddyCannonStatus = o->oMoveAngleYaw + (random_u16() & 0x1fff) - 0x1000;
            }
            speedgoal = 0.f;
            targetAngle = o->oBobombBuddyCannonStatus;
            break;
        case 2:
            // go to random place
            speedgoal = 4.f;
            targetAngle =
                atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
            break;
        case 3:
            // walk to penguin
            speedgoal = 3.f;
            o->oBobombBuddyPosXCopy = o->oHiddenBlueCoinSwitch->oPosX;
            o->oBobombBuddyPosZCopy = o->oHiddenBlueCoinSwitch->oPosZ;
            targetAngle =
                atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
            break;
    }

    if ((o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER) && (o->oBobombBuddyRole != 2)) {
        o->oBobombBuddyRole = 3;
    }
#define WALK 0
#define JUMP 2
#define STAND 3
#define BELLYSLIDE 1
    switch (o->oBobombBuddyRole) {
        case 0:
            // walk
            if (o->oForwardVel > 0.f) {
                cur_obj_init_animation(WALK);
                play_penguin_walking_sound(0);
            } else {

                cur_obj_init_animation(STAND);
            }
            if (o->oForwardVel > 8.f) {
                if (!(random_u16() % 150)) {
                    o->oBobombBuddyRole = 1;
                    o->oBobombBuddyBlinkTimer = 0;
                    cur_obj_play_sound_2(SOUND_OBJ_BABY_PENGUIN_DIVE);
                }
            }
            break;
        case 1:
            o->oBobombBuddyBlinkTimer++;
            cur_obj_init_animation(BELLYSLIDE);
            if (o->oBobombBuddyBlinkTimer > ((random_u16() & 0x1ff) + 40)) {
                o->oBobombBuddyRole = 0;
            }
            targetAngle = o->oMoveAngleYaw;
            // bellyslide
            speedgoal = 8.f;
            break;
        case 2:
            // jump
            cur_obj_init_animation(JUMP);
            if (o->oVelY < 2.f) {
                o->oBobombBuddyRole = 0;
            }
            speedgoal = 5.f;
            targetAngle = o->oMoveAngleYaw;
            break;
        case 3:
            // swim
            o->oVelY += 2.5f;
            cur_obj_init_animation(BELLYSLIDE);
            if (!(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)) {
                o->oBobombBuddyRole = 0;
            }
            speedgoal = 5.f;
            break;
    }

    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0240);
    o->oForwardVel = approach_f32(o->oForwardVel,
                                  speedgoal * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 1.1f, 2.f);
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
    if ((o->oVelY < 10.f) && (checkPenguFloor())) {
        o->oBobombBuddyRole = 2;
        o->oVelY = 36.f;
        waterLevel = find_water_level(o->oPosX, o->oPosZ) + 10.f;
        o->oForwardVel *= 0.75f;
        if (o->oPosY < waterLevel){
            o->oPosY = waterLevel;
        }
        o->oMoveFlags &= ~OBJ_MOVE_MASK_IN_WATER;
    }
}

/** Geo switch logic for Tuxie's mother's eyes. Cases 0-4. Interestingly, case
 * 4 is unused, and is the eye state seen in Shoshinkai 1995 footage.
 */
Gfx *geo_switch_tuxie_mother_eyes(s32 run, struct GraphNode *node, UNUSED Mat4 *mtx) {
    return NULL;
}
