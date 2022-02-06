#include "sm64.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/print.h"
#include "engine/math_util.h"
#include "game/segment2.h"
#include "game/save_file.h"
#include "puppycam.h"
#include "include/text_strings.h.in"
#include "gfx_dimensions.h"

u8 waterflag;
int timerbeforerotation = -100;
int timerbeforeminmovement = -100;
u8 surfhit = 0;

#define ACCEL 5

s16 newcam_yaw; // Z axis rotation
f32 newcam_yaw_acc;
#define DEFAULTTILT 0xc00
s16 newcam_pitch = DEFAULTTILT; // Y axis rotation
f32 newcam_pitch_acc;
u16 newcam_distance = 750;        // The distance the camera stays from the player
u16 newcam_distance_target = 750; // The distance the player camera tries to reach.
f32 newcam_pos_target[3];   // The position the camera is basing calculations off. *usually* Mario.
f32 newcam_pos[3];          // Position the camera is in the world
f32 newcam_pos_physical[3]; // Position the camera is in the world
f32 newcam_lookat[3];       // Position the camera is looking at
#define newcam_extheight 125
u8 newcam_centering = 0; // The flag that depicts whether the camera's going to try centreing.
s16 newcam_yaw_target;   // The yaw value the camera tries to set itself to when the centre flag is
                         // active. Is set to Mario's face angle.
f32 newcam_pan[3];
s16 newcam_coldist;

#define newcam_sensitivityX 75
#define newcam_sensitivityY 25
s16 newcam_invertX = 0; // Reverses movement of the camera axis.
s16 newcam_invertY = 1;
s16 newcam_auto = 1;
#define newcam_degrade 50

#include "puppycam_angles.inc.c"

/// This is called at every level initialisation.
#define MINSTEP 0xb00
#define INCREMENT 0x40
static void newcam_init(struct Camera *c, u8 dv) {
    float x, y, z;
    if (horizon()) {
        newcam_pitch = DEFAULTTILT + MINSTEP;
    } else if (superHorizon()) {
        newcam_pitch = DEFAULTTILT + MINSTEP * 3;
    } else {
        newcam_pitch = DEFAULTTILT;
    }
    adjustTimers(0);
    newcam_distance_target = 1500;
    if (gCurrLevelNum == LEVEL_ENDING) {
        newcam_yaw = c->yaw; // Mario and the camera's yaw have this offset between them.
    }

    newcam_distance = newcam_distance_target;
}

