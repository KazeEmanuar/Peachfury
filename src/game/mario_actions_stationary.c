#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "audio/data.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "camera.h"
#include "engine/math_util.h"
#include "interaction.h"
#include "level_update.h"
#include "mario.h"
#include "mario_actions_stationary.h"
#include "mario_step.h"
#include "memory.h"
#include "save_file.h"
#include "sound_init.h"
#include "surface_terrains.h"
#include "thread6.h"
#include "OPT_FOR_SIZE.h"

s32 check_common_idle_cancels(struct MarioState *m) {
    mario_drop_held_object(m);
    if (m->floor->normal.y < 0.29237169f) {
        return mario_push_off_steep_floor(m, ACT_FREEFALL, 0);
    }

    if (!mario_can_get_up(m)) { // CuckyDev: Force Mario into crouching state when under ceiling
        return set_mario_action(m, ACT_START_CROUCHING, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_JUMP, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_FIRST_PERSON, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        m->faceAngle[1] = (s16) m->intendedYaw;
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    if (m->input & INPUT_Z_DOWN) {
        return set_mario_action(m, ACT_START_CROUCHING, 0);
    }

    return 0;
}

s32 check_common_hold_idle_cancels(struct MarioState *m) {
    if (m->floor->normal.y < 0.29237169f) {
        return mario_push_off_steep_floor(m, ACT_HOLD_FREEFALL, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_HOLD_JUMP, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_HOLD_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        m->faceAngle[1] = (s16) m->intendedYaw;
        return set_mario_action(m, ACT_HOLD_WALKING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    if (m->input & INPUT_Z_DOWN) {
        return drop_and_set_mario_action(m, ACT_START_CROUCHING, 0);
    }

    return 0;
}

s32 act_idle(struct MarioState *m) {
    if (m->quicksandDepth > 30.0f) {
        return set_mario_action(m, ACT_IN_QUICKSAND, 0);
    }

    if (m->input & INPUT_IN_POISON_GAS) {
        return set_mario_action(m, ACT_COUGHING, 0);
    }

    if (!(m->actionArg & 1) && m->health < 0x300) {
        return set_mario_action(m, ACT_PANTING, 0);
    }

    if (check_common_idle_cancels(m)) {
        return 1;
    }
    if ((m->area->terrainType & TERRAIN_MASK) == TERRAIN_SPOOKY) {
        return set_mario_action(m, ACT_SHIVERING, 0);
    }
    if (m->actionState == 3) {
        if ((m->area->terrainType & TERRAIN_MASK) == TERRAIN_SNOW) {
            return set_mario_action(m, ACT_SHIVERING, 0);
        } else {
            return set_mario_action(m, ACT_START_SLEEPING, 0);
        }
    }

    if (/*m->actionArg & 1*/ 0) {
        set_mario_animation(m, MARIO_ANIM_STAND_AGAINST_WALL);
    } else {
        switch (m->actionState) {
            case 0:
                set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_LEFT);
                break;

            case 1:
                set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_RIGHT);
                break;

            case 2:
                set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_CENTER);
                break;
        }

        if (is_anim_at_end(m)) {
            if (++m->actionState == 3) {
                f32 deltaYOfFloorBehindMario =
                    m->pos[1] - find_floor_height_relative_polar(m, -0x8000, 60.0f);
                if (deltaYOfFloorBehindMario < -24.0f || 24.0f < deltaYOfFloorBehindMario
                    || m->floor->flags & SURFACE_FLAG_DYNAMIC) {
                    m->actionState = 0;
                } else {
                    // If Mario hasn't turned his head 10 times yet, stay idle instead of going to
                    // sleep.
                    m->actionTimer++;
                    if (m->actionTimer < 10) {
                        m->actionState = 0;
                    }
                }
            }
        }
    }

    stationary_ground_step(m);
    if (m->floor && (SURFACETYPE(m->floor) == 0x00A7)) {
        return set_mario_action(m, ACT_GRINDING, 0);
    }
    return 0;
}

void play_anim_sound(struct MarioState *m, u32 actionState, s32 animFrame, u32 sound) {
    if (m->actionState == actionState && m->marioObj->header.gfx.unk38.animFrame == animFrame) {
        play_sound(sound, m->marioObj->header.gfx.cameraToObject);
    }
}

s32 act_start_sleeping(struct MarioState *m) {
    s32 sp24;
    if (check_common_idle_cancels(m)) {
        return 1;
    }

    if (m->quicksandDepth > 30.0f) {
        return set_mario_action(m, ACT_IN_QUICKSAND, 0);
    }

    if (m->actionState == 4) {
        return set_mario_action(m, ACT_SLEEPING, 0);
    }

    switch (m->actionState) {
        case 0:
            sp24 = set_mario_animation(m, MARIO_ANIM_START_SLEEP_IDLE);
            break;

        case 1:
            sp24 = set_mario_animation(m, MARIO_ANIM_START_SLEEP_SCRATCH);
            break;

        case 2:
            sp24 = set_mario_animation(m, MARIO_ANIM_START_SLEEP_YAWN);
            m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            break;

        case 3:
            sp24 = set_mario_animation(m, MARIO_ANIM_START_SLEEP_SITTING);
            m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            break;
    }

    play_anim_sound(m, 1, 41, SOUND_ACTION_PAT_BACK);
    play_anim_sound(m, 1, 49, SOUND_ACTION_PAT_BACK);
    play_anim_sound(m, 3, 15, m->terrainSoundAddend + SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);

    if (is_anim_at_end(m)) {
        m->actionState++;
    }

    if (m->actionState == 2) {
        if (sp24 == -1) {
            play_sound(SOUND_MARIO_YAWNING, m->marioObj->header.gfx.cameraToObject);
        }
    }

    if (m->actionState == 1) {
        if (sp24 == -1) {
            play_sound(SOUND_MARIO_IMA_TIRED, m->marioObj->header.gfx.cameraToObject);
        }
    }

    stationary_ground_step(m);
    return 0;
}

s32 act_sleeping(struct MarioState *m) {
    s32 sp24;

    if (m->input & INPUT_UNKNOWN_A41F) {
        return set_mario_action(m, ACT_WAKING_UP, m->actionState);
    }
    if (m->quicksandDepth > 30.0f) {
        return set_mario_action(m, ACT_WAKING_UP, m->actionState);
    }

    if (m->pos[1] - find_floor_height_relative_polar(m, -0x8000, 60.0f) > 24.0f) {
        return set_mario_action(m, ACT_WAKING_UP, m->actionState);
    }

    m->marioBodyState->eyeState = MARIO_EYES_CLOSED;
    stationary_ground_step(m);
    switch (m->actionState) {
        case 0: {
            sp24 = set_mario_animation(m, MARIO_ANIM_SLEEP_IDLE);

            if (sp24 == -1 && !m->actionTimer) {
                lower_background_noise(2);
            }

            if (sp24 == 2) {
                play_sound(SOUND_MARIO_SNORING1, m->marioObj->header.gfx.cameraToObject);
            }

            if (sp24 == 20) {
                play_sound(SOUND_MARIO_SNORING2, m->marioObj->header.gfx.cameraToObject);
            }

            if (is_anim_at_end(m)) {
                m->actionTimer++;
                if (m->actionTimer > 45) {
                    m->actionState++;
                }
            }
            break;
        }
        case 1: {
            if (set_mario_animation(m, MARIO_ANIM_SLEEP_START_LYING) == 18) {
                play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
            }

            if (is_anim_at_end(m)) {
                m->actionState++;
            }
            break;
        }
        case 2: {
            sp24 = set_mario_animation(m, MARIO_ANIM_SLEEP_LYING);
            play_sound_if_no_flag(m, SOUND_MARIO_SNORING3, MARIO_ACTION_SOUND_PLAYED);
            break;
        }
    }
    return 0;
}

s32 act_waking_up(struct MarioState *m) {
    if (!m->actionTimer) {
        killSound(SOUND_MARIO_SNORING1, m->marioObj->header.gfx.cameraToObject);
        killSound(SOUND_MARIO_SNORING2, m->marioObj->header.gfx.cameraToObject);
        killSound(SOUND_MARIO_SNORING3, m->marioObj->header.gfx.cameraToObject);
        raise_background_noise(2);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    m->actionTimer++;

    if (m->actionTimer > 20) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    stationary_ground_step(m);

    set_mario_animation(m, !m->actionArg ? MARIO_ANIM_WAKE_FROM_SLEEP : MARIO_ANIM_WAKE_FROM_LYING);

    return 0;
}

s32 act_shivering(struct MarioState *m) {
    s32 sp24;

    if (check_common_idle_cancels(m)) {
        return 1;
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }
    if (m->input & INPUT_UNKNOWN_A41F) {
        m->actionState = 2;
    }

    stationary_ground_step(m);
    switch (m->actionState) {
        case 0: {
            sp24 = set_mario_animation(m, MARIO_ANIM_SHIVERING_WARMING_HAND);
            if (sp24 == 0x31) {
                if ((m->area->terrainType & TERRAIN_MASK) != TERRAIN_SPOOKY) {
                    m->particleFlags |= PARTICLE_BREATH;
                }
                play_sound(SOUND_MARIO_PANTING_COLD, m->marioObj->header.gfx.cameraToObject);
            }
            if (sp24 == 7 || sp24 == 0x51) {
                play_sound(SOUND_ACTION_CLAP_HANDS_COLD, m->marioObj->header.gfx.cameraToObject);
            }
            if (is_anim_past_end(m)) {
                m->actionState = 1;
            }
            break;
        }
        case 1: {
            sp24 = set_mario_animation(m, MARIO_ANIM_SHIVERING);
            if (sp24 == 9 || sp24 == 0x19 || sp24 == 0x2C) {
                play_sound(SOUND_ACTION_CLAP_HANDS_COLD, m->marioObj->header.gfx.cameraToObject);
            }
            break;
        }
        case 2: {
            set_mario_animation(m, MARIO_ANIM_SHIVERING_RETURN_TO_IDLE);
            if (is_anim_past_end(m)) {
                set_mario_action(m, ACT_IDLE, 0);
            }
            break;
        }
    }
    return 0;
}

s32 act_coughing(struct MarioState *m) {
    s32 sp1C;

    if (check_common_idle_cancels(m)) {
        return 1;
    }

    stationary_ground_step(m);
    sp1C = set_mario_animation(m, MARIO_ANIM_COUGHING);
    if (sp1C == 0x19 || sp1C == 0x23) {
        play_sound(SOUND_MARIO_COUGHING3, m->marioObj->header.gfx.cameraToObject);
    }

    if (sp1C == 0x32 || sp1C == 0x3A) {
        play_sound(SOUND_MARIO_COUGHING2, m->marioObj->header.gfx.cameraToObject);
    }

    if (sp1C == 0x47 || sp1C == 0x50) {
        play_sound(SOUND_MARIO_COUGHING1, m->marioObj->header.gfx.cameraToObject);
    }

    return 0;
}

s32 act_hold_idle(struct MarioState *m) {
    if (segmented_to_virtual(&bhvJumpingBox) == m->heldObj->behavior) {
        return set_mario_action(m, ACT_CRAZY_BOX_BOUNCE, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->quicksandDepth > 30.0f) {
        return drop_and_set_mario_action(m, ACT_IN_QUICKSAND, 0);
    }

    if (check_common_hold_idle_cancels(m)) {
        return 1;
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
    return 0;
}

s32 act_hold_heavy_idle(struct MarioState *m) {

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return drop_and_set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_HEAVY_WALKING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_HEAVY_THROW, 0);
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_IDLE_HEAVY_OBJ);
    return 0;
}

s32 act_standing_against_wall(struct MarioState *m) {

    if (m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_action_exits(m);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_FIRST_PERSON, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    set_mario_animation(m, MARIO_ANIM_STAND_AGAINST_WALL);
    stationary_ground_step(m);
    return 0;
}

s32 act_in_quicksand(struct MarioState *m) {
    if (m->quicksandDepth < 30.0f) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (check_common_idle_cancels(m) != 0) {
        return 1;
    }

    if (m->quicksandDepth > 70.0f) {
        set_mario_animation(m, MARIO_ANIM_DYING_IN_QUICKSAND);
    } else {
        set_mario_animation(m, MARIO_ANIM_IDLE_IN_QUICKSAND);
    }

    stationary_ground_step(m);
    return 0;
}

s32 act_crouching(struct MarioState *m) {
    if (mario_can_get_up(m)) { // crawlspaces

        if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
            return set_jumping_action(m, ACT_BACKFLIP, 0);
        }

        if (m->input & INPUT_OFF_FLOOR) {
            return set_mario_action(m, ACT_FREEFALL, 0);
        }

        if (m->input & INPUT_ABOVE_SLIDE) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }

        if (m->input & INPUT_FIRST_PERSON) {
            return set_mario_action(m, ACT_STOP_CROUCHING, 0);
        }

        if (!(m->input & INPUT_Z_DOWN)) {
            return set_mario_action(m, ACT_STOP_CROUCHING, 0);
        }
        if (m->input & INPUT_B_PRESSED) {
            return set_mario_action(m, ACT_PUNCHING, 9);
        }
    }
    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_START_CRAWLING, 0);
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_CROUCHING);
    return 0;
}

