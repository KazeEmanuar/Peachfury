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
extern void buzzywalk(void);

void buzzycode(void) {
    cur_obj_update_floor_and_walls();
    switch (o->oAction) {
        case 0:
            // walk
            o->oInteractType = 0x8000;
            cur_obj_init_animation(0);
            buzzywalk();
            o->oFaceAngleYaw = o->oMoveAngleYaw;
            if (o->oTimer > 15) {
                if (o->oDistanceToMario < 750.f) {
                    o->oAction++;
                    cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_ALERT);
                    cur_obj_init_animation(1);
                    o->oVelY = 20.f;
                }
            }
            break;
        case 1:
            // go in shell and attack
            cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), 0x800);
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.2f);
            if (o->header.gfx.unk38.animFrame > 9) {
                o->oAction++;
                o->oForwardVel = 0;
            }
            break;
        case 2:
            // charge in shell
            o->header.gfx.unk38.animFrame = 11;
            o->oAngleVelYaw += 0x0240;
            if (o->oTimer == 2) {
                spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oTimer == 7) {
                spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oTimer == 10) {
                spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
                cur_obj_play_sound_2(SOUND_GENERAL_PAINTING_EJECT);
            }
            if (o->oTimer == 12) {
                spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oTimer == 14) {
                spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oTimer >= 15) {
                spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
            }
            if (o->oAngleVelYaw > 0x1570) {
                o->oAction++;
                cur_obj_play_sound_2(SOUND_GENERAL_PAINTING_EJECT);
            }
            break;
        case 3:
            if (!(o->oTimer & 7)) {
                cur_obj_play_sound_2(SOUND_GENERAL_PAINTING_EJECT);
            }
            spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
            o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
            if (o->header.gfx.unk38.animFrame == 28) {
                o->oForwardVel = 48.f;
                o->oAction++;
                cur_obj_play_sound_2(SOUND_OBJ_MRI_SHOOT);
            }
            // stop shell spin
            break;
        case 4:
            if (o->oTimer > 20) {
                o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 2.0f);
            }
            if (o->oSubAction) {
                if (o->header.gfx.unk38.animFrame == 38) {
                    o->oAction = 0;
                    o->oMoveAngleYaw = o->oFaceAngleYaw;
                }
            } else {
                o->header.gfx.unk38.animFrame = 28;
                if ((o->oForwardVel < 2.0f) || (o->oMoveFlags & OBJ_MOVE_HIT_WALL)) {
                    o->oSubAction = 1;
                }
            }
            // come out of shell
            break;
        case 5:
            if ((o->oTimer > 100) && (o->oTimer < 134)) {
                o->oGraphYOffset = 7.f * ((o->oTimer & 1) * 2 - 1);
            }
            if (o->oTimer < 130) {
                o->header.gfx.unk38.animFrame = 28;
            }

            switch (o->oHeldState) {
                case HELD_FREE:
                    cur_obj_init_animation(1);
                    o->oInteractType = 0x0002;
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.0f);
                    o->oAngleVelYaw = approach_s16_symmetric(o->oAngleVelYaw, 0, 0x100);
                    if (o->header.gfx.unk38.animFrame == 38) {
                        o->oAction = 0;
                    }
                    break;
                case HELD_HELD:
                    gMarioState->heldObj->header.gfx.unk38.animFrame = 20;
                    cur_obj_become_intangible();
                    cur_obj_disable_rendering();
                    o->oTimer = 0;
                    o->oMoveAngleYaw = gMarioState->faceAngle[1];
                    o->oFaceAngleYaw = gMarioState->faceAngle[1];
                    break;
                case HELD_THROWN:
                    o->oForwardVel = 48.f;
                    o->oVelY = 20.f;
                case HELD_DROPPED:
                    o->oIntangibleTimer = 0;
                    cur_obj_enable_rendering();
                    o->oHeldState = HELD_FREE;
                    o->oTimer = 0;
                    break;
            }
            break;
    }
    if (o->oInteractStatus) {
        if (o->oInteractStatus == 0xC003) {
            o->oAction = 5;
            o->oTimer = 0;
        }
        o->oInteractStatus = 0;
    }
    o->oFaceAngleYaw += o->oAngleVelYaw;
    cur_obj_move_standard(-78);
}