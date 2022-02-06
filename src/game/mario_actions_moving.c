#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario.h"
#include "audio/external.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "mario_step.h"
#include "area.h"
#include "interaction.h"
#include "mario_actions_object.h"
#include "memory.h"
#include "behavior_data.h"
#include "thread6.h"
#include "object_helpers.h"
#include "OPT_FOR_SIZE.h"

struct LandingAction {
    s16 numFrames;
    s16 unk02;
    u32 verySteepAction;
    u32 endAction;
    u32 aPressedAction;
    u32 offFloorAction;
    u32 slideAction;
};

struct LandingAction sJumpLandAction = {
    4, 5, ACT_FREEFALL, ACT_JUMP_LAND_STOP, ACT_DOUBLE_JUMP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

struct LandingAction sFreefallLandAction = {
    4, 5, ACT_FREEFALL, ACT_FREEFALL_LAND_STOP, ACT_DOUBLE_JUMP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

struct LandingAction sSideFlipLandAction = {
    4, 5, ACT_FREEFALL, ACT_SIDE_FLIP_LAND_STOP, ACT_DOUBLE_JUMP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

struct LandingAction sHoldJumpLandAction = {
    4,
    5,
    ACT_HOLD_FREEFALL,
    ACT_HOLD_JUMP_LAND_STOP,
    ACT_HOLD_JUMP,
    ACT_HOLD_FREEFALL,
    ACT_HOLD_BEGIN_SLIDING,
};

struct LandingAction sHoldFreefallLandAction = {
    4,
    5,
    ACT_HOLD_FREEFALL,
    ACT_HOLD_FREEFALL_LAND_STOP,
    ACT_HOLD_JUMP,
    ACT_HOLD_FREEFALL,
    ACT_HOLD_BEGIN_SLIDING,
};

struct LandingAction sLongJumpLandAction = {
    6, 5, ACT_FREEFALL, ACT_LONG_JUMP_LAND_STOP, ACT_LONG_JUMP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

struct LandingAction sDoubleJumpLandAction = {
    4, 5, ACT_FREEFALL, ACT_DOUBLE_JUMP_LAND_STOP, ACT_JUMP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

struct LandingAction sTripleJumpLandAction = {
    4, 0, ACT_FREEFALL, ACT_TRIPLE_JUMP_LAND_STOP, ACT_UNINITIALIZED, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

struct LandingAction sBackflipLandAction = {
    4, 0, ACT_FREEFALL, ACT_BACKFLIP_LAND_STOP, ACT_BACKFLIP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};

Mat4 D_80339F50[2];

s16 tilt_body_running(struct MarioState *m) {
    if (m->floor == &gWaterSurfacePseudoFloor) {
        return 0;
    }
    s16 pitch = find_floor_slope(m, 0);
    pitch = pitch * m->forwardVel / 40.0f;
    return -pitch;
}

void play_step_sound(struct MarioState *m, s16 frame1, s16 frame2) {
    if (is_anim_past_frame(m, frame1) || is_anim_past_frame(m, frame2)) {
        if (m->quicksandDepth > 50.0f) {
            play_sound(SOUND_ACTION_QUICKSAND_STEP, m->marioObj->header.gfx.cameraToObject);
        } else if (m->marioObj->header.gfx.unk38.animID == MARIO_ANIM_TIPTOE) {
            play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_STEP_TIPTOE, 0);
        } else {
            play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_STEP, 0);
        }
    }
}

#define ROTSPEED 0x400
void align_with_floor(struct MarioState *m) {
    m->marioObj->header.gfx.angle[0] = m->faceAngle[0] = approach_s16_symmetric(
        m->faceAngle[0],
        coss(m->faceAngle[1] - atan2s(m->floor->normal.z, m->floor->normal.x))
            * atan2s(m->floor->normal.y, sqrtf(m->floor->normal.x * m->floor->normal.x
                                               + m->floor->normal.z * m->floor->normal.z)),
        ROTSPEED);
    m->marioObj->header.gfx.angle[2] = m->faceAngle[2] = approach_s16_symmetric(
        m->faceAngle[2],
        sins(m->faceAngle[1] - atan2s(m->floor->normal.z, m->floor->normal.x))
            * atan2s(m->floor->normal.y, sqrtf(m->floor->normal.x * m->floor->normal.x
                                               + m->floor->normal.z * m->floor->normal.z)),
        ROTSPEED);
}

s32 begin_walking_action(struct MarioState *m, f32 forwardVel, u32 action, u32 actionArg) {
    m->faceAngle[1] = m->intendedYaw;
    mario_set_forward_vel(m, forwardVel);
    return set_mario_action(m, action, actionArg);
}

void check_ledge_climb_down(struct MarioState *m) {
    struct WallCollisionData wallCols;
    struct Surface *floor;
    f32 floorHeight;
    struct Surface *wall;
    s16 wallAngle;
    s16 wallDYaw;

    if (m->forwardVel < 10.0f) {
        wallCols.x = m->pos[0];
        wallCols.y = m->pos[1];
        wallCols.z = m->pos[2];
        wallCols.radius = 10.0f;
        wallCols.offsetY = -10.0f;

        if (find_wall_collisions(&wallCols) != 0) {
            floorHeight = find_floor(wallCols.x, wallCols.y, wallCols.z, &floor);
            if (floor != NULL) {
                if (wallCols.y - floorHeight > 160.0f) {
                    wall = wallCols.walls[wallCols.numWalls - 1];
                    wallAngle = atan2s(wall->normal.z, wall->normal.x);
                    wallDYaw = wallAngle - m->faceAngle[1];

                    if (wallDYaw > -0x4000 && wallDYaw < 0x4000) {
                        m->pos[0] = wallCols.x - 20.0f * wall->normal.x;
                        m->pos[2] = wallCols.z - 20.0f * wall->normal.z;

                        m->faceAngle[0] = 0;
                        m->faceAngle[1] = wallAngle + 0x8000;

                        set_mario_action(m, ACT_LEDGE_CLIMB_DOWN, 0);
                        set_mario_animation(m, MARIO_ANIM_CLIMB_DOWN_LEDGE);
                    }
                }
            }
        }
    }
}

void slide_bonk(struct MarioState *m, u32 fastAction, u32 slowAction) {
    if (m->forwardVel > 16.0f) {
        mario_bonk_reflection(m, TRUE);
        drop_and_set_mario_action(m, fastAction, 0);
    } else {
        mario_set_forward_vel(m, 0.0f);
        set_mario_action(m, slowAction, 0);
    }
}

s32 set_triple_jump_action(struct MarioState *m) {
    if (m->flags & MARIO_WING_CAP) {
        return set_mario_action(m, ACT_FLYING_TRIPLE_JUMP, 0);
    } else if (m->forwardVel > 20.0f) {
        return set_mario_action(m, ACT_TRIPLE_JUMP, 0);
    } else {
        return set_mario_action(m, ACT_JUMP, 0);
    }

    return 0;
}

void update_sliding_angle(struct MarioState *m, f32 accel, f32 lossFactor) {
    s32 newFacingDYaw;
    s16 facingDYaw;

    struct Surface *floor = m->floor;
    s16 slopeAngle = atan2s(floor->normal.z, floor->normal.x);
    f32 steepness = sqrtf(1.f - floor->normal.y * floor->normal.y);

    m->slideVelX += accel * steepness * sins(slopeAngle);
    m->slideVelZ += accel * steepness * coss(slopeAngle);

    m->slideVelX *= lossFactor;
    m->slideVelZ *= lossFactor;

    m->slideYaw = atan2s(m->slideVelZ, m->slideVelX);

    facingDYaw = m->faceAngle[1] - m->slideYaw;
    newFacingDYaw = facingDYaw;

    //! -0x4000 not handled - can slide down a slope while facing perpendicular to it
    if (newFacingDYaw > 0 && newFacingDYaw <= 0x4000) {
        if ((newFacingDYaw -= 0x200) < 0) {
            newFacingDYaw = 0;
        }
    } else if (newFacingDYaw > -0x4000 && newFacingDYaw < 0) {
        if ((newFacingDYaw += 0x200) > 0) {
            newFacingDYaw = 0;
        }
    } else if (newFacingDYaw > 0x4000 && newFacingDYaw < 0x8000) {
        if ((newFacingDYaw += 0x200) > 0x8000) {
            newFacingDYaw = 0x8000;
        }
    } else if (newFacingDYaw > -0x8000 && newFacingDYaw <= -0x4000) {
        if ((newFacingDYaw -= 0x200) < -0x8000) {
            newFacingDYaw = -0x8000;
        }
    }

    m->faceAngle[1] = m->slideYaw + newFacingDYaw;

    m->vel[0] = m->slideVelX;
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ;

    mario_update_moving_sand(m);

    //! Speed is capped a frame late (butt slide HSG)
    m->forwardVel = sqrtf(m->slideVelX * m->slideVelX + m->slideVelZ * m->slideVelZ);
    if (m->forwardVel > 100.0f) {
        m->slideVelX = m->slideVelX * 100.0f / m->forwardVel;
        m->slideVelZ = m->slideVelZ * 100.0f / m->forwardVel;
    }

    if (newFacingDYaw < -0x4000 || newFacingDYaw > 0x4000) {
        m->forwardVel *= -1.0f;
    }
}

s32 update_sliding(struct MarioState *m, f32 stopSpeed) {
    f32 lossFactor;
    f32 accel;
    f32 oldSpeed;
    f32 newSpeed;

    s16 intendedDYaw = m->intendedYaw - m->slideYaw;
    f32 forward = coss(intendedDYaw);
    f32 sideward = sins(intendedDYaw);

    //! 10k glitch
    if (forward < 0.0f && m->forwardVel >= 0.0f) {
        forward *= 0.5f + 0.5f * m->forwardVel / 100.0f;
    }

    switch (mario_get_floor_class(m->floor)) {
        case SURFACE_CLASS_VERY_SLIPPERY:
            accel = 10.0f;
            lossFactor = m->intendedMag / 32.0f * forward * 0.02f + 0.98f;
            break;

        case SURFACE_CLASS_SLIPPERY:
            accel = 8.0f;
            lossFactor = m->intendedMag / 32.0f * forward * 0.02f + 0.96f;
            break;

        default:
            accel = 7.0f;
            lossFactor = m->intendedMag / 32.0f * forward * 0.02f + 0.92f;
            break;

        case SURFACE_CLASS_NOT_SLIPPERY:
            accel = 5.0f;
            lossFactor = m->intendedMag / 32.0f * forward * 0.02f + 0.92f;
            break;
    }

    oldSpeed = sqrtf(m->slideVelX * m->slideVelX + m->slideVelZ * m->slideVelZ);

    //! This is attempting to use trig derivatives to rotate Mario's speed.
    // It is slightly off/asymmetric since it uses the new X speed, but the old
    // Z speed.
    m->slideVelX += m->slideVelZ * (m->intendedMag / 32.0f) * sideward * 0.05f;
    m->slideVelZ -= m->slideVelX * (m->intendedMag / 32.0f) * sideward * 0.05f;

    newSpeed = sqrtf(m->slideVelX * m->slideVelX + m->slideVelZ * m->slideVelZ);

    if (oldSpeed > 0.0f && newSpeed > 0.0f) {
        m->slideVelX = m->slideVelX * oldSpeed / newSpeed;
        m->slideVelZ = m->slideVelZ * oldSpeed / newSpeed;
    }

    update_sliding_angle(m, accel, lossFactor);

    if (!mario_floor_is_slippery(m) && m->forwardVel * m->forwardVel < stopSpeed * stopSpeed) {
        mario_set_forward_vel(m, 0.0f);
        return TRUE;
    }

    return 0;
}

void apply_slope_accel(struct MarioState *m) {
    f32 slopeAccel;
    struct Surface *floor = m->floor;
    f32 steepness = sqrtf(1.f - floor->normal.y * floor->normal.y);
    s16 floorDYaw = m->floorAngle - m->faceAngle[1];

    if (mario_floor_is_slippery(m)) {
        s16 slopeClass = 0;

        if (m->action != ACT_SOFT_BACKWARD_GROUND_KB && m->action != ACT_SOFT_FORWARD_GROUND_KB) {
            slopeClass = mario_get_floor_class(m->floor);
        }

        switch (slopeClass) {
            case SURFACE_CLASS_VERY_SLIPPERY:
                slopeAccel = 5.3f;
                if (m->forwardVel > 0.f) {
                    m->forwardVel *= .9f;
                }
                break;
            case SURFACE_CLASS_SLIPPERY:
                slopeAccel = 2.7f;
                break;
            default:
                slopeAccel = 1.7f;
                break;
            case SURFACE_CLASS_NOT_SLIPPERY:
                slopeAccel = 0.0f;
                floorDYaw = 0;
                break;
        }

        m->forwardVel += slopeAccel * steepness * coss(floorDYaw);
    }

    m->slideYaw = m->faceAngle[1];

    m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);