s32 act_panting(struct MarioState *m) {

    if (m->health >= 0x500) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (check_common_idle_cancels(m)) {
        return 1;
    }

    if (set_mario_animation(m, MARIO_ANIM_WALK_PANTING) == 1) {
        play_sound(SOUND_MARIO_PANTING + ((gAudioRandom % 3U) << 0x10),
                   m->marioObj->header.gfx.cameraToObject);
    }

    stationary_ground_step(m);
    m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
    return 0;
}

void stopping_step(struct MarioState *m, s32 animID, u32 action) {
    stationary_ground_step(m);
    set_mario_animation(m, animID);
    if (is_anim_at_end(m) != 0) {
        set_mario_action(m, action, 0);
    }
}

s32 act_braking_stop(struct MarioState *m) {

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    if (!(m->input & INPUT_FIRST_PERSON)
        && m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_action_exits(m);
    }

    stopping_step(m, MARIO_ANIM_STOP_SKID, ACT_IDLE);
    return 0;
}

s32 act_butt_slide_stop(struct MarioState *m) {

    if (m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_action_exits(m);
    }

    stopping_step(m, MARIO_ANIM_STOP_SLIDE, ACT_IDLE);
    if (m->marioObj->header.gfx.unk38.animFrame == 6) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    return 0;
}