// store rotation initial and button press once buttonpressed happens
// if let go, rotate to rotationinitial+0x2000
// while pressed, do the normal ACCELeration
int heldframes = 0;
u16 storedrotation;
u16 storedbuttons;
#define SLUGGISHNESS 7
static void newcam_rotate_button(void) {
    s32 CBUTTONS = gPlayer1Controller->buttonDown;
    if (newcam_hitboxes()) {
        CBUTTONS &= (L_CBUTTONS | R_CBUTTONS);
    }
    if (!newcam_auto) {
        adjustTimers(-10);
    }
    if ((CBUTTONS & (L_CBUTTONS | R_CBUTTONS)) & ~storedbuttons) {
        storedrotation = newcam_yaw_target;
        storedbuttons = CBUTTONS;
    }

    if (CBUTTONS & R_CBUTTONS) {
        newcam_yaw_acc = approach_s16_symmetric(newcam_yaw_acc, -100, ACCEL);
        heldframes++;
        adjustTimers(-10);
    } else if (CBUTTONS & L_CBUTTONS) {
        newcam_yaw_acc = approach_s16_symmetric(newcam_yaw_acc, 100, ACCEL);
        heldframes++;
        adjustTimers(-10);
    } else {
        if (heldframes > 0 && heldframes < SLUGGISHNESS) {
            play_sound(SOUND_GENERAL_YOSHI_CAM, gDefaultSoundArgs);
            newcam_yaw_acc = 0;
            newcam_centering = 1;
            adjustTimers(-40);
            if (storedbuttons & R_CBUTTONS) {
                newcam_yaw_target = storedrotation + (ivrt(newcam_invertX) * 0x2000);
            } else {
                newcam_yaw_target = storedrotation - (ivrt(newcam_invertX) * 0x2000);
            }
        } else {
            newcam_yaw_acc -= (newcam_yaw_acc * ((f32) newcam_degrade / 100));
            newcam_yaw_acc = camera_approach_f32_symmetric(newcam_yaw_acc, 0, 10.f);
        }
        storedbuttons = 0;
        heldframes = 0;
    }
    if (CBUTTONS & U_CBUTTONS) {
        newcam_pitch_acc = approach_s16_symmetric(newcam_pitch_acc, -100, ACCEL);
        adjustTimers(0);
    } else if (CBUTTONS & D_CBUTTONS) {
        newcam_pitch_acc = approach_s16_symmetric(newcam_pitch_acc, 100, ACCEL);
        adjustTimers(0);
    } else {
        newcam_pitch_acc -= (newcam_pitch_acc * ((f32) newcam_degrade / 100));
        newcam_pitch_acc = camera_approach_f32_symmetric(newcam_pitch_acc, 0, 10.f);
    }

    if (CBUTTONS & (U_CBUTTONS | D_CBUTTONS)) {
        if (newcam_pitch > (DEFAULTTILT + (MINSTEP * 3))) {
            newcam_pitch = (DEFAULTTILT + (MINSTEP * 3));
            newcam_pitch_acc = 0;
            adjustTimers(-10);
        }
        if (waterflag == 3) {
            if (newcam_pitch < (DEFAULTTILT - (MINSTEP * 3))) {
                newcam_pitch = (DEFAULTTILT - (MINSTEP * 3));
                newcam_pitch_acc = 0;
                adjustTimers(-10);
            }
        } else {
            if (newcam_pitch < (DEFAULTTILT - MINSTEP)) {
                newcam_pitch = (DEFAULTTILT - MINSTEP);
                newcam_pitch_acc = 0;
                adjustTimers(-10);
            }
        }
    }

    newcam_distance = approach_s16_symmetric(newcam_distance, newcam_distance_target, 50);

    if (gPlayer1Controller->buttonPressed & R_TRIG) {
        play_sound(SOUND_MENU_CLICK_CHANGE_VIEW, gDefaultSoundArgs);
    }
    if (gPlayer1Controller->buttonDown & R_TRIG) {
        if ((gMarioState->action == ACT_CLIMBING_POLE) || (gMarioState->action == ACT_HOLDING_POLE)
            || (gMarioState->action == ACT_GRAB_POLE_FAST)
            || (gMarioState->action == ACT_GRAB_POLE_SLOW)) {
            newcam_yaw_target = gMarioState->faceAngle[1];
        } else {
            newcam_yaw_target = 0x8000 + gMarioState->faceAngle[1];
        }
        if (gMarioState->action & (ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_SWIMMING)) {
            newcam_pitch = 0x800;
        }
        newcam_centering = 1;
        adjustTimers(-10);
    }
    if (newcam_centering) {
        newcam_yaw = approach_s16_symmetric(newcam_yaw, newcam_yaw_target, 0x800);
        if (abs_angle_diff(newcam_yaw, newcam_yaw_target) < 0x0010)
            newcam_centering = 0;
    } else {
        newcam_yaw_target = newcam_yaw;
    }
}
void goDefaultPitch() {
    if (timerbeforerotation > 0) {
        newcam_pitch = approach_s16_symmetric(newcam_pitch, DEFAULTTILT, 0x40);
    }
}