    m->vel[0] = m->slideVelX;
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ;

    mario_update_moving_sand(m);
}

s32 apply_landing_accel(struct MarioState *m, f32 frictionFactor) {
    apply_slope_accel(m);

    if (!mario_floor_is_slippery(m)) {
        m->forwardVel *= frictionFactor;
        if (m->forwardVel * m->forwardVel < 1.0f) {
            mario_set_forward_vel(m, 0.0f);
            return TRUE;
        }
    }

    return 0;
}

void update_shell_speed(struct MarioState *m) {
    f32 targetSpeed;

    if (m->floorHeight < m->waterLevel) {
        m->floorHeight = m->waterLevel;
        m->floor = &gWaterSurfacePseudoFloor;
        m->floor->originOffset = m->waterLevel; //! Negative origin offset
    }

    targetSpeed = m->intendedMag * 2.0f;
    if (targetSpeed < 24.0f) {
        targetSpeed = 24.0f;
    }

    if (m->forwardVel <= 0.0f) {
        m->forwardVel += 1.1f;
    } else if (m->forwardVel <= targetSpeed) {
        m->forwardVel += 1.1f - m->forwardVel / 58.0f;
    } else if (m->floor->normal.y >= 0.95f) {
        m->forwardVel -= 1.0f;
    }
    m->faceAngle[1] =
        m->intendedYaw - approach_s32((s16) (m->intendedYaw - m->faceAngle[1]), 0, 0x800, 0x800);

    apply_slope_accel(m);
}

s32 apply_slope_decel(struct MarioState *m, f32 decelCoef) {
    f32 decel;
    s32 stopped = FALSE;

    switch (mario_get_floor_class(m->floor)) {
        case SURFACE_CLASS_VERY_SLIPPERY:
            decel = decelCoef * 0.2f;
            break;
        case SURFACE_CLASS_SLIPPERY:
            decel = decelCoef * 0.7f;
            break;
        default:
            decel = decelCoef * 2.0f;
            break;
        case SURFACE_CLASS_NOT_SLIPPERY:
            decel = decelCoef * 3.0f;
            break;
    }

    if ((m->forwardVel = approach_f32(m->forwardVel, 0.0f, decel, decel)) == 0.0f) {
        stopped = TRUE;
    }

    apply_slope_accel(m);
    return stopped;
}

s32 update_decelerating_speed(struct MarioState *m) {
    s32 stopped = FALSE;

    if ((m->forwardVel = approach_f32(m->forwardVel, 0.0f, 1.0f, 1.0f)) == 0.0f) {
        stopped = TRUE;
    }

    mario_set_forward_vel(m, m->forwardVel);
    mario_update_moving_sand(m);

    return stopped;
}

void update_walking_speed(struct MarioState *m) {
    f32 targetSpeed = m->intendedMag;

    if (m->quicksandDepth > 10.0f) {
        targetSpeed *= 6.25 / m->quicksandDepth;
    }

    if (m->forwardVel <= 0.0f) {
        m->forwardVel += 1.1f;
    } else if (m->forwardVel <= targetSpeed) {
        m->forwardVel += 1.1f - m->forwardVel / 43.0f;
    } else if (m->floor->normal.y >= 0.95f) {
        m->forwardVel -= 1.0f;
    }

    if (m->forwardVel > 48.0f) {
        m->forwardVel = 48.0f;
    }
    if ((m->forwardVel > 10.f) || (m->forwardVel < 0.f)) {
        m->faceAngle[1] =
            m->intendedYaw - approach_s32((s16) (m->intendedYaw - m->faceAngle[1]), 0, 0x800, 0x800);
    } else {
        m->faceAngle[1] = m->intendedYaw;
    }
    apply_slope_accel(m);
}

s32 should_begin_sliding(struct MarioState *m) {
    if (m->input & INPUT_ABOVE_SLIDE) {
        s32 movingBackward = m->forwardVel <= -1.0f;

        if (movingBackward || mario_facing_downhill(m, FALSE)) {
            return TRUE;
        }
    }

    return FALSE;
}

s32 analog_stick_held_back(struct MarioState *m) {
    s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
    return intendedDYaw < -0x471C || intendedDYaw > 0x471C;
}

s32 check_ground_dive_or_punch(struct MarioState *m) {
    if (m->input & INPUT_B_PRESSED) {
        //! Speed kick (shoutouts to SimpleFlips)
        if (m->forwardVel >= 29.0f && m->controller->stickMag > 48.0f) {
            m->vel[1] = 20.0f;
            return set_mario_action(m, ACT_DIVE, 1);
        } else if (m->forwardVel >= 29.0f) {
            m->particleFlags |= PARTICLE_SPARKLES;
        }

        return set_mario_action(m, ACT_MOVE_PUNCHING, 0);
    }

    return FALSE;
}

s32 begin_braking_action(struct MarioState *m) {
    mario_drop_held_object(m);

    if (m->actionState == 1) {
        m->faceAngle[1] = m->actionArg;
        return set_mario_action(m, ACT_STANDING_AGAINST_WALL, 0);
    }

    if (m->forwardVel >= 16.0f && m->floor->normal.y >= 0.17364818f) {
        return set_mario_action(m, ACT_BRAKING, 0);
    }

    return set_mario_action(m, ACT_DECELERATING, 0);
}

void anim_and_audio_for_walk(struct MarioState *m) {
    s32 val14;
    s32 val0C = TRUE;
    s16 targetPitch = 0;
    f32 val04;

    val04 = m->intendedMag > m->forwardVel ? m->intendedMag : m->forwardVel;

    if (val04 < 4.0f) {
        val04 = 4.0f;
    }

    if (m->quicksandDepth > 50.0f) {
        val14 = (s32) (val04 / 4.0f * 0x10000);
        set_mario_anim_with_accel(m, MARIO_ANIM_MOVE_IN_QUICKSAND, val14);
        play_step_sound(m, 19, 93);
        m->actionTimer = 0;
    } else {
        while (val0C) {
            switch (m->actionTimer) {
                case 0:
                    if (val04 > 8.0f) {
                        m->actionTimer = 2;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        if ((val14 = (s32) (val04 / 4.0f * 0x10000)) < 0x1000) {
                            val14 = 0x1000;
                        }
                        set_mario_anim_with_accel(m, MARIO_ANIM_START_TIPTOE, val14);
                        play_step_sound(m, 7, 22);
                        if (is_anim_past_frame(m, 23)) {
                            m->actionTimer = 2;
                        }

                        val0C = FALSE;
                    }
                    break;

                case 1:
                    if (val04 > 8.0f) {
                        m->actionTimer = 2;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        if ((val14 = (s32) (val04 * 0x10000)) < 0x1000) {
                            val14 = 0x1000;
                        }
                        set_mario_anim_with_accel(m, MARIO_ANIM_TIPTOE, val14);
                        play_step_sound(m, 14, 72);

                        val0C = FALSE;
                    }
                    break;

                case 2:
                    if (val04 < 5.0f) {
                        m->actionTimer = 1;
                    } else if (val04 > 22.0f) {
                        m->actionTimer = 3;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        val14 = (s32) (val04 / 4.0f * 0x10000);
                        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, val14);
                        play_step_sound(m, 10, 49);

                        val0C = FALSE;
                    }
                    break;

                case 3:
                    if (val04 < 18.0f) {
                        m->actionTimer = 2;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        val14 = (s32) (val04 / 4.0f * 0x10000);
                        set_mario_anim_with_accel(m, MARIO_ANIM_RUNNING, val14);
                        play_step_sound(m, 9, 45);
                        targetPitch = tilt_body_running(m);

                        val0C = FALSE;
                    }
                    break;
            }
        }
    }