s32 act_hold_butt_slide_stop(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_hold_action_exits(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    stopping_step(m, MARIO_ANIM_STAND_UP_FROM_SLIDING_WITH_LIGHT_OBJ, ACT_HOLD_IDLE);
    return 0;
}

s32 act_slide_kick_slide_stop(struct MarioState *m) {

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    stopping_step(m, MARIO_ANIM_CROUCH_FROM_SLIDE_KICK, ACT_CROUCHING);
    return 0;
}

s32 act_start_crouching(struct MarioState *m) {
    if (mario_can_get_up(m)) { // crawlspaces

        if (m->input & INPUT_OFF_FLOOR) {
            return set_mario_action(m, ACT_FREEFALL, 0);
        }

        if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
            return set_jumping_action(m, ACT_BACKFLIP, 0);
        }

        if (m->input & INPUT_ABOVE_SLIDE) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }
    }
    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_START_CROUCHING);
    if (is_anim_past_end(m)) {
        set_mario_action(m, ACT_CROUCHING, 0);
    }
    return 0;
}

s32 act_stop_crouching(struct MarioState *m) {

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_STOP_CROUCHING);
    if (is_anim_past_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
    }
    return 0;
}

s32 act_start_crawling(struct MarioState *m) {
    if (mario_can_get_up(m)) { // crawlspaces
        if (m->input & INPUT_FIRST_PERSON) {
            return set_mario_action(m, ACT_STOP_CROUCHING, 0);
        }

        if (m->input & INPUT_OFF_FLOOR) {
            return set_mario_action(m, ACT_FREEFALL, 0);
        }

        if (m->input & INPUT_ABOVE_SLIDE) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }
    }
    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_START_CRAWLING);
    if (is_anim_past_end(m)) {
        set_mario_action(m, ACT_CRAWLING, 0);
    }

    return 0;
}