s32 tiltbackup = 0;
static void newcam_update_values(void) { // For tilt, this just limits it so it doesn't go further than
                                         // 90 degrees either way. 90 degrees is actually 16384, but can
                                         // sometimes lead to issues, so I just leave it shy of 90.
    int timerbackup = timerbeforerotation;
    u8 prevWaterFlag = waterflag;
    waterflag = 0;
    newcam_yaw -= (newcam_yaw_acc * (newcam_sensitivityX / 10)) * ivrt(newcam_invertX);
    newcam_pitch -= (newcam_pitch_acc * (newcam_sensitivityY / 10)) * ivrt(newcam_invertY);
    switch (gMarioState->action) {
        case ACT_RIDE_MINECART:
        case ACT_SHOT_FROM_CANNON:
        case ACT_FLYING:
        case ACT_GRINDING:
            if (gMarioState->forwardVel > 30.f) {
                waterflag = 1;
            }
            break;
        case ACT_LEDGE_GRAB:
        case ACT_LEDGE_CLIMB_FAST:
        case ACT_LEDGE_CLIMB_SLOW_1:
        case ACT_LEDGE_CLIMB_SLOW_2:
        case ACT_IDLE:
        case ACT_PANTING:
        case ACT_TOP_OF_POLE:
            waterflag = 2;
            break;
        case ACT_GRAB_POLE_SLOW:
        case ACT_HOLDING_POLE:
        case ACT_GRAB_POLE_FAST:
            waterflag = 4;
            break;
        case ACT_HANG_VINE:
            waterflag = 7;
            break;
        case ACT_HANGING:
        case ACT_HANG_MOVING:
        case ACT_START_HANGING:
            break;
    }

    if (gMarioState->action & ACT_FLAG_SWIMMING) {
        if (prevWaterFlag == 0) {
            tiltbackup = newcam_pitch;
        }
        waterflag = 3;
        newcam_distance_target = 850;
    } else if (gMarioState->action & ACT_FLAG_SHORT_HITBOX) {
        if (!mario_can_get_up(gMarioState)) {
            if (prevWaterFlag == 0) {
                tiltbackup = newcam_pitch;
            }
            waterflag = 5;
            newcam_distance_target = 200;
        }
    } else {
        newcam_distance_target = 1500;
    }

    if (gMarioState->action & ACT_FLAG_HANGING) {
        if (prevWaterFlag == 0) {
            tiltbackup = newcam_pitch;
            newcam_pitch = 0xFE00;
        }
        waterflag = 6;
        newcam_distance_target = 1000;
        gMarioState->prevAction |= ACT_FLAG_HANGING;
    }

    if (timerbeforerotation < 0) {
        timerbeforerotation++;
    } else {
        timerbeforerotation = 0;
    }
    if ((prevWaterFlag != waterflag)
        && ((prevWaterFlag == 3) || (prevWaterFlag == 5) || (prevWaterFlag == 6))) {
        newcam_pitch = tiltbackup;
    }
    if (waterflag) {
        timerbeforerotation = timerbackup + 1;
        if (timerbeforerotation > 30) {
            switch (waterflag) {
                case 0:
                    goDefaultPitch();
                    break;
                case 1:
                    newcam_yaw = approach_s16_symmetric(newcam_yaw, gMarioState->faceAngle[1] + 0x8000,
                                                        (gMarioState->forwardVel * 0x6));
                    goDefaultPitch();
                    break;
                case 2:
                    newcam_yaw =
                        approach_s16_symmetric(newcam_yaw, gMarioState->faceAngle[1] + 0x8000, (0x100));
                    goDefaultPitch();
                    break;
                case 3:
#define SWIMVEL                                                                                        \
    sqrtf(gMarioState->forwardVel * gMarioState->forwardVel + gMarioState->vel[1] * gMarioState->vel[1])
                    if (gMarioState->pos[1] >= gMarioState->waterLevel - 130) {
                        newcam_pitch = (approach_s16_symmetric(newcam_pitch, tiltbackup, 0x100));
                    } else {
                        if (SWIMVEL > 4.f) {
                            newcam_pitch = approach_s16_symmetric(
                                newcam_pitch, (-gMarioState->faceAngle[0]) + 0x800, (SWIMVEL * 0x08));
                            newcam_yaw = approach_s16_symmetric(
                                newcam_yaw, gMarioState->faceAngle[1] + 0x8000, (SWIMVEL * 0x08));
                        } else {
                            newcam_pitch = (approach_s16_symmetric(newcam_pitch, 0x800, 0x20));
                            newcam_yaw = approach_s16_symmetric(
                                newcam_yaw, gMarioState->faceAngle[1] + 0x8000, 0x20);
                        }
                    }

                    break;
                case 4:
                    newcam_yaw =
                        approach_s16_symmetric(newcam_yaw, gMarioState->faceAngle[1] + 0x8000, (0x100));
                    goDefaultPitch();
                    break;
                case 5:
                    newcam_yaw =
                        approach_s16_symmetric(newcam_yaw, gMarioState->faceAngle[1] + 0x8000, (0x100));
                    goDefaultPitch();
                    break;
                case 6:
                    newcam_yaw = approach_s16_symmetric(newcam_yaw, gMarioState->faceAngle[1] + 0x8000,
                                                        (gMarioState->forwardVel * 0x6));
                    goDefaultPitch();
                    break;
                case 7:
                    if (gMarioState->usedObj->oBehParams & 0x01) {
                        if (timerbeforerotation > 0) {
                            newcam_yaw = approach_s16_symmetric(
                                newcam_yaw, gMarioState->usedObj->oFaceAngleYaw - 0x2000, 0x200);
                        }
                        newcam_distance_target = 1800;
                    } else {
                        if (timerbeforerotation > 0) {
                            newcam_yaw = approach_s16_symmetric(
                                newcam_yaw, gMarioState->usedObj->oFaceAngleYaw + 0x4000, 0x100);
                            newcam_distance_target = 2500;
                        }
                    }
                    break;
            }
        }
    } else {
        if (newcam_pitch < (DEFAULTTILT - MINSTEP)) {
            newcam_pitch = (DEFAULTTILT - MINSTEP);
        }
    }
}
u8 rendermariofromCam = 1;
#define COLSKIPDIST 750.f // how far the cam can move without clipping if there is mid level geometry
static void newcam_collision(void) {
    struct Surface *surf = 0;
    Vec3f camdir;
    Vec3f hitpos;
    struct Surface *surfback = 0;
    Vec3f camdirback;
    Vec3f hitposback;
    Vec3f calcPos;
    Vec3f noColRadius;
    u8 renderalways = 0;

    camdir[0] = newcam_pos_physical[0] - newcam_pos_target[0];
    camdir[1] = newcam_pos_physical[1] - newcam_pos_target[1];
    camdir[2] = newcam_pos_physical[2] - newcam_pos_target[2];
    vec3f_copy(calcPos, newcam_pos_target);

    camdirback[0] = -camdir[0];
    camdirback[1] = -camdir[1];
    camdirback[2] = -camdir[2];
    vec3f_normalize(camdirback);
    vec3f_mult(camdirback, COLSKIPDIST);

    noColRadius[0] = newcam_pos_target[0] - camdirback[0];
    noColRadius[1] = newcam_pos_target[1] - camdirback[1];
    noColRadius[2] = newcam_pos_target[2] - camdirback[2];

    find_surface_on_ray(noColRadius, camdirback, &surfback, &hitposback, 7);
    if (surfback) {
        if ((surfback->type & 0x000F0000) != (CAMERA_TERRAIN_NOSURFBACK << 16)) {
            vec3f_copy(calcPos, hitposback);
            renderalways = 1;
        }
    }
    find_surface_on_ray(calcPos, camdir, &surf, &hitpos, 7);
    newcam_coldist = sqrtf((newcam_pos_target[0] - hitpos[0]) * (newcam_pos_target[0] - hitpos[0])
                           + (newcam_pos_target[1] - hitpos[1]) * (newcam_pos_target[1] - hitpos[1])
                           + (newcam_pos_target[2] - hitpos[2]) * (newcam_pos_target[2] - hitpos[2]));
    if (surf && ((surf->type & 0x000F0000) != (CAMERA_TERRAIN_GOTHROUGH << 16))
        && (!surf->object || (surf->object->behavior != segmented_to_virtual(bhvWarpPipe)))) {
#define maxdist 1750
        if (newcam_coldist < maxdist) {
            newcam_pos[0] = hitpos[0];
            newcam_pos[1] = hitpos[1];
            newcam_pos[2] = hitpos[2];
            surfhit = 1;
#define mindist 150.f
            if (newcam_coldist > mindist) {
                vec3f_normalize(camdirback);
                vec3f_mult(camdirback, min(newcam_coldist - mindist, 40.f));
                vec3f_add(newcam_pos, camdirback);
            } else {
                rendermariofromCam = 0 + renderalways;
                newcam_pan[0] = 0;
                newcam_pan[1] = 0;
                newcam_pan[2] = 0;
            }
        }
    } else {
        newcam_coldist = newcam_distance;
    }
}
void vec3f_approach_smoothly(f32 *src, f32 *goal, f32 asymptotic, f32 linear) {
    f32 diff;
    f32 vector[3];
    vec3f_copy(vector, goal);
    vec3f_sub(vector, src);
    diff = vec3f_length(vector);
    vec3f_mult(vector, asymptotic);
    vec3f_add(src, vector);
    if (diff > .1f) {
        vec3f_normalize(vector);
        vec3f_mult(vector, linear);
        vec3f_add(src, vector);
    }
}