    m->marioObj->oMarioWalkingPitch =
        (s16) approach_s32(m->marioObj->oMarioWalkingPitch, targetPitch, 0x800, 0x800);
    m->marioObj->header.gfx.angle[0] = m->marioObj->oMarioWalkingPitch;
}

void anim_and_audio_for_hold_walk(struct MarioState *m) {
    s32 val0C;
    s32 val08 = TRUE;
    f32 val04;

    val04 = m->intendedMag > m->forwardVel ? m->intendedMag : m->forwardVel;
    val04 *= 0.5f;
    if (val04 < 2.0f) {
        val04 = 2.0f;
    }

    while (val08) {
        switch (m->actionTimer) {
            case 0:
                if (val04 > 6.0f) {
                    m->actionTimer = 1;
                } else {
                    //! (Speed Crash) Crashes if Mario's speed exceeds or equals 2^15.
                    val0C = (s32) (val04 * 0x10000);
                    set_mario_anim_with_accel(m, MARIO_ANIM_SLOW_WALK_WITH_LIGHT_OBJ, val0C);
                    play_step_sound(m, 12, 62);

                    val08 = FALSE;
                }
                break;

            case 1:
                if (val04 < 3.0f) {
                    m->actionTimer = 0;
                } else if (val04 > 11.0f) {
                    m->actionTimer = 2;
                } else {
                    //! (Speed Crash) Crashes if Mario's speed exceeds or equals 2^15.
                    val0C = (s32) (val04 * 0x10000);
                    set_mario_anim_with_accel(m, MARIO_ANIM_WALK_WITH_LIGHT_OBJ, val0C);
                    play_step_sound(m, 12, 62);

                    val08 = FALSE;
                }
                break;

            case 2:
                if (val04 < 8.0f) {
                    m->actionTimer = 1;
                } else {
                    //! (Speed Crash) Crashes if Mario's speed exceeds or equals 2^16.
                    val0C = (s32) (val04 / 2.0f * 0x10000);
                    set_mario_anim_with_accel(m, MARIO_ANIM_RUN_WITH_LIGHT_OBJ, val0C);
                    play_step_sound(m, 10, 49);

                    val08 = FALSE;
                }
                break;
        }
    }
}

void anim_and_audio_for_heavy_walk(struct MarioState *m) {
    set_mario_anim_with_accel(m, MARIO_ANIM_WALK_WITH_HEAVY_OBJ, (m->intendedMag * 0x10000));
    play_step_sound(m, 26, 79);
}

void push_or_sidle_wall(struct MarioState *m, Vec3f startPos) {
    s16 wallAngle;
    s16 dWallAngle;
    f32 dx = m->pos[0] - startPos[0];
    f32 dz = m->pos[2] - startPos[2];
    f32 movedDistance = sqrtf(dx * dx + dz * dz);
    //! (Speed Crash) If a wall is after moving 16384 distance, this crashes.
    s32 val04 = (s32) (movedDistance * 2.0f * 0x10000);

    if (m->forwardVel > 6.0f) {
        mario_set_forward_vel(m, 6.0f);
    }

    if (m->wall != NULL) {
        wallAngle = atan2s(m->wall->normal.z, m->wall->normal.x);
        dWallAngle = wallAngle - m->faceAngle[1];
    }

    if (m->wall == NULL || dWallAngle <= -0x71C8 || dWallAngle >= 0x71C8) {
        if (!mario_can_get_up(m)) { // CuckyDev: Force Mario into crouching state when under ceiling
            set_mario_action(m, ACT_START_CROUCHING, 0);
            return;
        }

        m->flags |= MARIO_UNKNOWN_31;
        set_mario_animation(m, MARIO_ANIM_PUSHING);
        play_step_sound(m, 6, 18);
    } else {
        if (dWallAngle < 0) {
            set_mario_anim_with_accel(m, MARIO_ANIM_SIDESTEP_RIGHT, val04);
        } else {
            set_mario_anim_with_accel(m, MARIO_ANIM_SIDESTEP_LEFT, val04);
        }

        if (m->marioObj->header.gfx.unk38.animFrame < 20) {
            play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);
        }

        m->actionState = 1;
        m->actionArg = wallAngle + 0x8000;
        m->marioObj->header.gfx.angle[1] = wallAngle + 0x8000;
        m->marioObj->header.gfx.angle[2] = find_floor_slope(m, 0x4000);
    }
}

void tilt_body_walking(struct MarioState *m, s16 startYaw) {
    struct MarioBodyState *val0C = m->marioBodyState;
    s16 animID = m->marioObj->header.gfx.unk38.animID;
    s16 dYaw;
    s16 val02;
    s16 val00;

    dYaw = m->faceAngle[1] - startYaw;
    //! (Speed Crash) These casts can cause a crash if (dYaw * forwardVel / 12) or
    //! (forwardVel * 170) exceed or equal 2^31.
    val02 = -(s16) (dYaw * m->forwardVel / 12.0f);
    val00 = (s16) (m->forwardVel * 170.0f);

    if (val02 > 0x1555) {
        val02 = 0x1555;
    }
    if (val02 < -0x1555) {
        val02 = -0x1555;
    }

    if (val00 > 0x1555) {
        val00 = 0x1555;
    }
    if (val00 < 0) {
        val00 = 0;
    }

    val0C->torsoAngle[2] = approach_s32(val0C->torsoAngle[2], val02, 0x400, 0x400);
    val0C->torsoAngle[0] = approach_s32(val0C->torsoAngle[0], val00, 0x400, 0x400);
}

void tilt_body_ground_shell(struct MarioState *m, s16 startYaw) {
    struct MarioBodyState *val0C = m->marioBodyState;
    s16 dYaw = m->faceAngle[1] - startYaw;
    //! (Speed Crash) These casts can cause a crash if (dYaw * forwardVel / 12) or
    //! (forwardVel * 170) exceed or equal 2^31. Harder (if not impossible to do)
    //! while on a Koopa Shell making this less of an issue.
    s16 val04 = -(s16) (dYaw * m->forwardVel / 12.0f);
    s16 val02 = (s16) (m->forwardVel * 170.0f);

    if (val04 > 0x1800) {
        val04 = 0x1800;
    }
    if (val04 < -0x1800) {
        val04 = -0x1800;
    }

    if (val02 > 0x1000) {
        val02 = 0x1000;
    }
    if (val02 < 0) {
        val02 = 0;
    }

    val0C->torsoAngle[2] = approach_s32(val0C->torsoAngle[2], val04, 0x200, 0x200);
    val0C->torsoAngle[0] = approach_s32(val0C->torsoAngle[0], val02, 0x200, 0x200);
    val0C->headAngle[2] = -val0C->torsoAngle[2];

    m->marioObj->header.gfx.angle[2] = val0C->torsoAngle[2];
    m->marioObj->header.gfx.pos[1] += 45.0f;
}

s32 act_walking(struct MarioState *m) {
    Vec3f startPos;
    s16 startYaw = m->faceAngle[1];

    mario_drop_held_object(m);

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return begin_braking_action(m);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jump_from_landing(m);
    }