s32 act_stop_crawling(struct MarioState *m) {
    if (mario_can_get_up(m)) { // crawlspaces

        if (m->input & INPUT_OFF_FLOOR) {
            return set_mario_action(m, ACT_FREEFALL, 0);
        }

        if (m->input & INPUT_ABOVE_SLIDE) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }
    }
    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_STOP_CRAWLING);
    if (is_anim_past_end(m)) {
        set_mario_action(m, ACT_CROUCHING, 0);
    }
    return 0;
}

s32 act_shockwave_bounce(struct MarioState *m) {
    s16 sp1E;
    f32 sp18;
    if (++m->actionTimer == 0x30) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    sp1E = (m->actionTimer % 0x10) << 0xC;
    sp18 = (f32) (((f32) (6 - m->actionTimer / 8) * 8.0f) + 4.0f);
    mario_set_forward_vel(m, 0);
    vec3f_set(m->vel, 0.0f, 0.0f, 0.0f);
    if (sins(sp1E) >= 0.0f) {
        m->pos[1] = sins(sp1E) * sp18 + m->floorHeight;
    } else {
        m->pos[1] = m->floorHeight - sins(sp1E) * sp18;
    }

    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    return 0;
}

s32 landing_step(struct MarioState *m, s32 arg1, u32 action) {
    stationary_ground_step(m);
    set_mario_animation(m, arg1);
    if (is_anim_at_end(m)) {
        return set_mario_action(m, action, 0);
    }
    return 0;
}