#define PANSIZE 220 // How much the camera sticks out a bit in the direction you're looking.
#define ASYMPTOTIC 0.05f
#define LINEAR 1.f
static void newcam_set_pan(void) {
    // Apply panning values based on Mario's direction.
    f32 goalPos[3];
    goalPos[0] = lengthdir_x(PANSIZE * min((newcam_distance / newcam_distance_target), 1.f),
                             gMarioState->faceAngle[1]);
    goalPos[1] = 0;
    goalPos[2] = lengthdir_y(PANSIZE * min((newcam_distance / newcam_distance_target), 1.f),
                             gMarioState->faceAngle[1]);
    if (!(gMarioState->action != ACT_HOLDING_BOWSER && gMarioState->action != ACT_SLEEPING
          && gMarioState->action != ACT_START_SLEEPING)) {
        goalPos[0] = 0;
        goalPos[1] = 0;
        goalPos[2] = 0;
    }
    vec3f_approach_smoothly(newcam_pan, goalPos, ASYMPTOTIC, LINEAR);
}
Vec3f wkStart;
struct Surface *wkHit = 0;
s16 newcam_floorpitch = 0;
#define autorotateForce 2.0f
static void newcam_position_cam(void) {
    f32 floorY = 0;
    f32 floorY2 = 0;
    s16 shakeX;
    s16 shakeY;
    struct Object *foc;
    Vec3f dir;
    Vec3f hit_pos;
    struct Surface *WKSURF = 0;
    register u16 diff1, diff2;

    timerbeforeminmovement++;
#define WKDIST 2500.f
#define WKROTSPEED 0x200
#define WKROTTIME 10
    if (gMarioState->action == ACT_WALL_KICK_AIR) {
        if (gMarioState->actionTimer < WKROTTIME) {
            if (wkHit) {
                dir[0] = wkHit->normal.x * WKDIST;
                dir[1] = 0;
                dir[2] = wkHit->normal.z * WKDIST;

                find_surface_on_ray(wkStart, dir, &WKSURF, hit_pos, 2);
                if (WKSURF) {
                    if (abs_angle_diff(atan2s(WKSURF->normal.z, WKSURF->normal.x),
                                       atan2s(wkHit->normal.z, wkHit->normal.x) - 0x8000)
                        < 0x800) {
                        diff1 = abs_angle_diff(atan2s(wkHit->normal.z, wkHit->normal.x) + 0x4000,
                                               newcam_yaw);
                        diff2 = abs_angle_diff(atan2s(wkHit->normal.z, wkHit->normal.x) - 0x4000,
                                               newcam_yaw);
                        if (min(diff1, diff2) < 0x2000) {
                            if (diff1 < diff2) {
                                newcam_yaw = approach_s16_symmetric(
                                    newcam_yaw, atan2s(wkHit->normal.z, wkHit->normal.x) + 0x4000,
                                    WKROTSPEED);
                            } else {
                                newcam_yaw = approach_s16_symmetric(
                                    newcam_yaw, atan2s(wkHit->normal.z, wkHit->normal.x) - 0x4000,
                                    WKROTSPEED);
                            }
                        }
                    }
                }
            }
        }
    } else if (gMarioState->action != ACT_WALL_SLIDE) {
        wkHit = 0;
        if (timerbeforeminmovement > 40) {
            if (!surfhit) { //! surfhit
                newcam_yaw = atan2s(-gMarioState->pos[2]
                                        - (gMarioState->pos[2] - newcam_pos_target[2]) * autorotateForce
                                        + newcam_pos[2],
                                    -gMarioState->pos[0]
                                        - (gMarioState->pos[0] - newcam_pos_target[0]) * autorotateForce
                                        + newcam_pos[0]);
            } else {
                newcam_yaw = approach_s16_symmetric(newcam_yaw, gMarioState->faceAngle[1] + 0x8000,
                                                    0x10 + gMarioState->forwardVel * 16);
                surfhit = 0;
            }
        }
    }
    if (!(gMarioState->action & ACT_FLAG_SWIMMING)) {
        calc_y_to_curr_floor(&floorY, 1.f, 200.f, &floorY2, 0.9f, 200.f); // fucked up for solid objects
    }
    newcam_update_values();
    shakeX = gLakituState.shakeMagnitude[1];
    shakeY = gLakituState.shakeMagnitude[0];
    newcam_pos_target[0] = gMarioState->pos[0];
    newcam_pos_target[1] =
        gMarioState->pos[1] + newcam_extheight - 100.f * (waterflag == 5) - 100.f * (waterflag == 6);
    newcam_pos_target[2] = gMarioState->pos[2];
    if (foc = focus()) {
        if (timerbeforeminmovement > 40) {
            newcam_yaw = approach_s16_symmetric(
                newcam_yaw,
                atan2s(-foc->oPosZ + newcam_pos_physical[2], -foc->oPosX + newcam_pos_physical[0]),
                0x400);
        }
    }
    newcam_floorpitch = approach_s16_symmetric(newcam_floorpitch, look_down_slopes(newcam_yaw) - 0x5b0,
                                               0x80 * absf(gMarioState->forwardVel) / 32.f + 0x20);
    newcam_pos[0] =
        newcam_pos_target[0]
        + lengthdir_x(lengthdir_y(newcam_distance, newcam_pitch + shakeX + newcam_floorpitch),
                      newcam_yaw + shakeY);
    newcam_pos[2] =
        newcam_pos_target[2]
        + lengthdir_y(lengthdir_y(newcam_distance, newcam_pitch + shakeX + newcam_floorpitch),
                      newcam_yaw + shakeY);
    newcam_pos[1] = newcam_pos_target[1]
                    + lengthdir_x(newcam_distance,
                                  newcam_pitch + gLakituState.shakeMagnitude[0] + newcam_floorpitch)
                    + floorY;

    newcam_pos_physical[0] =
        newcam_pos_target[0]
        + lengthdir_x(lengthdir_y(newcam_distance, newcam_pitch + shakeX + newcam_floorpitch),
                      newcam_yaw + shakeY);
    newcam_pos_physical[2] =
        newcam_pos_target[2]
        + lengthdir_y(lengthdir_y(newcam_distance, newcam_pitch + shakeX + newcam_floorpitch),
                      newcam_yaw + shakeY);
    newcam_pos_physical[1] =
        newcam_pos_target[1]
        + lengthdir_x(newcam_distance,
                      newcam_pitch + gLakituState.shakeMagnitude[0] + newcam_floorpitch)
        + floorY;
    newcam_set_pan();
    // Set where the camera wants to be looking at. This is almost always the place it's based off, too.
    newcam_lookat[0] = newcam_pos_target[0] + newcam_pan[0];
    newcam_lookat[1] = newcam_pos_target[1] + floorY2 + newcam_pan[1];
    newcam_lookat[2] = newcam_pos_target[2] + newcam_pan[2];

    newcam_collision();
}