    if (check_ground_dive_or_punch(m)) {
        return TRUE;
    }

    if (m->input & INPUT_UNKNOWN_5) {
        return begin_braking_action(m);
    }

    if (analog_stick_held_back(m) && m->forwardVel >= 16.0f) {
        return set_mario_action(m, ACT_TURNING_AROUND, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    vec3f_copy(startPos, m->pos);
    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
            break;

        case GROUND_STEP_NONE:
            anim_and_audio_for_walk(m);
            if (m->intendedMag - m->forwardVel > 16.0f) {
                m->particleFlags |= PARTICLE_DUST;
            }
            break;

        case GROUND_STEP_HIT_WALL:
            push_or_sidle_wall(m, startPos);
            m->actionTimer = 0;
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    m->faceAngle[0] = 0;
    check_ledge_climb_down(m);
    tilt_body_walking(m, startYaw);
    return FALSE;
}

s32 act_move_punching(struct MarioState *m) {
    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->actionState == 0 && (m->input & INPUT_A_DOWN)) {
        return set_mario_action(m, ACT_JUMP_KICK, 0);
    }

    m->actionState = 1;

    mario_update_punch_sequence(m);

    if (m->forwardVel >= 0.0f) {
        apply_slope_decel(m, 0.5f);
    } else {
        if ((m->forwardVel += 8.0f) >= 0.0f) {
            m->forwardVel = 0.0f;
        }
        apply_slope_accel(m);
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            m->particleFlags |= PARTICLE_DUST;
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    return FALSE;
}

s32 act_hold_walking(struct MarioState *m) {
    if (m->heldObj->behavior == segmented_to_virtual(bhvJumpingBox)) {
        return set_mario_action(m, ACT_CRAZY_BOX_BOUNCE, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WALKING, 0);
    }

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_HOLD_JUMP, 0);
    }

    if (m->input & INPUT_UNKNOWN_5) {
        return set_mario_action(m, ACT_HOLD_DECELERATING, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return drop_and_set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    m->intendedMag *= 0.8f;

    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_HOLD_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (m->forwardVel > 32.0f) {
                mario_set_forward_vel(m, 32.0f);
            }
            break;
    }

    anim_and_audio_for_hold_walk(m);

    if (0.8f * m->intendedMag - m->forwardVel > 10.0f) {
        m->particleFlags |= PARTICLE_DUST;
    }

    return FALSE;
}

s32 act_hold_heavy_walking(struct MarioState *m) {
    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_HEAVY_THROW, 0);
    }

    if (should_begin_sliding(m)) {
        return drop_and_set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_UNKNOWN_5) {
        return set_mario_action(m, ACT_HOLD_HEAVY_IDLE, 0);
    }

    m->intendedMag *= 0.1f;

    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            drop_and_set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (m->forwardVel > 10.0f) {
                mario_set_forward_vel(m, 10.0f);
            }
            break;
    }

    anim_and_audio_for_heavy_walk(m);
    return FALSE;
}

s32 act_turning_around(struct MarioState *m) {
    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_SIDE_FLIP, 0);
    }

    if (m->input & INPUT_UNKNOWN_5) {
        return set_mario_action(m, ACT_BRAKING, 0);
    }

    if (!analog_stick_held_back(m)) {
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (apply_slope_decel(m, 2.0f)) {
        return begin_walking_action(m, 8.0f, ACT_FINISH_TURNING_AROUND, 0);
    }

    play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);

    adjust_sound_for_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    if (m->forwardVel >= 18.0f) {
        set_mario_animation(m, MARIO_ANIM_TURNING_PART1);
    } else {
        set_mario_animation(m, MARIO_ANIM_TURNING_PART2);
        if (is_anim_at_end(m)) {
            if (m->forwardVel > 0.0f) {
                begin_walking_action(m, -m->forwardVel, ACT_WALKING, 0);
            } else {
                begin_walking_action(m, 8.0f, ACT_WALKING, 0);
            }
        }
    }

    return FALSE;
}

s32 act_finish_turning_around(struct MarioState *m) {
    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_SIDE_FLIP, 0);
    }

    update_walking_speed(m);
    set_mario_animation(m, MARIO_ANIM_TURNING_PART2);

    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_WALKING, 0);
    }

    m->marioObj->header.gfx.angle[1] += 0x8000;
    return FALSE;
}

s32 act_braking(struct MarioState *m) {
    if (!(m->input & INPUT_FIRST_PERSON)
        && (m->input
            & (INPUT_NONZERO_ANALOG | (INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE))) {
        return check_common_action_exits(m);
    }

    if (apply_slope_decel(m, 2.0f)) {
        return set_mario_action(m, ACT_BRAKING_STOP, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_MOVE_PUNCHING, 0);
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            break;

        case GROUND_STEP_HIT_WALL:
            slide_bonk(m, ACT_BACKWARD_GROUND_KB, ACT_BRAKING_STOP);
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);
    adjust_sound_for_speed(m);
    set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
    return FALSE;
}

s32 act_grinding(struct MarioState *m) {
    struct Surface *floor;
    Vec3f nextPos;
    u16 direction = atan2s(m->vel[2], m->vel[0]);
    f32 minO, maxO;
    s16 lineYaw;
    f32 biggestLine[4];
    f32 smallestLine[4];

    if (!m->actionTimer) {
        m->forwardVel =
            absf(m->forwardVel * coss(direction - atan2s(m->floor->normal.z, m->floor->normal.x)));
        // m->faceAngle[1] = atan2s(m->floor->normal.z, m->floor->normal.x);
    }
    m->actionTimer++;
    if ((m->input & ((INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE))) {
        return check_common_action_exits(m);
    }
    if (m->actionTimer > 3) {
        if (m->controller->buttonPressed & B_BUTTON) {
            m->forwardVel += 9.f;
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            m->actionTimer = 2;
            play_sound(SOUND_MARIO_YAH_WAH_HOO, m->marioObj->header.gfx.cameraToObject);
        }
    }
    // position into middle of triangle, take the min and max offset orthogonal offset and approach
    // middle
#define SIDELENGTH(v1, v2) ((f32)(sqr(v1[0] - v2[0]) + sqr(v1[2] - v2[2])))
    biggestLine[0] = m->floor->vertex1[0];
    biggestLine[1] = m->floor->vertex1[2];
    biggestLine[2] = m->floor->vertex2[0];
    biggestLine[3] = m->floor->vertex2[2];
    smallestLine[0] = m->floor->vertex1[0];
    smallestLine[1] = m->floor->vertex1[2];
    smallestLine[2] = m->floor->vertex2[0];
    smallestLine[3] = m->floor->vertex2[2];

    if (SIDELENGTH(m->floor->vertex1, m->floor->vertex2)
        > SIDELENGTH(m->floor->vertex1, m->floor->vertex3)) {
        smallestLine[2] = m->floor->vertex3[0];
        smallestLine[3] = m->floor->vertex3[2];
    } else {
        biggestLine[2] = m->floor->vertex3[0];
        biggestLine[3] = m->floor->vertex3[2];
    }
    if (SIDELENGTH(m->floor->vertex2, m->floor->vertex3)
        > ((f32)(sqr(biggestLine[0] - biggestLine[2]) + sqr(biggestLine[1] - biggestLine[3])))) {
        biggestLine[0] = m->floor->vertex2[0];
        biggestLine[1] = m->floor->vertex2[2];
        biggestLine[2] = m->floor->vertex3[0];
        biggestLine[3] = m->floor->vertex3[2];
    } else if (SIDELENGTH(m->floor->vertex2, m->floor->vertex3)
               < ((sqr(smallestLine[0] - smallestLine[2]) + sqr(smallestLine[1] - smallestLine[3])))) {
        smallestLine[0] = m->floor->vertex2[0];
        smallestLine[1] = m->floor->vertex2[2];
        smallestLine[2] = m->floor->vertex3[0];
        smallestLine[3] = m->floor->vertex3[2];
    }
    lineYaw = atan2s((biggestLine[3] + biggestLine[1] - smallestLine[3] - smallestLine[1]),
                     (biggestLine[2] + biggestLine[0] - smallestLine[2] - smallestLine[0]));

    if (coss((u16) (-direction + lineYaw)) <= coss((u16) (-direction + lineYaw + 0x8000))) {
        m->faceAngle[1] = lineYaw + 0x8000;
        m->forwardVel = approach_f32(m->forwardVel, 80.f, -8.f * (1 - m->floor->normal.y), 1.f);
    } else {
        m->faceAngle[1] = lineYaw;
        m->forwardVel = approach_f32(m->forwardVel, 80.f, 8.f * (1 - m->floor->normal.y), 1.f);
    }
#define VECDIST(vertex)                                                                                \
    (sins(m->faceAngle[1] + 0x4000) * (vertex[0] - m->pos[0])                                          \
     + coss(m->faceAngle[1] + 0x4000) * (vertex[2] - m->pos[2]))
    minO = VECDIST(m->floor->vertex1);
    maxO = VECDIST(m->floor->vertex1);
    if (minO > VECDIST(m->floor->vertex2)) {
        minO = VECDIST(m->floor->vertex2);
    } else if (maxO < VECDIST(m->floor->vertex2)) {
        maxO = VECDIST(m->floor->vertex2);
    }
    if (minO > VECDIST(m->floor->vertex3)) {
        minO = VECDIST(m->floor->vertex3);
    } else if (maxO < VECDIST(m->floor->vertex3)) {
        maxO = VECDIST(m->floor->vertex3);
    }
    m->pos[0] = approach_f32_symmetric(
        m->pos[0], m->pos[0] + sins(m->faceAngle[1] + 0x4000) * (minO + maxO), m->forwardVel * .2f);
    m->pos[2] = approach_f32_symmetric(
        m->pos[2], m->pos[2] + coss(m->faceAngle[1] + 0x4000) * (minO + maxO), m->forwardVel * .2f);

    m->vel[0] = m->forwardVel * sins(m->faceAngle[1]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[1]);

    m->slideVelX = m->vel[0];
    m->slideVelZ = m->vel[2];

    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    if (SURFACETYPE(m->floor) != 0x00A7) {
        return set_mario_action(m, ACT_BRAKING, 0);
    }
    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            break;

        case GROUND_STEP_HIT_WALL:
            slide_bonk(m, ACT_BACKWARD_GROUND_KB, ACT_BRAKING_STOP);
            break;
    }

    play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);
    adjust_sound_for_speed(m);
    set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
    return FALSE;
}