s32 check_common_landing_cancels(struct MarioState *m, u32 action) {

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        if (!action) {
            return set_jump_from_landing(m);
        } else {
            return set_jumping_action(m, action, 0);
        }
    }

    if (m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_action_exits(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    return 0;
}

s32 endLanding(struct MarioState *m, s32 animation) {
    if (check_common_landing_cancels(m, 0)) {
        return 1;
    }

    landing_step(m, animation, ACT_IDLE);
    return 0;
}
s32 act_jump_land_stop(struct MarioState *m) {
    return endLanding(m, MARIO_ANIM_LAND_FROM_SINGLE_JUMP);
}

s32 act_double_jump_land_stop(struct MarioState *m) {
    return endLanding(m, MARIO_ANIM_LAND_FROM_DOUBLE_JUMP);
}

s32 act_side_flip_land_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_SLIDEFLIP_LAND, ACT_IDLE);
    m->marioObj->header.gfx.angle[1] += 0x8000;
    return FALSE;
}

s32 act_freefall_land_stop(struct MarioState *m) {
    return endLanding(m, MARIO_ANIM_GENERAL_LAND);
}

s32 act_triple_jump_land_stop(struct MarioState *m) {
    return endLanding(m, MARIO_ANIM_TRIPLE_JUMP_LAND);
}