static void newcam_apply_values(struct Camera *c) {
    register s32 i;
    for (i = 0; i < 3; i++) {
        c->pos[i] = newcam_pos[i];
        gLakituState.pos[i] = newcam_pos[i];
        c->focus[i] = newcam_lookat[i];
        gLakituState.focus[i] = newcam_lookat[i];
    }
    rollBBHCam(c);
    c->yaw = newcam_yaw;
    gLakituState.yaw = newcam_yaw;
}
extern s16 fadeoutstarted;
extern s16 backupYaw;
// Main loop.
static void newcam_loop(struct Camera *c) {
    newcam_rotate_button();
    if (fadeoutstarted > 18) {
        newcam_centering = 1;
        newcam_yaw = backupYaw;
        newcam_yaw_target = backupYaw;
        newcam_pos[0] =
            newcam_pos_target[0]
            + lengthdir_x(lengthdir_y(newcam_distance, newcam_pitch + newcam_floorpitch), newcam_yaw);
        newcam_pos[2] =
            newcam_pos_target[2]
            + lengthdir_y(lengthdir_y(newcam_distance, newcam_pitch + newcam_floorpitch), newcam_yaw);
        newcam_pos[1] = newcam_pos_target[1]
                        + lengthdir_x(newcam_distance, newcam_pitch + gLakituState.shakeMagnitude[0]
                                                           + newcam_floorpitch);

        newcam_pos_physical[0] =
            newcam_pos_target[0]
            + lengthdir_x(lengthdir_y(newcam_distance, newcam_pitch + newcam_floorpitch), newcam_yaw);
        newcam_pos_physical[2] =
            newcam_pos_target[2]
            + lengthdir_y(lengthdir_y(newcam_distance, newcam_pitch + newcam_floorpitch), newcam_yaw);
        newcam_pos_physical[1] =
            newcam_pos_target[1]
            + lengthdir_x(newcam_distance,
                          newcam_pitch + gLakituState.shakeMagnitude[0] + newcam_floorpitch);
    }
    newcam_position_cam();
    newcam_find_fixed();
    if (gMarioObject)
        newcam_apply_values(c);
}