s32 act_decelerating(struct MarioState *m) {
    s32 val0C;
    s16 slopeClass = mario_get_floor_class(m->floor);

    if (!(m->input & INPUT_FIRST_PERSON)) {
        if (should_begin_sliding(m)) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }

        if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
            return set_jump_from_landing(m);
        }

        if (check_ground_dive_or_punch(m)) {
            return TRUE;
        }

        if (m->input & INPUT_NONZERO_ANALOG) {
            return set_mario_action(m, ACT_WALKING, 0);
        }

        if (m->input & INPUT_Z_PRESSED) {
            return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
        }
    }

    if (update_decelerating_speed(m)) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
                mario_bonk_reflection(m, TRUE);
            } else {
                mario_set_forward_vel(m, 0.0f);
            }
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
        set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_LEFT);
        play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);
        adjust_sound_for_speed(m);
    } else {
        // (Speed Crash) Crashes if speed exceeds 2^17.
        if ((val0C = (s32) (m->forwardVel / 4.0f * 0x10000)) < 0x1000) {
            val0C = 0x1000;
        }

        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, val0C);
        play_step_sound(m, 10, 49);
    }

    return FALSE;
}

s32 act_hold_decelerating(struct MarioState *m) {
    s16 slopeClass = mario_get_floor_class(m->floor);

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WALKING, 0);
    }

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_jumping_action(m, ACT_HOLD_JUMP, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return drop_and_set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_WALKING, 0);
    }

    if (update_decelerating_speed(m)) {
        return set_mario_action(m, ACT_HOLD_IDLE, 0);
    }

    m->intendedMag *= 0.4f;

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_HOLD_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
                mario_bonk_reflection(m, TRUE);
            } else {
                mario_set_forward_vel(m, 0.0f);
            }
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
        set_mario_animation(m, MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
        play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);
        adjust_sound_for_speed(m);
    } else {
        set_mario_anim_with_accel(m, MARIO_ANIM_WALK_WITH_LIGHT_OBJ,
                                  min(0x1000, (m->forwardVel * 0x10000)));
        play_step_sound(m, 12, 62);
    }

    return FALSE;
}

s32 act_riding_shell_ground(struct MarioState *m) {

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_mario_action(m, ACT_RIDING_SHELL_JUMP, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        mario_stop_riding_object(m);
        if (m->forwardVel < 24.0f) {
            mario_set_forward_vel(m, 24.0f);
        }
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    update_shell_speed(m);
    set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_START_RIDING_SHELL : MARIO_ANIM_RIDING_SHELL);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_RIDING_SHELL_FALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            mario_stop_riding_object(m);
            play_sound(SOUND_ACTION_BONK, m->marioObj->header.gfx.cameraToObject);
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
            break;
    }

    tilt_body_ground_shell(m, m->faceAngle[1]);
    if (m->floor->type & (SPECFLAG_BURNING << 8)) {
        play_sound(SOUND_MOVING_RIDING_SHELL_LAVA, m->marioObj->header.gfx.cameraToObject);
    } else {
        play_sound(SOUND_MOVING_TERRAIN_RIDING_SHELL + m->terrainSoundAddend,
                   m->marioObj->header.gfx.cameraToObject);
    }

    adjust_sound_for_speed(m);
#ifdef VERSION_SH
    reset_rumble_timers();
#endif
    return FALSE;
}

void tilt_body_minecart(struct MarioState *m) {
    struct MarioBodyState *val0C = m->marioBodyState;
    struct Object *marioObj = m->marioObj;
    s16 dYaw = m->faceAngle[1] - m->intendedYaw;
    s16 val04 = -(s16) (m->intendedMag * 130.0f * coss((dYaw + 0x4000) & 0xffff));
    s16 val02 = (s16) (m->intendedMag * 130.0f * coss(dYaw & 0xffff));

    val0C->torsoAngle[2] = approach_s16_symmetric(val0C->torsoAngle[2], val04, 0x200);
    val0C->torsoAngle[0] = approach_s16_symmetric(val0C->torsoAngle[0], val02, 0x200);
    val0C->headAngle[2] = -val0C->torsoAngle[2];
    val0C->headAngle[0] = -val0C->torsoAngle[0];

    marioObj->header.gfx.angle[2] = val0C->torsoAngle[2];
    marioObj->header.gfx.angle[0] = val0C->torsoAngle[0];
    marioObj->header.gfx.pos[1] += 45.0f;
}

extern u32 gAudioRandom;
s32 act_ride_minecart(struct MarioState *m) {
    s16 startYaw = m->faceAngle[1];
    struct MarioBodyState *val0C = m->marioBodyState;
    struct Object *marioObj = m->marioObj;
    struct Object *cart = cur_obj_nearest_object_with_behavior(bhvYoshiCart);
    if (cart) {
        if (m->controller->buttonDown & A_BUTTON) {
            if (cart->oMoveFlags & OBJ_MOVE_ON_GROUND) {
                cart->oVelY = 50.f;
                play_sound(SOUND_MARIO_YAH_WAH_HOO + ((gAudioRandom % 3) << 16),
                           m->marioObj->header.gfx.cameraToObject);
                cart->oMoveFlags &= ~OBJ_MOVE_ON_GROUND;
            }
        } else {
            if (cart->oVelY > 0.f) {
                cart->oVelY = cart->oVelY / 2.0f;
            }
        }
    } else {
        cart = cur_obj_nearest_object_with_behavior(bhvBowserCart);
        if (!cart) {
            cart = cur_obj_nearest_object_with_behavior(bhvBowserCartOW);
        }
    }

    set_mario_animation(m, MARIO_ANIM_MINECART);
    if (m->marioObj->header.gfx.unk38.animFrame > 0) {
        m->marioObj->header.gfx.unk38.animFrame--;
    }
    m->pos[0] = cart->oPosX;
    m->pos[1] = cart->oPosY;
    m->pos[2] = cart->oPosZ;
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    m->marioObj->header.gfx.pos[1] += 45.f;
    vec3s_copy(m->marioObj->header.gfx.angle, cart->header.gfx.angle);
    vec3s_copy(m->faceAngle, cart->header.gfx.angle);
    // tilt_body_minecart(m);
    val0C->torsoAngle[2] = cart->oFaceAngleRoll;
    val0C->torsoAngle[0] = cart->oFaceAnglePitch;
    val0C->headAngle[2] = -val0C->torsoAngle[2];
    val0C->headAngle[0] = -val0C->torsoAngle[0];

    marioObj->header.gfx.angle[2] = val0C->torsoAngle[2];
    marioObj->header.gfx.angle[0] = val0C->torsoAngle[0];
    if (cart->oForwardVel > 1.0f) {
        play_sound(SOUND_MOVING_TERRAIN_RIDING_SHELL + (3 << 16),
                   m->marioObj->header.gfx.cameraToObject);
    }

    return FALSE;
}