s32 act_backflip_land_stop(struct MarioState *m) {
    if (!(m->input & INPUT_Z_DOWN) || m->marioObj->header.gfx.unk38.animFrame >= 6) {
        m->input &= -3;
    }

    if (check_common_landing_cancels(m, ACT_BACKFLIP)) {
        return 1;
    }

    landing_step(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_IDLE);
    return 0;
}

s32 act_lava_boost_land(struct MarioState *m) {
    m->input &= -0x2011;
    return endLanding(m, MARIO_ANIM_STAND_UP_FROM_LAVA_BOOST);
}

s32 act_long_jump_land_stop(struct MarioState *m) {
    m->input &= -0x2001;
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return 1;
    }

    landing_step(m,
                 !m->marioObj->oMarioLongJumpIsSlow ? MARIO_ANIM_CROUCH_FROM_FAST_LONGJUMP
                                                    : MARIO_ANIM_CROUCH_FROM_SLOW_LONGJUMP,
                 ACT_CROUCHING);
    return 0;
}

s32 act_hold_jump_land_stop(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }
    if (m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_hold_action_exits(m);
    }
    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    landing_step(m, MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ, ACT_HOLD_IDLE);
    return 0;
}

s32 act_hold_freefall_land_stop(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }
    if (m->input & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_hold_action_exits(m);
    }
    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }
    landing_step(m, MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ, ACT_HOLD_IDLE);
    return 0;
}

s32 act_air_throw_land(struct MarioState *m) {
    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (++m->actionTimer == 4) {
        mario_throw_held_object(m);
    }

    landing_step(m, MARIO_ANIM_THROW_LIGHT_OBJECT, ACT_IDLE);
    return 0;
}

s32 act_twirl_land(struct MarioState *m) {
    m->actionState = 1;

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_TWIRL_LAND);
    if (m->angleVel[1] > 0) {
        m->angleVel[1] -= 0x400;
        if (m->angleVel[1] < 0) {
            m->angleVel[1] = 0;
        }

        m->twirlYaw += m->angleVel[1];
    }

    m->marioObj->header.gfx.angle[1] += m->twirlYaw;
    if (is_anim_at_end(m)) {
        if (m->angleVel[1] == 0) {
            m->faceAngle[1] += m->twirlYaw;
            set_mario_action(m, ACT_IDLE, 0);
        }
    }

    return 0;
}

s32 act_ground_pound_land(struct MarioState *m) {
    m->actionState = 1;

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BUTT_SLIDE, 0);
    }

    if (gMarioState->controller->buttonPressed & A_BUTTON) {
        gMarioState->vel[1] = 70.f;
        play_mario_jump_sound(gMarioState);
        return set_mario_action(m, ACT_TRIPLE_JUMP, 0);
    }
    landing_step(m, MARIO_ANIM_GROUND_POUND_LANDING, ACT_BUTT_SLIDE_STOP);
    return 0;
}

s32 act_first_person(struct MarioState *m) {
    if (m->actionState == 0) {
        lower_background_noise(2);
        set_camera_mode(m->area->camera, CAMERA_MODE_C_UP, 0x02);
        m->actionState = 1;
    } else {
        if ((m->controller->buttonPressed & (L_TRIG | Z_TRIG | A_BUTTON | B_BUTTON))) {
            raise_background_noise(2);
            play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gDefaultSoundArgs);
            set_camera_mode(m->area->camera, CAM_MODE_NEWCAM, 0x10); // last number does nothing
            return set_mario_action(m, ACT_IDLE, 0);
        }
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    return 0;
}

s32 check_common_stationary_cancels(struct MarioState *m) {
    if (m->pos[1] < m->waterLevel - 100) {
        return set_water_plunge_action(m);
    }

    if (m->input & INPUT_SQUISHED) {
        return drop_and_set_mario_action(m, ACT_SQUISHED, 0);
    }

    if (m->health < 0x100) {
        return drop_and_set_mario_action(m, ACT_STANDING_DEATH, 0);
    }
    return 0;
}