void move_into_c_up(struct Camera *c);
s32 update_c_up(Vec3f, Vec3f);
static s32 mode_c_up_camera(struct Camera *c) {
    adjustTimers(-40);
    storedbuttons = 0;
    // Play a sound when entering C-Up mode
    if (!(sCameraSoundFlags & CAM_SOUND_C_UP_PLAYED)) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gDefaultSoundArgs);
        sCameraSoundFlags |= CAM_SOUND_C_UP_PLAYED;
    }

    // Zoom in first
    if (gCameraMovementFlags & CAM_MOVING_INTO_MODE) {
        gCameraMovementFlags |= CAM_MOVE_C_UP_MODE;
        move_into_c_up(c);
        return 1;
    }

    if (!(gCameraMovementFlags & CAM_MOVE_STARTED_EXITING_C_UP)) {
        // Normal update
        sStatusFlags &= ~(CAM_FLAG_START_TRANSITION | CAM_FLAG_TRANSITION_OUT_OF_C_UP);
        sModeTransition.framesLeft = 0;
        move_mario_head_c_up(c);
        update_c_up(c->focus, c->pos);
    } else {
        // Exiting C-Up
        if (sStatusFlags & CAM_FLAG_TRANSITION_OUT_OF_C_UP) {
            // Retrieve the previous position and focus
            vec3f_copy(c->pos, sCameraStoreCUp.pos);
            vec3f_add(c->pos, gMarioState->pos);
            vec3f_copy(c->focus, sCameraStoreCUp.focus);
            vec3f_add(c->focus, gMarioState->pos);
            // Make Mario look forward
            gPlayerCameraState.headRotation[0] =
                approach_s16_symmetric(gPlayerCameraState.headRotation[0], 0, 1024);
            gPlayerCameraState.headRotation[1] =
                approach_s16_symmetric(gPlayerCameraState.headRotation[1], 0, 1024);
        } else {
            // Finished exiting C-Up
            gCameraMovementFlags &= ~(CAM_MOVE_STARTED_EXITING_C_UP | CAM_MOVE_C_UP_MODE);
        }
    }
    newcam_yaw = (sModeOffsetYaw + gMarioState->faceAngle[1] + 0x8000);
    newcam_yaw_target = newcam_yaw;
    newcam_centering = 1;
    return 0;
}