s32 act_crawling(struct MarioState *m) {
    if (mario_can_get_up(m)) { // crawlspaces
        if (should_begin_sliding(m)) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }

        if (m->input & INPUT_FIRST_PERSON) {
            return set_mario_action(m, ACT_STOP_CRAWLING, 0);
        }

        if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
            return set_jumping_action(m, ACT_JUMP, 0);
        }

        if (check_ground_dive_or_punch(m)) {
            return TRUE;
        }
        if (!(m->input & INPUT_Z_DOWN)) {
            return set_mario_action(m, ACT_STOP_CRAWLING, 0);
        }
    }

    if (m->input & INPUT_UNKNOWN_5) {
        return set_mario_action(m, ACT_STOP_CRAWLING, 0);
    }

    m->intendedMag *= 0.2f;

    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (m->forwardVel > 12.0f) {
                mario_set_forward_vel(m, 12.0f);
            }
            //! Possibly unintended missing break

        case GROUND_STEP_NONE:
            align_with_floor(m);
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    set_mario_anim_with_accel(m, MARIO_ANIM_CRAWLING, (m->intendedMag * 2.0f * 0x10000));
    play_step_sound(m, 26, 79);
    return FALSE;
}

s32 act_burning_ground(struct MarioState *m) {
    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return set_mario_action(m, ACT_BURNING_JUMP, 0);
    }

    m->marioObj->oMarioBurnTimer += 2;
    if (m->marioObj->oMarioBurnTimer > 160) {
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (m->waterLevel - m->floorHeight > 50.0f) {
        play_sound(SOUND_GENERAL_FLAME_OUT, m->marioObj->header.gfx.cameraToObject);
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (m->forwardVel < 8.0f) {
        m->forwardVel = 8.0f;
    }
    if (m->forwardVel > 48.0f) {
        m->forwardVel = 48.0f;
    }

    m->forwardVel = approach_f32(m->forwardVel, 32.0f, 4.0f, 1.0f);

    if (m->input & INPUT_NONZERO_ANALOG) {
        m->faceAngle[1] =
            m->intendedYaw - approach_s32((s16) (m->intendedYaw - m->faceAngle[1]), 0, 0x600, 0x600);
    }

    apply_slope_accel(m);

    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_BURNING_FALL, 0);
    }

    set_mario_anim_with_accel(m, MARIO_ANIM_RUNNING, (s32) (m->forwardVel / 2.0f * 0x10000));
    play_step_sound(m, 9, 45);

    m->particleFlags |= PARTICLE_FIRE;
    play_sound(SOUND_MOVING_LAVA_BURN, m->marioObj->header.gfx.cameraToObject);

    m->health -= 10;
    if (m->health < 0x100) {
        set_mario_action(m, ACT_STANDING_DEATH, 0);
    }

    m->marioBodyState->eyeState = MARIO_EYES_DEAD;
#ifdef VERSION_SH
    reset_rumble_timers();
#endif
    return FALSE;
}

void tilt_body_butt_slide(struct MarioState *m) {
    s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
    m->marioBodyState->torsoAngle[0] = (s32) (5461.3335f * m->intendedMag / 32.0f * coss(intendedDYaw));
    m->marioBodyState->torsoAngle[2] =
        (s32) (-(5461.3335f * m->intendedMag / 32.0f * sins(intendedDYaw)));
}

void common_slide_action(struct MarioState *m, u32 endAction, u32 airAction, s32 animation) {
    play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);

#ifdef VERSION_SH
    reset_rumble_timers();
#endif

    adjust_sound_for_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, airAction, 0);
            if (m->forwardVel < -50.0f || 50.0f < m->forwardVel) {
                play_sound(SOUND_MARIO_HOOHOO, m->marioObj->header.gfx.cameraToObject);
            }
            break;

        case GROUND_STEP_NONE:
            set_mario_animation(m, animation);
            align_with_floor(m);
            break;

        case GROUND_STEP_HIT_WALL:
            if (!mario_floor_is_slippery(m)) {
                if (m->forwardVel > 16.0f) {
                    m->particleFlags |= PARTICLE_VERTICAL_STAR;
                }
                slide_bonk(m, ACT_GROUND_BONK, endAction);
            } else if (m->wall != NULL) {
                s16 wallAngle = atan2s(m->wall->normal.z, m->wall->normal.x);
                f32 slideSpeed = sqrtf(m->slideVelX * m->slideVelX + m->slideVelZ * m->slideVelZ);

                if ((slideSpeed *= 0.9) < 4.0f) {
                    slideSpeed = 4.0f;
                }

                m->slideYaw = wallAngle - (s16) (m->slideYaw - wallAngle) + 0x8000;

                m->vel[0] = m->slideVelX = slideSpeed * sins(m->slideYaw);
                m->vel[2] = m->slideVelZ = slideSpeed * coss(m->slideYaw);
            }

            align_with_floor(m);
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }
}

s32 common_slide_action_with_jump(struct MarioState *m, u32 stopAction, u32 jumpAction, u32 airAction,
                                  s32 animation) {
    if (m->actionTimer == 5) {
        if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
            return set_jumping_action(m, jumpAction, 0);
        }
    } else {
        m->actionTimer++;
    }

    if (update_sliding(m, 4.0f)) {
        return set_mario_action(m, stopAction, 0);
    }

    common_slide_action(m, stopAction, airAction, animation);
    return FALSE;
}

s32 act_butt_slide(struct MarioState *m) {
    s32 cancel = common_slide_action_with_jump(m, ACT_BUTT_SLIDE_STOP, ACT_JUMP, ACT_BUTT_SLIDE_AIR,
                                               MARIO_ANIM_SLIDE);
    tilt_body_butt_slide(m);
    return cancel;
}

s32 act_hold_butt_slide(struct MarioState *m) {
    s32 cancel;

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_BUTT_SLIDE, 0);
    }

    cancel = common_slide_action_with_jump(m, ACT_HOLD_BUTT_SLIDE_STOP, ACT_HOLD_JUMP,
                                           ACT_HOLD_BUTT_SLIDE_AIR,
                                           MARIO_ANIM_SLIDING_ON_BOTTOM_WITH_LIGHT_OBJ);
    tilt_body_butt_slide(m);
    return cancel;
}

s32 act_crouch_slide(struct MarioState *m) {
    s32 cancel;

    if (mario_can_get_up(m)) { // crawlspaces
        if (m->input & INPUT_ABOVE_SLIDE) {
            return set_mario_action(m, ACT_BUTT_SLIDE, 0);
        }

        if (m->actionTimer < 30) {
            m->actionTimer++;
            if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
                if (m->forwardVel > 10.0f) {
                    return set_jumping_action(m, ACT_LONG_JUMP, 0);
                }
            }
        }

        if (m->input & INPUT_B_PRESSED) {
            if (m->forwardVel >= 10.0f) {
                return set_mario_action(m, ACT_SLIDE_KICK, 0);
            } else {
                return set_mario_action(m, ACT_MOVE_PUNCHING, 0x0009);
            }
        }

        if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
            return set_jumping_action(m, ACT_JUMP, 0);
        }

        if (m->input & INPUT_FIRST_PERSON) {
            return set_mario_action(m, ACT_BRAKING, 0);
        }
        cancel = common_slide_action_with_jump(m, ACT_CROUCHING, ACT_JUMP, ACT_FREEFALL,
                                               MARIO_ANIM_START_CROUCHING);
    } else {
        cancel = common_slide_action_with_jump(m, ACT_CROUCHING, ACT_CROUCH_SLIDE, ACT_FREEFALL,
                                               MARIO_ANIM_START_CROUCHING);
    }
    return cancel;
}

s32 act_slide_kick_slide(struct MarioState *m) {
    if (m->input & ((INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_B_PRESSED)) { // CuckyDev: Rollout on B press
#ifdef VERSION_SH
        queue_rumble_data(5, 80);
#endif
        return set_jumping_action(m, ACT_FORWARD_ROLLOUT, 0);
    }

    set_mario_animation(m, MARIO_ANIM_SLIDE_KICK);
    if (is_anim_at_end(m) && m->forwardVel < 1.0f) {
        return set_mario_action(m, ACT_SLIDE_KICK_SLIDE_STOP, 0);
    }

    update_sliding(m, 1.0f);
    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 2);
            break;

        case GROUND_STEP_HIT_WALL:
            mario_bonk_reflection(m, TRUE);
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    play_sound_and_spawn_particles(m, SOUND_MOVING_TERRAIN_SLIDE, 0);
    return FALSE;
}