s32 mario_execute_stationary_action(struct MarioState *m) {
    s32 sp24;

    if (check_common_stationary_cancels(m)) {
        return 1;
    }

    if (mario_update_quicksand(m, 0.5f)) {
        return 1;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_IDLE:                    sp24 = act_idle(m);                             break;
        case ACT_START_SLEEPING:          sp24 = act_start_sleeping(m);                   break;
        case ACT_SLEEPING:                sp24 = act_sleeping(m);                         break;
        case ACT_WAKING_UP:               sp24 = act_waking_up(m);                        break;
        case ACT_PANTING:                 sp24 = act_panting(m);                          break;
        case ACT_HOLD_IDLE:               sp24 = act_hold_idle(m);                        break;
        case ACT_HOLD_HEAVY_IDLE:         sp24 = act_hold_heavy_idle(m);                  break;
        case ACT_IN_QUICKSAND:            sp24 = act_in_quicksand(m);                     break;
        case ACT_STANDING_AGAINST_WALL:   sp24 = act_standing_against_wall(m);            break;
        case ACT_COUGHING:                sp24 = act_coughing(m);                         break;
        case ACT_SHIVERING:               sp24 = act_shivering(m);                        break;
        case ACT_CROUCHING:               sp24 = act_crouching(m);                        break;
        case ACT_START_CROUCHING:         sp24 = act_start_crouching(m);                  break;
        case ACT_STOP_CROUCHING:          sp24 = act_stop_crouching(m);                   break;
        case ACT_START_CRAWLING:          sp24 = act_start_crawling(m);                   break;
        case ACT_STOP_CRAWLING:           sp24 = act_stop_crawling(m);                    break;
        case ACT_SLIDE_KICK_SLIDE_STOP:   sp24 = act_slide_kick_slide_stop(m);            break;
        case ACT_SHOCKWAVE_BOUNCE:        sp24 = act_shockwave_bounce(m);                 break;
        case ACT_FIRST_PERSON:            sp24 = act_first_person(m);                     break;
        case ACT_JUMP_LAND_STOP:          sp24 = act_jump_land_stop(m);                   break;
        case ACT_DOUBLE_JUMP_LAND_STOP:   sp24 = act_double_jump_land_stop(m);            break;
        case ACT_FREEFALL_LAND_STOP:      sp24 = act_freefall_land_stop(m);               break;
        case ACT_SIDE_FLIP_LAND_STOP:     sp24 = act_side_flip_land_stop(m);              break;
        case ACT_HOLD_JUMP_LAND_STOP:     sp24 = act_hold_jump_land_stop(m);              break;
        case ACT_HOLD_FREEFALL_LAND_STOP: sp24 = act_hold_freefall_land_stop(m);          break;
        case ACT_AIR_THROW_LAND:          sp24 = act_air_throw_land(m);                   break;
        case ACT_LAVA_BOOST_LAND:         sp24 = act_lava_boost_land(m);                  break;
        case ACT_TWIRL_LAND:              sp24 = act_twirl_land(m);                       break;
        case ACT_TRIPLE_JUMP_LAND_STOP:   sp24 = act_triple_jump_land_stop(m);            break;
        case ACT_BACKFLIP_LAND_STOP:      sp24 = act_backflip_land_stop(m);               break;
        case ACT_LONG_JUMP_LAND_STOP:     sp24 = act_long_jump_land_stop(m);              break;
        case ACT_GROUND_POUND_LAND:       sp24 = act_ground_pound_land(m);                break;
        case ACT_BRAKING_STOP:            sp24 = act_braking_stop(m);                     break;
        case ACT_BUTT_SLIDE_STOP:         sp24 = act_butt_slide_stop(m);                  break;
        case ACT_HOLD_BUTT_SLIDE_STOP:    sp24 = act_hold_butt_slide_stop(m);             break;
    }
    /* clang-format on */

    if (!sp24) {
        if (m->input & INPUT_IN_WATER) {
            m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
        }
    }

    return sp24;
}