s32 stomach_slide_action(struct MarioState *m, u32 stopAction, u32 airAction, s32 animation) {
    if (m->actionTimer == 5) {
        if (!(m->input & INPUT_ABOVE_SLIDE) && (m->input & ((INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_B_PRESSED))) {
#ifdef VERSION_SH
            queue_rumble_data(5, 80);
#endif
            return drop_and_set_mario_action(
                m, m->forwardVel >= 0.0f ? ACT_FORWARD_ROLLOUT : ACT_BACKWARD_ROLLOUT, 0);
        }
    } else {
        m->actionTimer++;
    }

    if (update_sliding(m, 4.0f)) {
        return set_mario_action(m, stopAction, 0);
    }

    common_slide_action(m, stopAction, airAction, animation);
    return FALSE;
}

s32 act_stomach_slide(struct MarioState *m) {
    s32 cancel = stomach_slide_action(m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
    return cancel;
}

s32 act_hold_stomach_slide(struct MarioState *m) {
    s32 cancel;

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_STOMACH_SLIDE, 0);
    }

    cancel = stomach_slide_action(m, ACT_DIVE_PICKING_UP, ACT_HOLD_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
    return cancel;
}

s32 act_dive_slide(struct MarioState *m) {
    if (!(m->input & INPUT_ABOVE_SLIDE) && (m->input & ((INPUT_A_PRESSED | INPUT_A_LAST_FRAME) | INPUT_B_PRESSED))) {
#ifdef VERSION_SH
        queue_rumble_data(5, 80);
#endif
        if ((m->flags & MARIO_ACTION_SOUND_PLAYED) == 0) {
            m->particleFlags |= PARTICLE_SPARKLES;
        }
        return set_mario_action(m, m->forwardVel > 0.0f ? ACT_FORWARD_ROLLOUT : ACT_BACKWARD_ROLLOUT,
                                0);
    }

    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);

    //! If the dive slide ends on the same frame that we pick up on object,
    // Mario will not be in the dive slide action for the call to
    // mario_check_object_grab, and so will end up in the regular picking action,
    // rather than the picking up after dive action.

    if (update_sliding(m, 8.0f) && is_anim_at_end(m)) {
        mario_set_forward_vel(m, 0.0f);
        set_mario_action(m, ACT_STOMACH_SLIDE_STOP, 0);
    }

    if (mario_check_object_grab(m)) {
        mario_grab_used_object(m);
        m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
        return TRUE;
    }

    common_slide_action(m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_DIVE);
    return FALSE;
}

s32 common_ground_knockback_action(struct MarioState *m, s32 animation, s32 arg2, s32 arg3, s32 arg4) {
    s32 val04;

    if (arg3) {
        play_mario_heavy_landing_sound_once(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }

    if (arg4 > 0) {
        play_sound_if_no_flag(m, SOUND_MARIO_ATTACKED, MARIO_MARIO_SOUND_PLAYED);
    } else {
        play_sound_if_no_flag(m, SOUND_MARIO_OOOF2, MARIO_MARIO_SOUND_PLAYED);
    }

    if (m->forwardVel > 32.0f) {
        m->forwardVel = 32.0f;
    }
    if (m->forwardVel < -32.0f) {
        m->forwardVel = -32.0f;
    }

    val04 = set_mario_animation(m, animation);
    if (val04 < arg2) {
        apply_landing_accel(m, 0.9f);
    } else if (m->forwardVel >= 0.0f) {
        mario_set_forward_vel(m, 0.1f);
    } else {
        mario_set_forward_vel(m, -0.1f);
    }

    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        if (m->forwardVel >= 0.0f) {
            set_mario_action(m, ACT_FORWARD_AIR_KB, arg4);
        } else {
            set_mario_action(m, ACT_BACKWARD_AIR_KB, arg4);
        }
    } else if (is_anim_at_end(m)) {
        if (m->health < 0x100) {
            set_mario_action(m, ACT_STANDING_DEATH, 0);
        } else {
            if (arg4 > 0) {
                m->invincTimer = 30;
            }
            set_mario_action(m, ACT_IDLE, 0);
        }
    }

    return val04;
}

s32 act_hard_backward_ground_kb(struct MarioState *m) {
    s32 val04 =
        common_ground_knockback_action(m, MARIO_ANIM_FALL_OVER_BACKWARDS, 0x2B, TRUE, m->actionArg);
    if (val04 == 0x2B && m->health < 0x100) {
        set_mario_action(m, ACT_DEATH_ON_BACK, 0);
    } else if (val04 == 0x36 && m->prevAction == ACT_SPECIAL_DEATH_EXIT) {
        play_sound(SOUND_MARIO_MAMA_MIA, m->marioObj->header.gfx.cameraToObject);
    } else if (val04 == 0x45) {
        play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    return FALSE;
}

s32 act_hard_forward_ground_kb(struct MarioState *m) {
    s32 val04 = common_ground_knockback_action(m, MARIO_ANIM_LAND_ON_STOMACH, 0x15, TRUE, m->actionArg);
    if (val04 == 0x17 && m->health < 0x100) {
        set_mario_action(m, ACT_DEATH_ON_STOMACH, 0);
    }

    return FALSE;
}

s32 act_backward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_BACKWARD_KB, 0x16, TRUE, m->actionArg);
    return FALSE;
}

s32 act_forward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_FORWARD_KB, 0x14, TRUE, m->actionArg);
    return FALSE;
}

s32 act_soft_backward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_SOFT_BACK_KB, 0x64, FALSE, m->actionArg);
    return FALSE;
}

s32 act_soft_forward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_SOFT_FRONT_KB, 0x64, FALSE, m->actionArg);
    return FALSE;
}

s32 act_ground_bonk(struct MarioState *m) {
    if (common_ground_knockback_action(m, MARIO_ANIM_GROUND_BONK, 0x20, TRUE, m->actionArg) == 0x20) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }
    return FALSE;
}

s32 act_death_exit_land(struct MarioState *m) {
    s32 val04;

    //  apply_landing_accel(m, 0.9f);
    play_mario_heavy_landing_sound_once(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);

    val04 = set_mario_animation(m, MARIO_ANIM_FALL_OVER_BACKWARDS);

    if (val04 == 0x36) {
        play_sound(SOUND_MARIO_MAMA_MIA, m->marioObj->header.gfx.cameraToObject);
    } else if (val04 == 0x44) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
    }

    return FALSE;
}

u32 common_landing_action(struct MarioState *m, s16 animation, u32 airAction) {
    u32 stepResult;

    if (m->input & INPUT_NONZERO_ANALOG) {
        apply_landing_accel(m, 0.98f);
    } else if (m->forwardVel >= 16.0f) {
        apply_slope_decel(m, 2.0f);
    } else {
        m->vel[1] = 0.0f;
    }

    stepResult = perform_ground_step(m);
    switch (stepResult) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, airAction, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            set_mario_animation(m, MARIO_ANIM_PUSHING);
            break;
        case GROUND_STEP_ON_RAIL: // kaze
            set_mario_action(m, ACT_GRINDING, 0);
            set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
            break;
    }

    if (m->forwardVel > 16.0f) {
        m->particleFlags |= PARTICLE_DUST;
    }

    set_mario_animation(m, animation);
    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);

    if (m->floor->type & (SPECFLAG_SINKSAND << 8)) {
        m->quicksandDepth += (4 - m->actionTimer) * 3.5f - 0.5f;
    }

    return stepResult;
}
extern u32 landTimeLeft;
s32 common_landing_cancels(struct MarioState *m, struct LandingAction *landingAction,
                           s32 (*setAPressAction)(struct MarioState *, u32, u32)) {
    //! Everything here, including floor steepness, is checked before checking
    // if Mario is actually on the floor. This leads to e.g. remote sliding.

    if (m->floor->normal.y < 0.2923717f) {
        return mario_push_off_steep_floor(m, landingAction->verySteepAction, 0);
    }

    m->doubleJumpTimer = landingAction->unk02;

    if (should_begin_sliding(m)) {
        return set_mario_action(m, landingAction->slideAction, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, landingAction->endAction, 0);
    }
    landTimeLeft = landingAction->numFrames - m->actionTimer;
    if (++m->actionTimer >= landingAction->numFrames) {
        if (!mario_can_get_up(m)) { // CuckyDev: Force Mario into crouching state when under ceiling
            if (landingAction->endAction == ACT_LONG_JUMP_LAND_STOP)
                return set_mario_action(m, ACT_CROUCHING, 0);
            else
                return set_mario_action(m, ACT_START_CROUCHING, 0);
        } else {
            return set_mario_action(m, landingAction->endAction, 0);
        }
    }

    if (m->input & (INPUT_A_PRESSED | INPUT_A_LAST_FRAME)) {
        return setAPressAction(m, landingAction->aPressedAction, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, landingAction->offFloorAction, 0);
    }

    return FALSE;
}

s32 act_jump_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_LAND_FROM_SINGLE_JUMP, ACT_FREEFALL);
    return FALSE;
}

s32 act_freefall_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sFreefallLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_GENERAL_LAND, ACT_FREEFALL);
    return FALSE;
}

s32 act_side_flip_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sSideFlipLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (common_landing_action(m, MARIO_ANIM_SLIDEFLIP_LAND, ACT_FREEFALL) != GROUND_STEP_HIT_WALL) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
    }
    return FALSE;
}

s32 act_hold_jump_land(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_JUMP_LAND_STOP, 0);
    }

    if (common_landing_cancels(m, &sHoldJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ, ACT_HOLD_FREEFALL);
    return FALSE;
}

s32 act_hold_freefall_land(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_FREEFALL_LAND_STOP, 0);
    }

    if (common_landing_cancels(m, &sHoldFreefallLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ, ACT_HOLD_FREEFALL);
    return FALSE;
}

s32 act_long_jump_land(struct MarioState *m) {
    if (!(m->input & INPUT_Z_DOWN)) {
        m->input &= ~(INPUT_A_PRESSED | INPUT_A_LAST_FRAME);
    }

    if (common_landing_cancels(m, &sLongJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        play_sound_if_no_flag(m, SOUND_MARIO_UH2_2, MARIO_MARIO_SOUND_PLAYED);
    }

    common_landing_action(m,
                          !m->marioObj->oMarioLongJumpIsSlow ? MARIO_ANIM_CROUCH_FROM_FAST_LONGJUMP
                                                             : MARIO_ANIM_CROUCH_FROM_SLOW_LONGJUMP,
                          ACT_FREEFALL);
    return FALSE;
}

s32 act_double_jump_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sDoubleJumpLandAction, set_triple_jump_action)) {
        return TRUE;
    }
    common_landing_action(m, MARIO_ANIM_LAND_FROM_DOUBLE_JUMP, ACT_FREEFALL);
    return FALSE;
}

s32 act_triple_jump_land(struct MarioState *m) {
    m->input &= ~(INPUT_A_PRESSED | INPUT_A_LAST_FRAME);

    if (common_landing_cancels(m, &sTripleJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        play_sound_if_no_flag(m, SOUND_MARIO_HAHA, MARIO_MARIO_SOUND_PLAYED);
    }

    common_landing_action(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_FREEFALL);
    return FALSE;
}

s32 act_backflip_land(struct MarioState *m) {
    if (!(m->input & INPUT_Z_DOWN)) {
        m->input &= ~(INPUT_A_PRESSED | INPUT_A_LAST_FRAME);
    }

    if (common_landing_cancels(m, &sBackflipLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        play_sound_if_no_flag(m, SOUND_MARIO_HAHA, MARIO_MARIO_SOUND_PLAYED);
    }

    common_landing_action(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_FREEFALL);
    return FALSE;
}

s32 quicksand_jump_land_action(struct MarioState *m, s32 animation1, s32 animation2, u32 endAction,
                               u32 airAction) {
    if (m->actionTimer++ < 6) {
        m->quicksandDepth -= (7 - m->actionTimer) * 0.8f;
        if (m->quicksandDepth < 1.0f) {
            m->quicksandDepth = 1.1f;
        }

        play_mario_jump_sound(m);
        set_mario_animation(m, animation1);
    } else {
        if (m->actionTimer >= 13) {
            return set_mario_action(m, endAction, 0);
        }

        set_mario_animation(m, animation2);
    }

    apply_landing_accel(m, 0.95f);
    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, airAction, 0);
    }

    return FALSE;
}

s32 act_quicksand_jump_land(struct MarioState *m) {
    s32 cancel = quicksand_jump_land_action(m, MARIO_ANIM_SINGLE_JUMP, MARIO_ANIM_LAND_FROM_SINGLE_JUMP,
                                            ACT_JUMP_LAND_STOP, ACT_FREEFALL);
    return cancel;
}

s32 act_hold_quicksand_jump_land(struct MarioState *m) {
    s32 cancel = quicksand_jump_land_action(m, MARIO_ANIM_JUMP_WITH_LIGHT_OBJ,
                                            MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ,
                                            ACT_HOLD_JUMP_LAND_STOP, ACT_HOLD_FREEFALL);
    return cancel;
}

s32 check_common_moving_cancels(struct MarioState *m) {
    if (m->pos[1] < m->waterLevel - 100) {
        return set_water_plunge_action(m);
    }

    if (m->input & INPUT_SQUISHED) {
        return drop_and_set_mario_action(m, ACT_SQUISHED, 0);
    }

    if (!(m->action & ACT_FLAG_INVULNERABLE)) {
        if (m->health < 0x100) {
            return drop_and_set_mario_action(m, ACT_STANDING_DEATH, 0);
        }
    }

    return FALSE;
}

s32 mario_execute_moving_action(struct MarioState *m) {
    s32 cancel;

    if (check_common_moving_cancels(m)) {
        return TRUE;
    }

    if (mario_update_quicksand(m, 0.25f)) {
        return TRUE;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_WALKING:                  cancel = act_walking(m);                  break;
        case ACT_HOLD_WALKING:             cancel = act_hold_walking(m);             break;
        case ACT_HOLD_HEAVY_WALKING:       cancel = act_hold_heavy_walking(m);       break;
        case ACT_TURNING_AROUND:           cancel = act_turning_around(m);           break;
        case ACT_FINISH_TURNING_AROUND:    cancel = act_finish_turning_around(m);    break;
        case ACT_BRAKING:                  cancel = act_braking(m);                  break;
        case ACT_GRINDING:                 cancel = act_grinding(m);                 break;
        case ACT_RIDING_SHELL_GROUND:      cancel = act_riding_shell_ground(m);      break;
        case ACT_RIDE_MINECART:            cancel = act_ride_minecart(m);            break;
        case ACT_CRAWLING:                 cancel = act_crawling(m);                 break;
        case ACT_BURNING_GROUND:           cancel = act_burning_ground(m);           break;
        case ACT_DECELERATING:             cancel = act_decelerating(m);             break;
        case ACT_HOLD_DECELERATING:        cancel = act_hold_decelerating(m);        break;
        case ACT_BUTT_SLIDE:               cancel = act_butt_slide(m);               break;
        case ACT_STOMACH_SLIDE:            cancel = act_stomach_slide(m);            break;
        case ACT_HOLD_BUTT_SLIDE:          cancel = act_hold_butt_slide(m);          break;
        case ACT_HOLD_STOMACH_SLIDE:       cancel = act_hold_stomach_slide(m);       break;
        case ACT_DIVE_SLIDE:               cancel = act_dive_slide(m);               break;
        case ACT_MOVE_PUNCHING:            cancel = act_move_punching(m);            break;
        case ACT_CROUCH_SLIDE:             cancel = act_crouch_slide(m);             break;
        case ACT_SLIDE_KICK_SLIDE:         cancel = act_slide_kick_slide(m);         break;
        case ACT_HARD_BACKWARD_GROUND_KB:  cancel = act_hard_backward_ground_kb(m);  break;
        case ACT_HARD_FORWARD_GROUND_KB:   cancel = act_hard_forward_ground_kb(m);   break;
        case ACT_BACKWARD_GROUND_KB:       cancel = act_backward_ground_kb(m);       break;
        case ACT_FORWARD_GROUND_KB:        cancel = act_forward_ground_kb(m);        break;
        case ACT_SOFT_BACKWARD_GROUND_KB:  cancel = act_soft_backward_ground_kb(m);  break;
        case ACT_SOFT_FORWARD_GROUND_KB:   cancel = act_soft_forward_ground_kb(m);   break;
        case ACT_GROUND_BONK:              cancel = act_ground_bonk(m);              break;
        case ACT_DEATH_EXIT_LAND:          cancel = act_death_exit_land(m);          break;
        case ACT_JUMP_LAND:                cancel = act_jump_land(m);                break;
        case ACT_FREEFALL_LAND:            cancel = act_freefall_land(m);            break;
        case ACT_DOUBLE_JUMP_LAND:         cancel = act_double_jump_land(m);         break;
        case ACT_SIDE_FLIP_LAND:           cancel = act_side_flip_land(m);           break;
        case ACT_HOLD_JUMP_LAND:           cancel = act_hold_jump_land(m);           break;
        case ACT_HOLD_FREEFALL_LAND:       cancel = act_hold_freefall_land(m);       break;
        case ACT_TRIPLE_JUMP_LAND:         cancel = act_triple_jump_land(m);         break;
        case ACT_BACKFLIP_LAND:            cancel = act_backflip_land(m);            break;
        case ACT_QUICKSAND_JUMP_LAND:      cancel = act_quicksand_jump_land(m);      break;
        case ACT_HOLD_QUICKSAND_JUMP_LAND: cancel = act_hold_quicksand_jump_land(m); break;
        case ACT_LONG_JUMP_LAND:           cancel = act_long_jump_land(m);           break;
    }
    /* clang-format on */

    if (!cancel && (m->input & INPUT_IN_WATER)) {
        m->particleFlags |= PARTICLE_WAVE_TRAIL;
        m->particleFlags &= ~PARTICLE_DUST;
    }

    return cancel;
}
