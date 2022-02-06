#include <ultra64.h>

#define INCLUDED_FROM_CAMERA_C

#include "prevent_bss_reordering.h"
#include "sm64.h"
#include "camera.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "mario_misc.h"
#include "game_init.h"
#include "hud.h"
#include "engine/math_util.h"
#include "area.h"
#include "engine/surface_collision.h"
#include "engine/behavior_script.h"
#include "level_update.h"
#include "ingame_menu.h"
#include "mario_actions_cutscene.h"
#include "save_file.h"
#include "object_helpers.h"
#include "print.h"
#include "spawn_sound.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "object_list_processor.h"
#include "paintings.h"
#include "engine/graph_node.h"
#include "level_table.h"
#include "allheader.h"
#include "OPT_FOR_SIZE.h"

#define CBUTTON_MASK (U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS)

// BSS
/**
 * Stores Lakitu's position from the last frame, used for transitioning in next_lakitu_state()
 */
Vec3f sOldPosition;
Vec3f sOldFocus;
struct PlayerCameraState gPlayerCameraState;
/**
 * Lakitu's position and focus.
 * @see LakituState
 */
struct LakituState gLakituState;
struct CameraFOVStatus sFOVState;
struct TransitionInfo sModeTransition;
struct Camera *gCamera;
/**
 * The current spline that controls the camera's position during the credits.
 */
#define SPLINESEGMENTS 32
struct CutsceneSplinePoint sCurCreditsSplinePos[SPLINESEGMENTS];
struct CutsceneSplinePoint sCurCreditsSplineFocus[SPLINESEGMENTS];
f32 sCutsceneSplineSegmentProgress;
s16 sCutsceneSplineSegment;

// Shaky Hand-held Camera effect variables
struct HandheldShakePoint sHandheldShakeSpline[4];
s16 sHandheldShakeMag;
f32 sHandheldShakeTimer;
f32 sHandheldShakeInc;
s16 sHandheldShakePitch;
s16 sHandheldShakeYaw;
s16 sHandheldShakeRoll;
s32 gObjCutsceneDone;

/**
 * Flags that determine what movements the camera should start / do this frame.
 */
s16 gCameraMovementFlags;
s16 sStatusFlags;
s16 sCameraSoundFlags;
s16 sCutsceneDialogID;
s16 sCutsceneShot;
s16 gCutsceneTimer;
/**
 * The angle of the direction vector from the area's center to Mario's position.
 */
s16 sCUpCameraPitch;
s16 sModeOffsetYaw;
f32 sCannonYOffset;
/**
 * These structs are used by the cutscenes. Most of the fields are unused, and some (all?) of the used
 * ones have multiple uses.
 * Check the cutscene_start functions for documentation on the cvars used by a specific cutscene.
 */
struct CutsceneVariable sCutsceneVars[10];
struct ModeTransitionInfo sModeInfo;
struct CameraStoredInfo sCameraStoreCUp;
struct CameraStoredInfo sCameraStoreCutscene;
struct Object *gCutsceneFocus = NULL;

/**
 * How fast the camera's yaw should approach the next yaw.
 */
u8 sObjectCutscene = 0;
u8 gRecentCutscene = 0;
u8 sFramesSinceCutsceneEnded = 0;
/**
 * Mario's response to a dialog.
 * 0 = No response yet
 * 1 = Yes
 * 2 = No
 * 3 = Dialog doesn't have a response
 */
u8 sCutsceneDialogResponse = 0;

__attribute__((always_inline))  inline  s32 update_c_up(Vec3f, Vec3f);
__attribute__((always_inline))  inline  s32 update_in_cannon(Vec3f, Vec3f);

// Move these two tables to another include file?
extern u8 sDanceCutsceneIndexTable[][4];

/**
 * Start shaking the camera's yaw (side to side)
 */
void set_camera_yaw_shake(s32 mag, s32 decay, s32 inc) {
    if (ABS(mag) > ABS(gLakituState.shakeMagnitude[1])) {
        gLakituState.shakeMagnitude[1] = mag;
        gLakituState.shakeYawDecay = decay;
        gLakituState.shakeYawVel = inc;
    }
}
void set_camera_shake_from_hit(s32 shake) {
    switch (shake) {
        // Makes the camera stop for a bit
        case SHAKE_ATTACK:
            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_FALL_DAMAGE:
            set_camera_pitch_shake(0x60, 0x3, 0x8000);
            set_camera_roll_shake(0x60, 0x3, 0x8000);
            break;

        case SHAKE_GROUND_POUND:
            set_camera_pitch_shake(0x60, 0xC, 0x8000);
            break;

        case SHAKE_SMALL_DAMAGE:
            set_camera_yaw_shake(0x80, 0x8, 0x4000);
            set_camera_roll_shake(0x80, 0x8, 0x4000);
            set_fov_shake(0x100, 0x30, 0x8000);

            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_MED_DAMAGE:
            set_camera_yaw_shake(0x100, 0x10, 0x4000);
            set_camera_roll_shake(0x100, 0x10, 0x4000);
            set_fov_shake(0x180, 0x40, 0x8000);

            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_LARGE_DAMAGE:
            set_camera_yaw_shake(0x180, 0x20, 0x4000);
            set_camera_roll_shake(0x200, 0x20, 0x4000);
            set_fov_shake(0x200, 0x50, 0x8000);
            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_HIT_FROM_BELOW:
            gLakituState.focHSpeed = 0.07;
            gLakituState.posHSpeed = 0.07;
            break;

        case SHAKE_SHOCK:
            set_camera_pitch_shake(random_float() * 64.f, 0x8, 0x8000);
            set_camera_yaw_shake(random_float() * 64.f, 0x8, 0x8000);
            break;
    }
}

u16 shakeValues[11][6] = { { 0, 0, 0, 0, 0, 0 },
                           { 0x60, 8, 0x4000, 0, 0, 0 },
                           { 0xC0, 8, 0x4000, 0, 0, 0 },
                           { 0x100, 8, 0x3000, 0, 0, 0 },
                           { 0, 0, 0, 0, 0, 0 },
                           {
                               0x60,
                               8,
                               0x1000,
                               0x20,
                               0x80,
                               0x200,
                           },
                           { 0x80, 0x10, 0x3000, 0, 0, 0 },
                           { 0x20, 8, 0x8000, 0, 0, 0 },
                           { 0x40, 8, 0x8000, 0, 0, 0 },
                           { 0x20, 8, 0x8000, 0x400, 0x10, 0x100 },
                           { 0x40, 2, 0x8000, 0, 0, 0 } };
/**
 * Start a shake from the environment
 */
void set_environmental_camera_shake(s32 shake) {
    set_camera_pitch_shake(shakeValues[shake][0], shakeValues[shake][1], shakeValues[shake][2]);
    if (shakeValues[shake][3]) {
        set_camera_yaw_shake(shakeValues[shake][3], shakeValues[shake][4], shakeValues[shake][5]);
    }
}

/**
 * Starts a camera shake, but scales the amplitude by the point's distance from the camera
 */
void set_camera_shake_from_point(s32 shake, f32 posX, f32 posY, f32 posZ) {
    switch (shake) {
        case SHAKE_POS_BOWLING_BALL:
            set_pitch_shake_from_point(0x28, 0x8, 0x4000, 2000.f, posX, posY, posZ);
            break;

        case SHAKE_POS_SMALL:
            set_pitch_shake_from_point(0x80, 0x8, 0x4000, 4000.f, posX, posY, posZ);
            set_fov_shake_from_point_preset(SHAKE_FOV_SMALL, posX, posY, posZ);
            break;

        case SHAKE_POS_MEDIUM:
            set_pitch_shake_from_point(0xC0, 0x8, 0x4000, 6000.f, posX, posY, posZ);
            set_fov_shake_from_point_preset(SHAKE_FOV_MEDIUM, posX, posY, posZ);
            break;

        case SHAKE_POS_LARGE:
            set_pitch_shake_from_point(0x100, 0x8, 0x3000, 8000.f, posX, posY, posZ);
            set_fov_shake_from_point_preset(SHAKE_FOV_LARGE, posX, posY, posZ);
            break;
    }
}
// if marios last time on a floor was on a solid object, set that to the minimum possible floorheight.
// struct Object *marioFloorObjlastFrame;
struct Object *marioLastObj = 0;
f32 calc_y_to_curr_floor(f32 *posOff, f32 posMul, f32 posBound, f32 *focOff, f32 focMul, f32 focBound) {
    f32 floorHeight = gMarioState->floorHeight;
    if (gMarioState->pos[1] == floorHeight) {
        if (gMarioState->floor) {
            if (marioLastObj = gMarioState->floor->object) {
                gMarioState->lastFloorHeight = floorHeight;
            } else {
                gMarioState->lastFloorHeight = -999999.f;
            }
        }
    } else if (marioLastObj && (marioLastObj->oDistanceToMario < marioLastObj->oCollisionDistance)) {
        marioLastObj = 0;
        gMarioState->lastFloorHeight = -999999.f;
    }
    if (floorHeight < gMarioState->lastFloorHeight) {
        floorHeight = gMarioState->lastFloorHeight;
    }

    if (gMarioState->action & ACT_FLAG_ON_POLE) {
        if (floorHeight >= gMarioStates[0].usedObj->oPosY
            && gMarioState->pos[1]
                   < 0.7f * gMarioStates[0].usedObj->hitboxHeight + gMarioStates[0].usedObj->oPosY) {
            posBound = 1200;
        }
    }

    *posOff = (floorHeight - gMarioState->pos[1]) * posMul;
    if (*posOff > posBound) {
        *posOff = posBound;
    } else if (*posOff < -posBound) {
        *posOff = -posBound;
    }

    *focOff = (floorHeight - gMarioState->pos[1]) * focMul;
    if (*focOff > focBound) {
        *focOff = focBound;
    } else if (*focOff < -focBound) {
        *focOff = -focBound;
    }
}

/**
 * Pitch the camera down when the camera is facing down a slope
 */
__attribute__((always_inline))  inline  s16 look_down_slopes(s32 camYaw) {
    struct Surface *floor;
    f32 floorDY;
    // Default pitch
    s32 pitch = 0x05B0;
    // x and z offsets towards the camera
    f32 xOff = gMarioState->pos[0] + sins(camYaw) * 40.f;
    f32 zOff = gMarioState->pos[2] + coss(camYaw) * 40.f;

    floorDY = find_floor(xOff, gMarioState->pos[1], zOff, &floor) - gMarioState->pos[1];

    if (floor != NULL) {
        if (floorDY > 0) {
            if (!(floor->normal.y >= 0.95f && floorDY < 100.f)) {
                pitch += atan2s(40.f, floorDY);
            }
        }
    }

    return pitch;
}

void move_mario_head_c_up(UNUSED struct Camera *c) {

    sCUpCameraPitch += (s16) (gPlayer1Controller->stickY * 10.f);
    sModeOffsetYaw -= (s16) (gPlayer1Controller->stickX * 10.f);

    if (sCUpCameraPitch > 0x38E3) {
        sCUpCameraPitch = 0x38E3;
    }
    if (sCUpCameraPitch < -0x2000) {
        sCUpCameraPitch = -0x2000;
    }
    if (sModeOffsetYaw > 0x5555) {
        sModeOffsetYaw = 0x5555;
    }
    if (sModeOffsetYaw < -0x5555) {
        sModeOffsetYaw = -0x5555;
    }
    gPlayerCameraState.headRotation[0] = sCUpCameraPitch * 3 / 4;
    gPlayerCameraState.headRotation[1] = sModeOffsetYaw * 3 / 4;
}

// Compiler gets mad if I put this any further above. thanks refresh 7
#include "../../enhancements/puppycam.inc.c"
void focus_on_mario(Vec3f focus, Vec3f pos, f32 posYOff, f32 focYOff, f32 dist, s32 pitch, s32 yaw) {
    Vec3f marioPos;

    marioPos[0] = gMarioState->pos[0];
    marioPos[1] = gMarioState->pos[1] + posYOff;
    marioPos[2] = gMarioState->pos[2];

    vec3f_set_dist_and_angle(marioPos, pos, dist, pitch, yaw);

    focus[0] = gMarioState->pos[0];
    focus[1] = gMarioState->pos[1] + focYOff;
    focus[2] = gMarioState->pos[2];
}

u8 sDanceCutsceneTable[] = {
    CUTSCENE_DANCE_FLY_AWAY,
    CUTSCENE_DANCE_ROTATE,
    CUTSCENE_DANCE_CLOSEUP,
    CUTSCENE_KEY_DANCE,
    CUTSCENE_DANCE_DEFAULT,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    TRUE,
};

/**
 * The mode used when C-Up is pressed.
 */
s32 update_c_up(Vec3f focus, Vec3f pos) {
    s32 pitch = sCUpCameraPitch;
    s32 yaw = gMarioState->faceAngle[1] + sModeOffsetYaw + DEGREES(180);

    focus_on_mario(focus, pos, 125.f, 125.f, 250.f, pitch, yaw);
    return gMarioState->faceAngle[1];
}

/**
 * Zooms the camera in for C-Up mode
 */
void move_into_c_up(struct Camera *c) {
    struct LinearTransitionPoint *start = &sModeInfo.transitionStart;
    struct LinearTransitionPoint *end = &sModeInfo.transitionEnd;

    f32 dist = end->dist - start->dist;
    s32 pitch = end->pitch - start->pitch;
    s32 yaw = end->yaw - start->yaw;

    // Linearly interpolate from start to end position's polar coordinates
    dist = start->dist + dist * sModeInfo.frame / sModeInfo.max;
    pitch = start->pitch + pitch * sModeInfo.frame / sModeInfo.max;
    yaw = start->yaw + yaw * sModeInfo.frame / sModeInfo.max;

    // Linearly interpolate the focus from start to end
    c->focus[0] = start->focus[0] + (end->focus[0] - start->focus[0]) * sModeInfo.frame / sModeInfo.max;
    c->focus[1] = start->focus[1] + (end->focus[1] - start->focus[1]) * sModeInfo.frame / sModeInfo.max;
    c->focus[2] = start->focus[2] + (end->focus[2] - start->focus[2]) * sModeInfo.frame / sModeInfo.max;

    vec3f_add(c->focus, gMarioState->pos);
    vec3f_set_dist_and_angle(c->focus, c->pos, dist, pitch, yaw);

    gPlayerCameraState.headRotation[0] = 0;
    gPlayerCameraState.headRotation[1] = 0;

    // Finished zooming in
    if (++sModeInfo.frame == sModeInfo.max) {
        gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;
    }
}

/**
 * The main update function for C-Up mode
 */
s32 mode_c_up_camera(struct Camera *c);

/**
 * Used when Mario is in a cannon.
 */
__attribute__((always_inline))  inline  s32 update_in_cannon(Vec3f focus, Vec3f pos) {
    focus_on_mario(pos, focus, 125.f + sCannonYOffset, 125.f, 800.f, gMarioState->faceAngle[0],
                   gMarioState->faceAngle[1]);
    return gMarioState->faceAngle[1];
}

/**
 * Updates the camera when Mario is in a cannon.
 * sCannonYOffset is used to make the camera rotate down when Mario has just entered the cannon
 */
__attribute__((always_inline))  inline  void mode_cannon_camera(struct Camera *c) {

    gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;
    c->nextYaw = update_in_cannon(c->focus, c->pos);
    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        set_camera_mode(c, CAM_MODE_NEWCAM, 1);
        newcam_yaw = c->nextYaw - 0x8000;
        sCannonYOffset = 0.f;
        sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    } else {
        sCannonYOffset = approach_f32(sCannonYOffset, 0.f, 100.f, 100.f);
    }
}

void set_camera_mode(struct Camera *c, s32 mode, s32 frames) { // puppycam
    struct LinearTransitionPoint *start = &sModeInfo.transitionStart;
    struct LinearTransitionPoint *end = &sModeInfo.transitionEnd;

    // Clear movement flags that would affect the transition
    gCameraMovementFlags &= (u16) ~(CAM_MOVE_RESTRICT | CAM_MOVE_ROTATE);
    gCameraMovementFlags |= CAM_MOVING_INTO_MODE;
    sCUpCameraPitch = newcam_pitch;
    sModeOffsetYaw = 0;

    sModeInfo.newMode = (mode != -1) ? mode : sModeInfo.lastMode;
    sModeInfo.lastMode = c->mode;
    sModeInfo.max = frames;
    sModeInfo.frame = 1;

    c->mode = sModeInfo.newMode;
    gLakituState.mode = c->mode;

    vec3f_copy(end->focus, c->focus);
    vec3f_sub(end->focus, gMarioState->pos);

    vec3f_copy(end->pos, c->pos);
    vec3f_sub(end->pos, gMarioState->pos);

    vec3f_copy(start->focus, gLakituState.curFocus);
    vec3f_sub(start->focus, gMarioState->pos);

    vec3f_copy(start->pos, gLakituState.curPos);
    vec3f_sub(start->pos, gMarioState->pos);

    vec3f_get_dist_and_angle(start->focus, start->pos, &start->dist, &start->pitch, &start->yaw);
    vec3f_get_dist_and_angle(end->focus, end->pos, &end->dist, &end->pitch, &end->yaw);
}

/**
 * When sHandheldShakeMag is nonzero, this function adds small random offsets to `focus` every time
 * sHandheldShakeTimer increases above 1.0, simulating the camera shake caused by unsteady hands.
 *
 * This function must be called every frame in order to actually apply the effect, since the effect's
 * mag and inc are set to 0 every frame at the end of this function.
 */
__attribute__((always_inline))  inline  void shake_camera_handheld(Vec3f pos, Vec3f focus) {
    s32 i;
    Vec3f shakeOffset;
    Vec3f shakeSpline[4];
    f32 dist;
    s32 pitch;
    s32 yaw;

    if (sHandheldShakeMag == 0) {
        vec3f_set(shakeOffset, 0.f, 0.f, 0.f);
    } else {
        for (i = 0; i < 4; i++) {
            shakeSpline[i][0] = sHandheldShakeSpline[i].point[0];
            shakeSpline[i][1] = sHandheldShakeSpline[i].point[1];
            shakeSpline[i][2] = sHandheldShakeSpline[i].point[2];
        }
        evaluate_cubic_spline(sHandheldShakeTimer, shakeOffset, shakeSpline[0], shakeSpline[1],
                              shakeSpline[2], shakeSpline[3]);
        if (1.f <= (sHandheldShakeTimer += sHandheldShakeInc)) {
            // The first 3 control points are always (0,0,0), so the random spline is always just a
            // straight line
            for (i = 0; i < 3; i++) {
                vec3s_copy(sHandheldShakeSpline[i].point, sHandheldShakeSpline[i + 1].point);
            }
            random_vec3s(sHandheldShakeSpline[3].point, sHandheldShakeMag, sHandheldShakeMag,
                         sHandheldShakeMag / 2);
            sHandheldShakeTimer -= 1.f;

            // Code dead, this is set to be 0 before it is used.
            sHandheldShakeInc = random_float() * 0.5f;
            if (sHandheldShakeInc < 0.02f) {
                sHandheldShakeInc = 0.02f;
            }
        }
    }

    sHandheldShakePitch = approach_s16_asymptotic(sHandheldShakePitch, shakeOffset[0], 0x08);
    sHandheldShakeYaw = approach_s16_asymptotic(sHandheldShakeYaw, shakeOffset[1], 0x08);
    sHandheldShakeRoll = approach_s16_asymptotic(sHandheldShakeRoll, shakeOffset[2], 0x08);

    if (sHandheldShakePitch | sHandheldShakeYaw) {
        vec3f_get_dist_and_angle(pos, focus, &dist, &pitch, &yaw);
        pitch += sHandheldShakePitch;
        yaw += sHandheldShakeYaw;
        vec3f_set_dist_and_angle(pos, focus, dist, pitch, yaw);
    }

    // Unless called every frame, the effect will stop after the first time.
    sHandheldShakeMag = 0;
    sHandheldShakeInc = 0.f;
}
/**
 * Updates Lakitu's position/focus and applies camera shakes.
 */
__attribute__((always_inline))  inline  void update_lakitu(struct Camera *c) {
    struct Surface *floor = NULL;
    Vec3f newPos;
    Vec3f newFoc;
    f32 distToFloor;
    s32 newYaw;

    if (gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN) {
    } else {
        newYaw =
            next_lakitu_state(newPos, newFoc, c->pos, c->focus, sOldPosition, sOldFocus, c->nextYaw);
        if (c->cutscene == 0) {
            set_or_approach_s16_symmetric(&c->yaw, newYaw, 0x400);
        }
        sStatusFlags &= ~CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;

        // Update old state
        vec3f_copy(sOldPosition, newPos);
        vec3f_copy(sOldFocus, newFoc);

        gLakituState.yaw = c->yaw;
        gLakituState.nextYaw = c->nextYaw;
        vec3f_copy(gLakituState.goalPos, c->pos);
        vec3f_copy(gLakituState.goalFocus, c->focus);

        vec3f_approach_smoothly(gLakituState.curPos, newPos, gLakituState.posHSpeed, 0.f);
        vec3f_approach_smoothly(gLakituState.curFocus, newFoc, gLakituState.focHSpeed, 0.f);

        // Adjust Lakitu's speed back to normal
        set_or_approach_f32_asymptotic(&gLakituState.focHSpeed, 0.3f, 0.05f);
        set_or_approach_f32_asymptotic(&gLakituState.posHSpeed, 0.3f, 0.05f);

        // Turn on smooth movement when it hasn't been blocked for 2 frames
        if (sStatusFlags & CAM_FLAG_BLOCK_SMOOTH_MOVEMENT) {
            sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
        } else {
            sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
        }

        vec3f_copy(gLakituState.pos, gLakituState.curPos);
        vec3f_copy(gLakituState.focus, gLakituState.curFocus);

        vec3f_get_dist_and_angle(gLakituState.pos, gLakituState.focus, &gLakituState.focusDistance,
                                 &gLakituState.oldPitch, &gLakituState.oldYaw);

        gLakituState.roll = 0;

        // Apply camera shakes
        shake_camera_pitch(gLakituState.pos, gLakituState.focus);
        shake_camera_yaw(gLakituState.pos, gLakituState.focus);
        shake_camera_roll(&gLakituState.roll);
        shake_camera_handheld(gLakituState.pos, gLakituState.focus);

        if (gMarioState->action == ACT_DIVE && gLakituState.lastFrameAction != ACT_DIVE) {
            set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
        }

        gLakituState.roll += sHandheldShakeRoll;
        gLakituState.roll += gLakituState.keyDanceRoll;

        vec3f_copy(sModeTransition.marioPos, gMarioState->pos);
    }
    clamp_pitch(gLakituState.pos, gLakituState.focus, 0x3E00, -0x3E00);
    gLakituState.mode = c->mode;
    gLakituState.defMode = c->defMode;
}

/**
 * The main camera update function.
 * Gets controller input, checks for cutscenes, handles mode changes, and moves the camera
 */
__attribute__((always_inline))  inline  void init_camera(struct Camera *c);
void update_camera(struct Camera *c) {
    rendermariofromCam = 1;
    gCamera = c;

    // Initialize the camera
    // sStatusFlags &= ~CAM_FLAG_FRAME_AFTER_CAM_INIT;
    if (gCameraMovementFlags & CAM_MOVE_INIT_CAMERA) {
        init_camera(c);
        gCameraMovementFlags &= ~CAM_MOVE_INIT_CAMERA;
        // sStatusFlags |= CAM_FLAG_FRAME_AFTER_CAM_INIT;
    }

    // Store previous geometry information
    vec3f_copy(c->pos, gLakituState.goalPos);
    vec3f_copy(c->focus, gLakituState.goalFocus);

    c->yaw = gLakituState.yaw;
    c->nextYaw = gLakituState.nextYaw;
    c->mode = gLakituState.mode;
    c->defMode = gLakituState.defMode;

    if (c->cutscene != 0) {
        play_cutscene(c);
        sFramesSinceCutsceneEnded = 0;
    } else {
        // Clear the recent cutscene after 8 frames
        if (gRecentCutscene != 0 && sFramesSinceCutsceneEnded < 8) {
            sFramesSinceCutsceneEnded++;
            if (sFramesSinceCutsceneEnded >= 8) {
                gRecentCutscene = 0;
                sFramesSinceCutsceneEnded = 0;
            }
        }

        switch (c->mode) {
            case CAMERA_MODE_C_UP:
                mode_c_up_camera(c);
                break;

            case CAMERA_MODE_INSIDE_CANNON:
                mode_cannon_camera(c);
                break;

            // case CAM_MODE_NEWCAM:
            default:
                newcam_loop(c);
                break;
        }
    }
    // Start any Mario-related cutscenes
    start_cutscene(c, get_cutscene_from_mario_status(c));

    update_lakitu(c);

    gLakituState.lastFrameAction = gMarioState->action;
}

/**
 * Reset all the camera variables to their arcane defaults
 */
void reset_camera(struct Camera *c) {

    gCamera = c;
    gCameraMovementFlags = 0;
    sStatusFlags = 0;
    gCutsceneTimer = 0;
    sCutsceneShot = 0;
    gObjCutsceneDone = FALSE;
    gCutsceneFocus = NULL;
    vec3f_copy(sModeTransition.marioPos, gMarioState->pos);
    sModeTransition.framesLeft = 0;
    gCameraMovementFlags = 0;
    gCameraMovementFlags |= CAM_MOVE_INIT_CAMERA;
    sStatusFlags = 0;
    sCUpCameraPitch = 0;
    sModeOffsetYaw = 0;
    sCannonYOffset = 0.f;
    c->doorStatus = DOOR_DEFAULT;
    gPlayerCameraState.headRotation[0] = 0;
    gPlayerCameraState.headRotation[1] = 0;
    // gPlayerCameraState.cameraEvent = 0;
    gPlayerCameraState.usedObj = NULL;
    gLakituState.shakeMagnitude[0] = 0;
    gLakituState.shakeMagnitude[1] = 0;
    gLakituState.shakeMagnitude[2] = 0;
    gLakituState.unusedVec2[0] = 0;
    gLakituState.unusedVec2[1] = 0;
    gLakituState.unusedVec2[2] = 0;
    gLakituState.unusedVec1[0] = 0.f;
    gLakituState.unusedVec1[1] = 0.f;
    gLakituState.unusedVec1[2] = 0.f;
    gLakituState.lastFrameAction = 0;
    set_fov_function(CAM_FOV_DEFAULT);
    sFOVState.fov = 45.f;
    sFOVState.fovOffset = 0.f;
    sFOVState.shakeAmplitude = 0.f;
    sFOVState.shakePhase = 0;
    sObjectCutscene = 0;
    gRecentCutscene = 0;
    if ((((gCurrCourseNum < 16) && (gCurrCourseNum > 0)) || gCurrLevelNum == LEVEL_CASTLE_GROUNDS)
        && (LevelTimer ==0)) {
        gMarioState->prevAction = gMarioState->action;
        gMarioState->statusForCamera->cameraEvent = 0xc8;
        play_music(0, SEQ_EVENT_YOSHIBEAT, 0);
        advance_cutscene_step(gMarioState);
        save_file_set_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum, 0x80);
    } else {
        gPlayerCameraState.cameraEvent = 0;
    }
}

__attribute__((always_inline))  inline  void init_camera(struct Camera *c) {
    struct Surface *floor = 0;
    Vec3f marioOffset;
    s32 i;

    gLakituState.mode = c->mode;
    gLakituState.defMode = c->defMode;
    gLakituState.posHSpeed = 0.3f;
    gLakituState.focHSpeed = 0.3f; // @bug set focHSpeed back-to-back
    gLakituState.roll = 0;
    gLakituState.keyDanceRoll = 0;
    gLakituState.unused = 0;
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    for (i = 0; i < SPLINESEGMENTS; i++) {
        sCurCreditsSplinePos[i].index = -1;
        sCurCreditsSplineFocus[i].index = -1;
    }
    sCutsceneSplineSegment = 0;
    sCutsceneSplineSegmentProgress = 0.f;
    sHandheldShakeInc = 0.f;
    sHandheldShakeTimer = 0.f;
    sHandheldShakeMag = 0;
    for (i = 0; i < 4; i++) {
        sHandheldShakeSpline[i].index = -1;
    }
    sHandheldShakePitch = 0;
    sHandheldShakeYaw = 0;
    sHandheldShakeRoll = 0;
    c->cutscene = 0;
    marioOffset[0] = 0.f;
    marioOffset[1] = 125.f;
    marioOffset[2] = 400.f;

    // Set the camera's starting position or start a cutscene for certain levels

    // Set the camera pos to marioOffset (relative to Mario), added to Mario's position
    offset_rotated(c->pos, gMarioState->pos, marioOffset, gMarioState->faceAngle);
    if (c->mode != CAMERA_MODE_BEHIND_MARIO) {
        c->pos[1] =
            find_floor(gMarioState->pos[0], gMarioState->pos[1] + 100.f, gMarioState->pos[2], &floor)
            + 125.f;
    }
    vec3f_copy(c->focus, gMarioState->pos);
    vec3f_copy(gLakituState.curPos, c->pos);
    vec3f_copy(gLakituState.curFocus, c->focus);
    vec3f_copy(gLakituState.goalPos, c->pos);
    vec3f_copy(gLakituState.goalFocus, c->focus);
    vec3f_copy(gLakituState.pos, c->pos);
    vec3f_copy(gLakituState.focus, c->focus);
    gLakituState.yaw = calculate_yaw(c->focus, c->pos);
    gLakituState.nextYaw = gLakituState.yaw;
    c->yaw = gLakituState.yaw;
    c->nextYaw = gLakituState.yaw;

    gLakituState.mode = CAM_MODE_NEWCAM;
    gLakituState.defMode = CAM_MODE_NEWCAM;
    newcam_init(c, 0);
}

/**
 * Allocate the GraphNodeCamera's config.camera, and copy `c`'s focus to the Camera's area center point.
 */
__attribute__((always_inline)) inline   void create_camera(struct GraphNodeCamera *gc,
                                                  struct AllocOnlyPool *pool) {
    s32 mode = gc->config.mode;
    struct Camera *c = alloc_only_pool_alloc(pool, sizeof(struct Camera));

    gc->config.camera = c;
    c->mode = mode;
    c->defMode = mode;
    c->cutscene = 0;
    c->doorStatus = DOOR_DEFAULT;
    c->areaCenX = gc->focus[0];
    c->areaCenY = gc->focus[1];
    c->areaCenZ = gc->focus[2];
    c->yaw = 0;
    vec3f_copy(c->pos, gc->pos);
    vec3f_copy(c->focus, gc->focus);
}

/**
 * Copy Lakitu's pos and foc into `gc`
 */
__attribute__((always_inline))  inline  void update_graph_node_camera(struct GraphNodeCamera *gc) {

    gc->rollScreen = gLakituState.roll;
    if (!gMoveSpeed) {
        gc->pos[0] = gLakituState.pos[0];
        gc->pos[1] = gLakituState.pos[1];
        gc->pos[2] = gLakituState.pos[2];
        gc->focus[0] = gLakituState.focus[0];
        gc->focus[1] = gLakituState.focus[1];
        gc->focus[2] = gLakituState.focus[2];
    } else {
        if (gMoveSpeed == 1) {
            gc->pos[0] = approach_pos(gc->pos[0], gLakituState.pos[0]);
            gc->pos[1] = approach_pos(gc->pos[1], gLakituState.pos[1]);
            gc->pos[2] = approach_pos(gc->pos[2], gLakituState.pos[2]);
            gc->focus[0] = approach_pos(gc->focus[0], gLakituState.focus[0]);
            gc->focus[1] = approach_pos(gc->focus[1], gLakituState.focus[1]);
            gc->focus[2] = approach_pos(gc->focus[2], gLakituState.focus[2]);
        } else {
            gc->pos[0] = approach_posF(gc->pos[0], gLakituState.pos[0]);
            gc->pos[1] = approach_posF(gc->pos[1], gLakituState.pos[1]);
            gc->pos[2] = approach_posF(gc->pos[2], gLakituState.pos[2]);
            gc->focus[0] = approach_posF(gc->focus[0], gLakituState.focus[0]);
            gc->focus[1] = approach_posF(gc->focus[1], gLakituState.focus[1]);
            gc->focus[2] = approach_posF(gc->focus[2], gLakituState.focus[2]);
        }
    }

    // vec3f_copy(gc->pos, gLakituState.pos);
    // vec3f_copy(gc->focus, gLakituState.focus);
    // zoom_out_if_paused_and_outside(gc);
}

Gfx *geo_camera_main(s32 callContext, struct GraphNode *g, void *context) {
    struct GraphNodeCamera *gc = (struct GraphNodeCamera *) g;

    switch (callContext) {
        case GEO_CONTEXT_CREATE:
            create_camera(gc, context);
            break;
        case GEO_CONTEXT_RENDER:
            update_graph_node_camera(gc);
            break;
    }
    return NULL;
}

void vec3f_sub(Vec3f dst, Vec3f src) {
    dst[0] -= src[0];
    dst[1] -= src[1];
    dst[2] -= src[2];
}

void object_pos_to_vec3f(Vec3f dst, struct Object *o) {
    dst[0] = o->oPosX;
    dst[1] = o->oPosY;
    dst[2] = o->oPosZ;
}

void vec3f_to_object_pos(struct Object *o, Vec3f src) {
    o->oPosX = src[0];
    o->oPosY = src[1];
    o->oPosZ = src[2];
}

/**
 * Produces values using a cubic b-spline curve. Basically Q is the used output,
 * u is a value between 0 and 1 that represents the position along the spline,
 * and a0-a3 are parameters that define the spline.
 *
 * The spline is described at www2.cs.uregina.ca/~anima/408/Notes/Interpolation/UniformBSpline.htm
 */
void evaluate_cubic_spline(f32 u, Vec3f Q, Vec3f a0, Vec3f a1, Vec3f a2, Vec3f a3) {
    f32 B[4];
    f32 x;
    f32 y;
    f32 z;

    if (u > 1.f) {
        u = 1.f;
    }

    B[0] = (1.f - u) * (1.f - u) * (1.f - u) / 6.f;
    B[1] = u * u * u / 2.f - u * u + 0.6666667f;
    B[2] = -u * u * u / 2.f + u * u / 2.f + u / 2.f + 0.16666667f;
    B[3] = u * u * u / 6.f;

    Q[0] = B[0] * a0[0] + B[1] * a1[0] + B[2] * a2[0] + B[3] * a3[0];
    Q[1] = B[0] * a0[1] + B[1] * a1[1] + B[2] * a2[1] + B[3] * a3[1];
    Q[2] = B[0] * a0[2] + B[1] * a1[2] + B[2] * a2[2] + B[3] * a3[2];

    // Unused code
    /*B[0] = -0.5f * u * u + u - 0.33333333f;
    B[1] = 1.5f * u * u - 2.f * u - 0.5f;
    B[2] = -1.5f * u * u + u + 1.f;
    B[3] = 0.5f * u * u - 0.16666667f;

    x = B[0] * a0[0] + B[1] * a1[0] + B[2] * a2[0] + B[3] * a3[0];
    y = B[0] * a0[1] + B[1] * a1[1] + B[2] * a2[1] + B[3] * a3[1];
    z = B[0] * a0[2] + B[1] * a1[2] + B[2] * a2[2] + B[3] * a3[2];*/
}

/**
 * Computes the point that is `progress` percent of the way through segment `splineSegment` of `spline`,
 * and stores the result in `p`. `progress` and `splineSegment` are updated if `progress` becomes
 * >= 1.0.
 *
 * When neither of the next two points' speeds == 0, the number of frames is between 1 and 255.
 * Otherwise it's infinite.
 *
 * To calculate the number of frames it will take to progress through a spline segment:
 * If the next two speeds are the same and nonzero, it's 1.0 / firstSpeed.
 *
 * s1 and s2 are short hand for first/secondSpeed. The progress at any frame n is defined by a
 * recurrency relation: p(n+1) = (s2 - s1 + 1) * p(n) + s1 Which can be written as p(n) = (s2 * ((s2 -
 * s1 + 1)^(n) - 1)) / (s2 - s1)
 *
 * Solving for the number of frames:
 *      n = log(((s2 - s1) / s1) + 1) / log(s2 - s1 + 1)
 *
 * @return 1 if the point has reached the end of the spline, when `progress` reaches 1.0 or greater, and
 * the 4th CutsceneSplinePoint in the current segment away from spline[splineSegment] has an index of
 * -1.
 */
s32 move_point_along_spline(Vec3f p, struct CutsceneSplinePoint spline[], s16 *splineSegment,
                            f32 *progress) {
    s32 finished = 0;
    Vec3f controlPoints[4];
    s32 i = 0;
    f32 u = *progress;
    f32 progressChange;
    f32 firstSpeed = 0;
    f32 secondSpeed = 0;
    s32 segment = *splineSegment;

    if (*splineSegment < 0) {
        segment = 0;
        u = 0;
    }
    if (spline[segment].index == -1 || spline[segment + 1].index == -1
        || spline[segment + 2].index == -1) {
        return 1;
    }

    for (i = 0; i < 4; i++) {
        controlPoints[i][0] = spline[segment + i].point[0];
        controlPoints[i][1] = spline[segment + i].point[1];
        controlPoints[i][2] = spline[segment + i].point[2];
    }
    evaluate_cubic_spline(u, p, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);

    if (spline[*splineSegment + 1].speed != 0) {
        firstSpeed = 1.0f / spline[*splineSegment + 1].speed;
    }
    if (spline[*splineSegment + 2].speed != 0) {
        secondSpeed = 1.0f / spline[*splineSegment + 2].speed;
    }
    progressChange = (secondSpeed - firstSpeed) * *progress + firstSpeed;

    if (1 <= (*progress += progressChange)) {
        (*splineSegment)++;
        if (spline[*splineSegment + 3].index == -1) {
            *splineSegment = 0;
            finished = 1;
        }
        *progress -= 1;
    }
    return finished;
}

/**
 * Enables the handheld shake effect for this frame.
 *
 * @see shake_camera_handheld()
 */
void set_handheld_shake(s32 mode) {
    switch (mode) {
        // They're not in numerical order because that would be too simple...
        case HAND_CAM_SHAKE_CUTSCENE: // Lowest increment
            sHandheldShakeMag = 0x600;
            sHandheldShakeInc = 0.04f;
            break;
        case HAND_CAM_SHAKE_LOW: // Lowest magnitude
            sHandheldShakeMag = 0x300;
            sHandheldShakeInc = 0.06f;
            break;
        case HAND_CAM_SHAKE_HIGH: // Highest mag and inc
            sHandheldShakeMag = 0x1000;
            sHandheldShakeInc = 0.1f;
            break;
        case HAND_CAM_SHAKE_UNUSED: // Never used
            sHandheldShakeMag = 0x600;
            sHandheldShakeInc = 0.07f;
            break;
        case HAND_CAM_SHAKE_HANG_OWL: // exactly the same as UNUSED...
            sHandheldShakeMag = 0x600;
            sHandheldShakeInc = 0.07f;
            break;
        case HAND_CAM_SHAKE_STAR_DANCE: // Slightly steadier than HANG_OWL and UNUSED
            sHandheldShakeMag = 0x400;
            sHandheldShakeInc = 0.07f;
            break;
        default:
            sHandheldShakeMag = 0x0;
            sHandheldShakeInc = 0.f;
    }
}

__attribute__((always_inline))  inline  s32 clamp_pitch(Vec3f from, Vec3f to, s32 maxPitch, s32 minPitch) {
    s32 outOfRange = 0;
    s16 pitch;
    s16 yaw;
    f32 dist;

    vec3f_get_dist_and_angle(from, to, &dist, &pitch, &yaw);
    if (pitch > maxPitch) {
        pitch = maxPitch;
        outOfRange++;
    }
    if (pitch < minPitch) {
        pitch = minPitch;
        outOfRange++;
    }
    vec3f_set_dist_and_angle(from, to, dist, pitch, yaw);
    return outOfRange;
}

void set_or_approach_f32_asymptotic(f32 *dst, f32 goal, f32 scale) {
    if (sStatusFlags & CAM_FLAG_SMOOTH_MOVEMENT) {
        *dst = approach_f32_asymptotic(*dst, goal, scale);
    } else {
        *dst = goal;
    }
}

/**
 * Nearly the same as the above function, returns new value instead.
 */
f32 approach_f32_asymptotic(f32 current, f32 target, f32 multiplier) {
    return current + (target - current) * multiplier;
}

/**
 * Approaches an s16 value in the same fashion as approach_f32_asymptotic, returns the new value.
 * Note: last parameter is the reciprocal of what it would be in the f32 functions
 */
s32 approach_s16_asymptotic(s32 current, s32 target, s32 divisor) {
    s32 temp = current;

    if (divisor == 0) {
        current = target;
    } else {
        temp -= target;
        temp -= temp / divisor;
        temp += target;
        current = temp;
    }
    return current;
}

void approach_vec3f_asymptotic(Vec3f current, Vec3f target, f32 xMul, f32 yMul, f32 zMul) {
    current[0] = approach_f32_asymptotic(current[0], target[0], xMul);
    current[1] = approach_f32_asymptotic(current[1], target[1], yMul);
    current[2] = approach_f32_asymptotic(current[2], target[2], zMul);
}

void set_or_approach_vec3f_asymptotic(Vec3f dst, Vec3f goal, f32 xMul, f32 yMul, f32 zMul) {
    set_or_approach_f32_asymptotic(&dst[0], goal[0], xMul);
    set_or_approach_f32_asymptotic(&dst[1], goal[1], yMul);
    set_or_approach_f32_asymptotic(&dst[2], goal[2], zMul);
}

s32 camera_approach_s16_symmetric_bool(s16 *current, s32 target, s32 increment) {
    s32 dist = target - *current;

    if (increment < 0) {
        increment = -1 * increment;
    }
    if (dist > 0) {
        dist -= increment;
        if (dist >= 0) {
            *current = target - dist;
        } else {
            *current = target;
        }
    } else {
        dist += increment;
        if (dist <= 0) {
            *current = target - dist;
        } else {
            *current = target;
        }
    }
    if (*current == target) {
        return FALSE;
    } else {
        return TRUE;
    }
}

void set_or_approach_s16_symmetric(s16 *current, s32 target, s32 increment) {
    if (sStatusFlags & CAM_FLAG_SMOOTH_MOVEMENT) {
        *current = approach_s16_symmetric(*current, target, increment);
    } else {
        *current = target;
    }
}

/**
 * Nearly the same as the above function, this one returns the new value in place of a bool.
 */
f32 camera_approach_f32_symmetric(f32 current, f32 target, f32 increment) {
    f32 dist = target - current;

    if (increment < 0) {
        increment = -1 * increment;
    }
    if (dist > 0) {
        dist -= increment;
        if (dist > 0) {
            current = target - dist;
        } else {
            current = target;
        }
    } else {
        dist += increment;
        if (dist < 0) {
            current = target - dist;
        } else {
            current = target;
        }
    }
    return current;
}

/**
 * Generate a vector with all three values about zero. The
 * three ranges determine how wide the range about zero.
 */
__attribute__((always_inline))  inline  void random_vec3s(Vec3s dst, s32 xRange, s32 yRange, s32 zRange) {
    f32 randomFloat;
    f32 tempXRange;
    f32 tempYRange;
    f32 tempZRange;

    randomFloat = random_float();
    tempXRange = xRange;
    dst[0] = randomFloat * tempXRange - tempXRange / 2;

    randomFloat = random_float();
    tempYRange = yRange;
    dst[1] = randomFloat * tempYRange - tempYRange / 2;

    randomFloat = random_float();
    tempZRange = zRange;
    dst[2] = randomFloat * tempZRange - tempZRange / 2;
}

/**
 * Decrease value by multiplying it by the distance from (`posX`, `posY`, `posZ`) to
 * the camera divided by `maxDist`
 *
 * @return the reduced value
 */
s32 reduce_by_dist_from_camera(s32 value, f32 maxDist, f32 posX, f32 posY, f32 posZ) {
    Vec3f pos;
    f32 dist;
    s16 pitch;
    s16 yaw;
    s16 goalPitch;
    s16 goalYaw;
    s32 result = 0;
    // Direction from pos to (Lakitu's) goalPos
    f32 goalDX = gLakituState.goalPos[0] - posX;
    f32 goalDY = gLakituState.goalPos[1] - posY;
    f32 goalDZ = gLakituState.goalPos[2] - posZ;

    dist = sqrtf(goalDX * goalDX + goalDY * goalDY + goalDZ * goalDZ);
    if (maxDist > dist) {
        pos[0] = posX;
        pos[1] = posY;
        pos[2] = posZ;
        vec3f_get_dist_and_angle(gLakituState.goalPos, pos, &dist, &pitch, &yaw);
        if (dist < maxDist) {
            calculate_angles(gLakituState.goalPos, gLakituState.goalFocus, &goalPitch, &goalYaw);
            //! Must be same line to match EU
            pitch -= goalPitch;
            yaw -= goalYaw;
            dist -= 2000.f;
            if (dist < 0.f) {
                dist = 0.f;
            }
            maxDist -= 2000.f;
            if (maxDist < 2000.f) {
                maxDist = 2000.f;
            }
            result = value * (1.f - dist / maxDist);
            if (pitch < -0x1800 || pitch > 0x400 || yaw < -0x1800 || yaw > 0x1800) {
                result /= 2;
            }
        }
    }
    return result;
}

/**
 * Calculates the distance between two points and sets a vector to a point
 * scaled along a line between them. Typically, somewhere in the middle.
 */
__attribute__((always_inline))  inline  void scale_along_line(Vec3f dst, Vec3f from, Vec3f to, f32 scale) {
    Vec3f tempVec;

    tempVec[0] = (to[0] - from[0]) * scale + from[0];
    tempVec[1] = (to[1] - from[1]) * scale + from[1];
    tempVec[2] = (to[2] - from[2]) * scale + from[2];
    vec3f_copy(dst, tempVec);
}
s32 calculate_pitch(Vec3f from, Vec3f to) {
    f32 dx = to[0] - from[0];
    f32 dy = to[1] - from[1];
    f32 dz = to[2] - from[2];

    return atan2s(sqrtf(dx * dx + dz * dz), dy);
}

s32 calculate_yaw(Vec3f from, Vec3f to) {
    f32 dx = to[0] - from[0];
    f32 dz = to[2] - from[2];

    return atan2s(dz, dx);
}

/**
 * Calculates the pitch and yaw between two vectors.
 */
__attribute__((always_inline))  inline  void calculate_angles(Vec3f from, Vec3f to, s16 *pitch, s16 *yaw) {
    f32 dx = to[0] - from[0];
    f32 dy = to[1] - from[1];
    f32 dz = to[2] - from[2];

    *pitch = atan2s(sqrtf(dx * dx + dz * dz), dy);
    *yaw = atan2s(dz, dx);
}

/**
 * Finds the distance between two vectors.
 */
__attribute__((always_inline))  inline  f32 calc_abs_dist(Vec3f a, Vec3f b) {
    f32 distX = b[0] - a[0];
    f32 distY = b[1] - a[1];
    f32 distZ = b[2] - a[2];
    f32 distAbs = sqrtf(distX * distX + distY * distY + distZ * distZ);

    return distAbs;
}

/**
 * Start shaking the camera's pitch (up and down)
 */
void set_camera_pitch_shake(s32 mag, s32 decay, s32 inc) {
    if (gLakituState.shakeMagnitude[0] < mag) {
        gLakituState.shakeMagnitude[0] = mag;
        gLakituState.shakePitchDecay = decay;
        gLakituState.shakePitchVel = inc;
    }
}

/**
 * Start shaking the camera's roll (rotate screen clockwise and counterclockwise)
 */
void set_camera_roll_shake(s32 mag, s32 decay, s32 inc) {
    if (gLakituState.shakeMagnitude[2] < mag) {
        gLakituState.shakeMagnitude[2] = mag;
        gLakituState.shakeRollDecay = decay;
        gLakituState.shakeRollVel = inc;
    }
}

/**
 * Start shaking the camera's pitch, but reduce `mag` by it's distance from the camera
 */
void set_pitch_shake_from_point(s32 mag, s32 decay, s32 inc, f32 maxDist, f32 posX, f32 posY,
                                f32 posZ) {
    Vec3f pos;
    f32 dist;
    s16 dummyPitch;
    s16 dummyYaw;

    pos[0] = posX;
    pos[1] = posY;
    pos[2] = posZ;
    vec3f_get_dist_and_angle(gLakituState.goalPos, pos, &dist, &dummyPitch, &dummyYaw);
    mag = reduce_by_dist_from_camera(mag, maxDist, posX, posY, posZ);
    if (mag != 0) {
        set_camera_pitch_shake(mag, decay, inc);
    }
}
/**
 * Update the shake offset by `increment`
 */
void increment_shake_offset(s16 *offset, s32 increment) {
    if (increment == -0x8000) {
        *offset = (*offset & 0x8000) + 0xC000;
    } else {
        *offset += increment;
    }
}

/**
 * Apply a vertical shake to the camera by adjusting its pitch
 */
__attribute__((always_inline))  inline  void shake_camera_pitch(Vec3f pos, Vec3f focus) {
    f32 dist;
    s16 pitch;
    s16 yaw;

    if (gLakituState.shakeMagnitude[0] | gLakituState.shakeMagnitude[1]) {
        vec3f_get_dist_and_angle(pos, focus, &dist, &pitch, &yaw);
        pitch += gLakituState.shakeMagnitude[0] * sins(gLakituState.shakePitchPhase);
        vec3f_set_dist_and_angle(pos, focus, dist, pitch, yaw);
        increment_shake_offset(&gLakituState.shakePitchPhase, gLakituState.shakePitchVel);
        if (camera_approach_s16_symmetric_bool(&gLakituState.shakeMagnitude[0], 0,
                                               gLakituState.shakePitchDecay)
            == 0) {
            gLakituState.shakePitchPhase = 0;
        }
    }
}

/**
 * Apply a horizontal shake to the camera by adjusting its yaw
 */
__attribute__((always_inline))  inline  void shake_camera_yaw(Vec3f pos, Vec3f focus) {
    f32 dist;
    s16 pitch;
    s16 yaw;

    if (gLakituState.shakeMagnitude[1] != 0) {
        vec3f_get_dist_and_angle(pos, focus, &dist, &pitch, &yaw);
        yaw += gLakituState.shakeMagnitude[1] * sins(gLakituState.shakeYawPhase);
        vec3f_set_dist_and_angle(pos, focus, dist, pitch, yaw);
        increment_shake_offset(&gLakituState.shakeYawPhase, gLakituState.shakeYawVel);
        if (camera_approach_s16_symmetric_bool(&gLakituState.shakeMagnitude[1], 0,
                                               gLakituState.shakeYawDecay)
            == 0) {
            gLakituState.shakeYawPhase = 0;
        }
    }
}

/**
 * Apply a rotational shake to the camera by adjusting its roll
 */
__attribute__((always_inline))  inline  void shake_camera_roll(s16 *roll) {

    if (gLakituState.shakeMagnitude[2] != 0) {
        increment_shake_offset(&gLakituState.shakeRollPhase, gLakituState.shakeRollVel);
        *roll += gLakituState.shakeMagnitude[2] * sins(gLakituState.shakeRollPhase);
        if (camera_approach_s16_symmetric_bool(&gLakituState.shakeMagnitude[2], 0,
                                               gLakituState.shakeRollDecay)
            == 0) {
            gLakituState.shakeRollPhase = 0;
        }
    }
}
/**
 * Starts a cutscene dialog. Only has an effect when `trigger` is 1
 */
s32 trigger_cutscene_dialog(s32 trigger) {

    if (trigger == 1) {
        start_object_cutscene_without_focus(CUTSCENE_READ_MESSAGE);
    }
    return 0;
}

/**
 * Zero the 10 cvars.
 */
__attribute__((always_inline))  inline  void clear_cutscene_vars(UNUSED struct Camera *c) {
    s32 i;

    for (i = 0; i < 10; i++) {
        sCutsceneVars[i].unused1 = 0;
        vec3f_set(sCutsceneVars[i].point, 0.f, 0.f, 0.f);
        vec3f_set(sCutsceneVars[i].unusedPoint, 0.f, 0.f, 0.f);
        vec3s_set(sCutsceneVars[i].angle, 0, 0, 0);
        sCutsceneVars[i].unused2 = 0;
    }
}

/**
 * Start the cutscene, `cutscene`, if it is not already playing.
 */
__attribute__((always_inline))  inline  void start_cutscene(struct Camera *c, s32 cutscene) {
    if (c->cutscene != cutscene) {
        c->cutscene = cutscene;
        clear_cutscene_vars(c);
    }
}

/**
 * Look up the victory dance cutscene in sDanceCutsceneTable
 *
 * First the index entry is determined based on the course and the star that was just picked up
 * Like the entries in sZoomOutAreaMasks, each entry represents two stars
 * The current courses's 4 bits of the index entry are used as the actual index into sDanceCutsceneTable
 *
 * @return the victory cutscene to use
 */
s32 determine_dance_cutscene(UNUSED struct Camera *c) {
    s32 cutscene = 0;
    s32 cutsceneIndex = 0;
    s32 starIndex = (gLastCompletedStarNum - 1) / 2;
    s32 courseIndex = gCurrCourseNum;

    if (starIndex > 3) {
        starIndex = 0;
    }
    if (courseIndex > COURSE_MAX) {
        courseIndex = COURSE_NONE;
    }
    cutsceneIndex = sDanceCutsceneIndexTable[courseIndex][starIndex];

    if (gLastCompletedStarNum & 1) {
        // Odd stars take the lower four bytes
        cutsceneIndex &= 0xF;
    } else {
        // Even stars use the upper four bytes
        cutsceneIndex = cutsceneIndex >> 4;
    }
    cutscene = sDanceCutsceneTable[cutsceneIndex];
    return cutscene;
}

/**
 * @return `pullResult` or `pushResult` depending on Mario's door action
 */
__attribute__((always_inline))  inline  s32 open_door_cutscene(s32 pullResult, s32 pushResult) {
    if (gMarioState->action == ACT_PULLING_DOOR) {
        return pullResult;
    }
    if (gMarioState->action == ACT_PUSHING_DOOR) {
        return pushResult;
    }
}

/**
 * If no cutscenes are playing, determines if a cutscene should play based on Mario's action and
 * cameraEvent
 *
 * @return the cutscene that should start, 0 if none
 */
__attribute__((always_inline))  inline  s32 get_cutscene_from_mario_status(struct Camera *c) {
    s32 cutscene = c->cutscene;

    if (cutscene == 0) {
        // A cutscene started by an object, if any, will start if nothing else happened
        cutscene = sObjectCutscene;
        sObjectCutscene = 0;
        if (gPlayerCameraState.cameraEvent == CAM_EVENT_DOOR) {
            cutscene = open_door_cutscene(CUTSCENE_DOOR_PULL, CUTSCENE_DOOR_PUSH);
        }
        if (gPlayerCameraState.cameraEvent == CAM_EVENT_DOOR_WARP) {
            cutscene = CUTSCENE_DOOR_WARP;
        }
        if (gPlayerCameraState.cameraEvent == CAM_EVENT_CANNON) {
            cutscene = CUTSCENE_ENTER_CANNON;
        }
        if (gPlayerCameraState.cameraEvent == 0xc8) {
            cutscene = 0xc9;
        }

        switch (gMarioState->action) {
            case ACT_DEATH_EXIT:
                cutscene = CUTSCENE_DEATH_EXIT;
                break;
            case ACT_EXIT_AIRBORNE:
                cutscene = CUTSCENE_EXIT_PAINTING_SUCC;
                break;
            case ACT_SPECIAL_EXIT_AIRBORNE:
                cutscene = CUTSCENE_EXIT_SPECIAL_SUCC;
                break;
            case ACT_SPECIAL_DEATH_EXIT:
                cutscene = CUTSCENE_NONPAINTING_DEATH;
                break;
            case ACT_ENTERING_STAR_DOOR:
                if (c->doorStatus == DOOR_DEFAULT) {
                    cutscene = CUTSCENE_SLIDING_DOORS_OPEN;
                } else {
                    cutscene = CUTSCENE_DOOR_PULL_MODE;
                }
                break;
            case ACT_UNLOCKING_KEY_DOOR:
                cutscene = CUTSCENE_UNLOCK_KEY_DOOR;
                break;
            case ACT_WATER_DEATH:
                cutscene = CUTSCENE_WATER_DEATH;
                break;
            case ACT_DEATH_ON_BACK:
                cutscene = CUTSCENE_DEATH_ON_BACK;
                break;
            case ACT_DEATH_ON_STOMACH:
                cutscene = CUTSCENE_DEATH_ON_STOMACH;
                break;
            case ACT_STANDING_DEATH:
                cutscene = CUTSCENE_STANDING_DEATH;
                break;
            case ACT_SUFFOCATION:
                cutscene = CUTSCENE_SUFFOCATION_DEATH;
                break;
        /*    case ACT_QUICKSAND_DEATH:
                cutscene = CUTSCENE_QUICKSAND_DEATH;*/
                break;
            case ACT_ELECTROCUTION:
                cutscene = CUTSCENE_STANDING_DEATH;
                break;
            case ACT_STAR_DANCE_EXIT:
                cutscene = determine_dance_cutscene(c);
                break;
            case ACT_STAR_DANCE_WATER:
                cutscene = determine_dance_cutscene(c);
                break;
            case ACT_STAR_DANCE_NO_EXIT:
                cutscene = CUTSCENE_DANCE_DEFAULT;
                break;
        }
        switch (gPlayerCameraState.cameraEvent) {
            case CAM_EVENT_START_INTRO:
                cutscene = CUTSCENE_INTRO_PEACH;
                break;
            case CAM_EVENT_START_GRAND_STAR:
                cutscene = CUTSCENE_GRAND_STAR;
                break;
            case CAM_EVENT_START_ENDING:
                cutscene = CUTSCENE_ENDING;
                break;
            case CAM_EVENT_START_END_WAVING:
                cutscene = CUTSCENE_END_WAVING;
                break;
            case CAM_EVENT_START_CREDITS:
                cutscene = CUTSCENE_CREDITS;
                break;
        }
    }
    //! doorStatus is reset every frame. CameraTriggers need to constantly set doorStatus
    c->doorStatus = DOOR_DEFAULT;

    return cutscene;
}

/**
 * Moves the camera when Mario has triggered a warp
 */
void warp_camera(f32 displacementX, f32 displacementY, f32 displacementZ) {
    Vec3f displacement;
    struct MarioState *marioStates = &gMarioStates[0];
    struct LinearTransitionPoint *start = &sModeInfo.transitionStart;
    struct LinearTransitionPoint *end = &sModeInfo.transitionEnd;

    displacement[0] = displacementX;
    displacement[1] = displacementY;
    displacement[2] = displacementZ;
    vec3f_add(gLakituState.curPos, displacement);
    vec3f_add(gLakituState.curFocus, displacement);
    vec3f_add(gLakituState.goalPos, displacement);
    vec3f_add(gLakituState.goalFocus, displacement);
    vec3f_add(newcam_pos_target, displacement);
    vec3f_add(newcam_pos, displacement);
    marioStates->waterLevel += displacementY;

    vec3f_add(start->focus, displacement);
    vec3f_add(start->pos, displacement);
    vec3f_add(end->focus, displacement);
    vec3f_add(end->pos, displacement);
}

/**
 * Set the camera's focus to Mario's position, and add several relative offsets.
 *
 * @param leftRight offset to Mario's left/right, relative to his faceAngle
 * @param yOff y offset
 * @param forwBack offset to Mario's front/back, relative to his faceAngle
 * @param yawOff offset to Mario's faceAngle, changes the direction of `leftRight` and `forwBack`
 */
void set_focus_rel_mario(struct Camera *c, f32 leftRight, f32 yOff, f32 forwBack, s32 yawOff) {
    s32 yaw;
    f32 focFloorYOff;

    calc_y_to_curr_floor(&focFloorYOff, 1.f, 200.f, &focFloorYOff, 0.9f, 200.f);
    yaw = gMarioState->faceAngle[1] + yawOff;
    c->focus[2] = gMarioState->pos[2] + forwBack * coss(yaw) - leftRight * sins(yaw);
    c->focus[0] = gMarioState->pos[0] + forwBack * sins(yaw) + leftRight * coss(yaw);
    c->focus[1] = gMarioState->pos[1] + yOff + focFloorYOff;
}

void offset_rotated(Vec3f dst, Vec3f from, Vec3f to, Vec3s rotation) {
    Vec3f unusedCopy;
    Vec3f pitchRotated;

    vec3f_copy(unusedCopy, from);

    // First rotate the direction by rotation's pitch
    //! The Z axis is flipped here.
    pitchRotated[2] = -(to[2] * coss(rotation[0]) - to[1] * sins(rotation[0]));
    pitchRotated[1] = to[2] * sins(rotation[0]) + to[1] * coss(rotation[0]);
    pitchRotated[0] = to[0];

    // Rotate again by rotation's yaw
    dst[0] = from[0] + pitchRotated[2] * sins(rotation[1]) + pitchRotated[0] * coss(rotation[1]);
    dst[1] = from[1] + pitchRotated[1];
    dst[2] = from[2] + pitchRotated[2] * coss(rotation[1]) - pitchRotated[0] * sins(rotation[1]);
}

__attribute__((always_inline))  inline  void determine_pushing_or_pulling_door(s16 *rotation) {
    if (gMarioState->action == ACT_PULLING_DOOR) {
        *rotation = 0;
    } else {
        *rotation = DEGREES(180);
    }
}

/**
 * Calculate Lakitu's next position and focus, according to gCamera's state,
 * and store them in `newPos` and `newFoc`.
 *
 * @param newPos where Lakitu should fly towards this frame
 * @param newFoc where Lakitu should look towards this frame
 *
 * @param curPos gCamera's pos this frame
 * @param curFoc gCamera's foc this frame
 *
 * @param oldPos gCamera's pos last frame
 * @param oldFoc gCamera's foc last frame
 *
 * @return Lakitu's next yaw, which is the same as the yaw passed in if no transition happened
 */
__attribute__((always_inline))  inline  s32 next_lakitu_state(Vec3f newPos, Vec3f newFoc, Vec3f curPos,
                                                     Vec3f curFoc, Vec3f oldPos, Vec3f oldFoc,
                                                     s32 yaw) {
    s32 yawVelocity;
    s32 pitchVelocity;
    f32 distVelocity;
    f32 goalDist;
    s32 goalPitch;
    s32 goalYaw;
    f32 distTimer = sModeTransition.framesLeft;
    s32 angleTimer = sModeTransition.framesLeft;
    Vec3f nextPos;
    Vec3f nextFoc;
    s32 i;
    f32 floorHeight;
    struct Surface *floor;

    // If not transitioning, just use gCamera's current pos and foc
    vec3f_copy(newPos, curPos);
    vec3f_copy(newFoc, curFoc);

    // Transition from the last mode to the current one
    if (sModeTransition.framesLeft > 0) {
        vec3f_get_dist_and_angle(curFoc, curPos, &goalDist, &goalPitch, &goalYaw);
        distVelocity = ABS(goalDist - sModeTransition.posDist) / distTimer;
        pitchVelocity = ABS(goalPitch - sModeTransition.posPitch) / angleTimer;
        yawVelocity = ABS(goalYaw - sModeTransition.posYaw) / angleTimer;

        sModeTransition.posDist =
            approach_f32_symmetric(sModeTransition.posDist, goalDist, distVelocity);
        sModeTransition.posYaw = approach_s16_symmetric(sModeTransition.posYaw, goalYaw, yawVelocity);
        sModeTransition.posPitch =
            approach_s16_symmetric(sModeTransition.posPitch, goalPitch, pitchVelocity);
        vec3f_set_dist_and_angle(curFoc, nextPos, sModeTransition.posDist, sModeTransition.posPitch,
                                 sModeTransition.posYaw);

        vec3f_get_dist_and_angle(curPos, curFoc, &goalDist, &goalPitch, &goalYaw);
        pitchVelocity = sModeTransition.focPitch / sModeTransition.framesLeft;
        yawVelocity = sModeTransition.focYaw / sModeTransition.framesLeft;
        distVelocity = sModeTransition.focDist / sModeTransition.framesLeft;

        sModeTransition.focPitch =
            approach_s16_symmetric(sModeTransition.focPitch, goalPitch, pitchVelocity);
        sModeTransition.focYaw = approach_s16_symmetric(sModeTransition.focYaw, goalYaw, yawVelocity);
        sModeTransition.focDist = approach_f32_symmetric(sModeTransition.focDist, 0, distVelocity);
        vec3f_set_dist_and_angle(curFoc, nextFoc, sModeTransition.focDist, sModeTransition.focPitch,
                                 sModeTransition.focYaw);

        vec3f_copy(newFoc, nextFoc);
        vec3f_copy(newPos, nextPos);

        if (gCamera->cutscene != 0 || !(gCameraMovementFlags & CAM_MOVE_C_UP_MODE)) {
            floorHeight = find_floor(newPos[0], newPos[1], newPos[2], &floor);
            if (floorHeight != -11000.f) {
                if ((floorHeight += 125.f) > newPos[1]) {
                    newPos[1] = floorHeight;
                }
            }
            f32_find_wall_collision(&newPos[0], &newPos[1], &newPos[2], 0.f, 100.f);
        }
        sModeTransition.framesLeft--;
        yaw = calculate_yaw(newFoc, newPos);
    } else {
        sModeTransition.posDist = 0.f;
        sModeTransition.posPitch = 0;
        sModeTransition.posYaw = 0;
        sStatusFlags &= ~CAM_FLAG_TRANSITION_OUT_OF_C_UP;
    }
    vec3f_copy(sModeTransition.marioPos, gMarioState->pos);
    return yaw;
}

/**
 * Terminates a list of CameraTriggers.
 */
#define NULL_TRIGGER                                                                                   \
    { 0, NULL, 0, 0, 0, 0, 0, 0, 0 }

#define _ NULL
#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, cameratable) 0,
#define DEFINE_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, cameratable) 0,

struct CameraTrigger sCamBitDW[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamBOB[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamWF[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamPSS[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamCastleGrounds[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamBBH[] = {
	{-1, 5, 7996, -3853, -3414, 14667, 1000, 2303, 0xffff},
	{-1, 1, 6049, -308, 1356, 3000, 3255, 2303, 0xffff},
	{-1, 3, -4586, 2368, 356, 3000, 3255, 3073, 0xffff},
	{-1, 6, -9535, 2368, -1649, 740, 3255, 3073, 0xffff},
	{-1, 4, -4700, -3703, 4154, 6332, 3255, 2303, 0xffff},
	{-1, 4, -10246, -3618, 1584, 1202, 2080, 3216, 0xffff},
	{-1, 2, -8198, 1484, 325, 688, 3255, 2326, 0xffff},
	{-1, 2, -3059, -3439, 368, 6217, 3255, 1573, 0xffff},
	{-1, 7, -10925, 2368, -1649, 740, 3255, 3073, 0xffff},
	{-1, 8, -12261, 2368, -1649, 740, 3255, 3073, 0xffff},
	{-1, 9, -13622, 2368, -1649, 777, 3255, 3073, 0xffff},
	NULL_TRIGGER
};
struct CameraTrigger sCamJRB[] = { NULL_TRIGGER };
struct CameraTrigger sCamCCM[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamHMC[] = {
	NULL_TRIGGER
};
struct CameraTrigger sCamEnding[] = { NULL_TRIGGER };
struct CameraTrigger sCamVCUtM[] = { NULL_TRIGGER };
struct CameraTrigger *sCameraTriggers[LEVEL_COUNT + 1] = {
    NULL,
#include "levels/level_defines.h"
};
#undef _
#undef STUB_LEVEL
#undef DEFINE_LEVEL

/**
 * Start a cutscene focusing on an object
 * This will play if nothing else happened in the same frame, like exiting or warping.
 */
extern s16 newcam_yaw;
extern void adjustTimers(s32 time);
void start_object_cutscene(s32 cutscene, struct Object *o) {
    sObjectCutscene = cutscene;
    gRecentCutscene = 0;
    gCutsceneFocus = o;
    gObjCutsceneDone = FALSE;
    if (o->behavior != segmented_to_virtual(bhvSpawnedStarNoLevelExit)) {
        newcam_yaw = obj_angle_to_object(o, gMarioObject); // newcam puppycam
    }
    newcam_yaw_target = newcam_yaw;
    adjustTimers(0);
}

/**
 * Start a low-priority cutscene without focusing on an object
 * This will play if nothing else happened in the same frame, like exiting or warping.
 */
__attribute__((always_inline))  inline  s32 start_object_cutscene_without_focus(s32 cutscene) {
    sObjectCutscene = cutscene;
    sCutsceneDialogResponse = 0;
    return 0;
}

s32 cutscene_object_with_dialog(s32 cutscene, struct Object *o, s32 dialogID) {

    if ((gCamera->cutscene == 0) && (sObjectCutscene == 0)) {
        if (gRecentCutscene != cutscene) {
            start_object_cutscene(cutscene, o);
            if (dialogID != -1) {
                sCutsceneDialogID = dialogID;
            } else {
                sCutsceneDialogID = DIALOG_001;
            }
        } else {
            return sCutsceneDialogResponse;
        }

        gRecentCutscene = 0;
    }
    return 0;
}

/**
 * @return 0 if not started, 1 if started, and -1 if finished
 */
s32 cutscene_object(s32 cutscene, struct Object *o) {

    if ((gCamera->cutscene == 0) && (sObjectCutscene == 0)) {
        if (gRecentCutscene != cutscene) {
            start_object_cutscene(cutscene, o);
            return 1;
        } else {
            return -1;
        }
    }
    return 0;
}

/**
 * Update the camera's yaw and nextYaw. This is called from cutscenes to ignore the camera mode's yaw.
 */
void update_camera_yaw(struct Camera *c) {
    c->nextYaw = calculate_yaw(c->focus, c->pos);
    c->yaw = c->nextYaw;
    newcam_yaw = c->nextYaw;
    newcam_yaw_target = newcam_yaw;
    newcam_centering;
}

void cutscene_reset_spline(void) {
    sCutsceneSplineSegment = 0;
    sCutsceneSplineSegmentProgress = 0;
}

void init_spline_point(struct CutsceneSplinePoint *splinePoint, s32 index, s32 speed, Vec3s point) {
    splinePoint->index = index;
    splinePoint->speed = speed;
    vec3s_copy(splinePoint->point, point);
}

void copy_spline_segment(struct CutsceneSplinePoint dst[], struct CutsceneSplinePoint src[]) {
    s32 j = 0;
    s32 i = 0;

    init_spline_point(&dst[i], src[j].index, src[j].speed, src[j].point);
    i += 1;
    do {
        do {
            init_spline_point(&dst[i], src[j].index, src[j].speed, src[j].point);
            i += 1;
            j += 1;
        } while ((src[j].index != -1) && (src[j].index != -1)); //! same comparison performed twice
    } while (j > 16);

    // Create the end of the spline by duplicating the last point
    do {
        init_spline_point(&dst[i], 0, src[j].speed, src[j].point);
    } while (0);
    do {
        init_spline_point(&dst[i + 1], 0, 0, src[j].point);
    } while (0);
    do {
        init_spline_point(&dst[i + 2], 0, 0, src[j].point);
    } while (0);
    do {
        init_spline_point(&dst[i + 3], -1, 0, src[j].point);
    } while (0);
}

/**
 * Triggers Mario to enter a dialog state. This is used to make Mario look at the focus of a cutscene,
 * for example, bowser.
 * @param state 0 = stop, 1 = start, 2 = start and look up, and 3 = start and look down
 *
 * @return if Mario left the dialog state, return CUTSCENE_LOOP, else return gCutsceneTimer
 */
s32 cutscene_common_set_dialog_state(s32 state) {
    if (set_mario_npc_dialog(state) == 2) {
        return CUTSCENE_LOOP;
    }
    return gCutsceneTimer;
}

/**
 * Cause Mario to enter the normal dialog state.
 */
BAD_RETURN(s32) cutscene_mario_dialog(UNUSED struct Camera *c) {
    gCutsceneTimer = cutscene_common_set_dialog_state(1);
}

void store_info_star(struct Camera *c) {
    vec3f_copy(sCameraStoreCutscene.pos, c->pos);
    sCameraStoreCutscene.focus[0] = gMarioState->pos[0];
    sCameraStoreCutscene.focus[1] = c->focus[1];
    sCameraStoreCutscene.focus[2] = gMarioState->pos[2];
}

void retrieve_info_star(struct Camera *c) {
    vec3f_copy(c->pos, sCameraStoreCutscene.pos);
    vec3f_copy(c->focus, sCameraStoreCutscene.focus);
}

void pan_camera(struct Camera *c, s32 incPitch, s32 incYaw) {
    f32 distCamToFocus;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(c->pos, c->focus, &distCamToFocus, &pitch, &yaw);
    pitch += incPitch;
    yaw += incYaw;
    vec3f_set_dist_and_angle(c->pos, c->focus, distCamToFocus, pitch, yaw);
}

BAD_RETURN(s32) cutscene_shake_explosion(UNUSED struct Camera *c) {
    set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
    cutscene_set_fov_shake_preset(1);
}

/**
 * Change the spherical coordinates of `to` relative to `from` by `incDist`, `incPitch`, and `incYaw`
 *
 * @param from    the base position
 * @param[out] to the destination position
 */
void rotate_and_move_vec3f(Vec3f to, Vec3f from, f32 incDist, s32 incPitch, s32 incYaw) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(from, to, &dist, &pitch, &yaw);
    pitch += incPitch;
    yaw += incYaw;
    dist += incDist;
    vec3f_set_dist_and_angle(from, to, dist, pitch, yaw);
}

void set_flag_post_door(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_BEHIND_MARIO_POST_DOOR;
}

void cutscene_soften_music(UNUSED struct Camera *c) {
    func_8031FFB4(SEQ_PLAYER_LEVEL, 60, 40);
}

void cutscene_unsoften_music(UNUSED struct Camera *c) {
    sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
}

/**
 * Adjust the camera focus towards a point `dist` units in front of Mario.
 * @param dist distance in Mario's forward direction. Note that this is relative to Mario, so a negative
 *        distance will focus in front of Mario, and a positive distance will focus behind him.
 */
void focus_in_front_of_mario(struct Camera *c, f32 dist, f32 speed) {
    Vec3f goalFocus, offset;

    offset[0] = 0.f;
    offset[2] = dist;
    offset[1] = 100.f;

    offset_rotated(goalFocus, gMarioState->pos, offset, gMarioState->faceAngle);
    approach_vec3f_asymptotic(c->focus, goalFocus, speed, speed, speed);
}

/**
 * Approach Mario and look up. Since Mario faces the camera when he collects the star, there's no need
 * to worry about the camera's yaw.
 */
BAD_RETURN(s32) cutscene_dance_move_to_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    dist = approach_f32_asymptotic(dist, 600.f, 0.3f);
    pitch = approach_s16_asymptotic(pitch, 0x1000, 0x10);
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

BAD_RETURN(s32) cutscene_dance_rotate(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, gMarioState->pos, 0, 0, 0x200);
}

BAD_RETURN(s32) cutscene_dance_rotate_move_back(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, gMarioState->pos, -15.f, 0, 0);
}

BAD_RETURN(s32) cutscene_dance_rotate_move_towards_mario(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, gMarioState->pos, 20.f, 0, 0);
}

/**
 * Slowly turn to the point 100 units in front of Mario
 */
BAD_RETURN(s32) cutscene_dance_default_focus_mario(struct Camera *c) {
    focus_in_front_of_mario(c, -100.f, 0.2f);
}

BAD_RETURN(s32) cutscene_dance_rotate_focus_mario(struct Camera *c) {
    focus_in_front_of_mario(c, -200.f, 0.03f);
}

BAD_RETURN(s32) cutscene_dance_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x200, 0x28, 0x8000);
}
BAD_RETURN(s32) cutscene_dance_default_rotate(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_fov_function(CAM_FOV_DEFAULT);
    cutscene_event(cutscene_dance_default_focus_mario, c, 0, 20);
    cutscene_event(cutscene_dance_move_to_mario, c, 0, 39);
    // Shake the camera on the 4th beat of the music, when Mario gives the peace sign.
    cutscene_event(cutscene_dance_shake_fov, c, 40, 40);

    if (c->cutscene != CUTSCENE_DANCE_DEFAULT) { // CUTSCENE_DANCE_ROTATE
        cutscene_event(cutscene_dance_rotate_focus_mario, c, 75, 102);
        cutscene_event(cutscene_dance_rotate, c, 50, -1);
        // These two functions move the camera away and then towards Mario.
        cutscene_event(cutscene_dance_rotate_move_back, c, 50, 80);
        cutscene_event(cutscene_dance_rotate_move_towards_mario, c, 70, 90);
    } else {
        // secret star, 100 coin star, or bowser red coin star.
        if ((gMarioState->action != ACT_STAR_DANCE_NO_EXIT)
            && (gMarioState->action != ACT_STAR_DANCE_WATER)
            && (gMarioState->action != ACT_STAR_DANCE_EXIT)) {
            gCutsceneTimer = CUTSCENE_STOP;
            c->cutscene = 0;
            sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
        }
    }
}

/**
 * If the camera's yaw is out of the range of `absYaw` +- `yawMax`, then set the yaw to `absYaw`
 */
BAD_RETURN(s32) star_dance_bound_yaw(struct Camera *c, s32 absYaw, s32 yawMax) {
    s16 dummyPitch, yaw;
    f32 distCamToMario;
    s32 yawFromAbs;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &distCamToMario, &dummyPitch, &yaw);
    yawFromAbs = yaw - absYaw;

    // Because angles are s16, this checks if yaw is negative
    if ((yawFromAbs & 0x8000) != 0) {
        yawFromAbs = -yawFromAbs;
    }
    if (yawFromAbs > yawMax) {
        yaw = absYaw;
        c->nextYaw = yaw;
        c->yaw = yaw;
    }
}

/**
 * Start the closeup dance cutscene by restricting the camera's yaw in certain areas.
 * Store the camera's focus in cvar9.
 */
BAD_RETURN(s32) cutscene_dance_closeup_start(struct Camera *c) {
    UNUSED s32 pad[2];

    if ((gLastCompletedStarNum == 4) && (gCurrCourseNum == COURSE_JRB)) {
        star_dance_bound_yaw(c, 0x0, 0x4000);
    }
    if ((gLastCompletedStarNum == 1) && (gCurrCourseNum == COURSE_DDD)) {
        star_dance_bound_yaw(c, 0x8000, 0x5000);
    }
    if ((gLastCompletedStarNum == 5) && (gCurrCourseNum == COURSE_WDW)) {
        star_dance_bound_yaw(c, 0x8000, 0x800);
    }

    vec3f_copy(sCutsceneVars[9].point, c->focus);
    //! cvar8 is unused in the closeup cutscene
    sCutsceneVars[8].angle[0] = 0x2000;
}

/**
 * Focus the camera on Mario eye height.
 */
BAD_RETURN(s32) cutscene_dance_closeup_focus_mario(struct Camera *c) {
    Vec3f marioPos;

    vec3f_set(marioPos, gMarioState->pos[0], gMarioState->pos[1] + 125.f, gMarioState->pos[2]);
    approach_vec3f_asymptotic(sCutsceneVars[9].point, marioPos, 0.2f, 0.2f, 0.2f);
    vec3f_copy(c->focus, sCutsceneVars[9].point);
}

/**
 * Fly above Mario, looking down.
 */
BAD_RETURN(s32) cutscene_dance_closeup_fly_above(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;
    s32 goalPitch = 0x1800;

    if ((gLastCompletedStarNum == 6 && gCurrCourseNum == COURSE_SL)
        || (gLastCompletedStarNum == 4 && gCurrCourseNum == COURSE_TTC)) {
        goalPitch = 0x800;
    }

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    dist = approach_f32_asymptotic(dist, 800.f, 0.05f);
    pitch = approach_s16_asymptotic(pitch, goalPitch, 16);
    yaw = approach_s16_asymptotic(yaw, c->yaw, 8);
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Fly closer right when Mario gives the peace sign.
 */
BAD_RETURN(s32) cutscene_dance_closeup_fly_closer(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    dist = approach_f32_asymptotic(dist, 240.f, 0.4f);
    yaw = approach_s16_asymptotic(yaw, c->yaw, 8);
    pitch = approach_s16_asymptotic(pitch, 0x1000, 5);
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Zoom in by increasing fov to 80 degrees. Most dramatic zoom in the game.
 */
BAD_RETURN(s32) cutscene_dance_closeup_zoom(UNUSED struct Camera *c) {
    set_fov_function(CAM_FOV_APP_80);
}

/**
 * Shake fov, starts on the first frame Mario has the peace sign up.
 */
BAD_RETURN(s32) cutscene_dance_closeup_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x300, 0x30, 0x8000);
}

/**
 * The camera moves in for a closeup on Mario. Used for stars that are underwater or in tight places.
 */
BAD_RETURN(s32) cutscene_dance_closeup(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (gMarioState->action == ACT_STAR_DANCE_WATER) {
        cutscene_event(cutscene_dance_closeup_start, c, 0, 0);
        cutscene_event(cutscene_dance_closeup_focus_mario, c, 0, -1);
        cutscene_event(cutscene_dance_closeup_fly_above, c, 0, 62);
        cutscene_event(cutscene_dance_closeup_fly_closer, c, 63, -1);
        cutscene_event(cutscene_dance_closeup_zoom, c, 63, 63);
        cutscene_event(cutscene_dance_closeup_shake_fov, c, 70, 70);
    } else {
        cutscene_event(cutscene_dance_closeup_start, c, 0, 0);
        cutscene_event(cutscene_dance_closeup_focus_mario, c, 0, -1);
        // Almost twice as fast as under water
        cutscene_event(cutscene_dance_closeup_fly_above, c, 0, 32);
        cutscene_event(cutscene_dance_closeup_fly_closer, c, 33, -1);
        cutscene_event(cutscene_dance_closeup_zoom, c, 33, 33);
        cutscene_event(cutscene_dance_closeup_shake_fov, c, 40, 40);
    }
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

/**
 * cvar8.point[2] is the amount to increase distance from Mario
 */
BAD_RETURN(s32) cutscene_dance_fly_away_start(struct Camera *c) {
    Vec3f areaCenter;

    vec3f_copy(sCutsceneVars[9].point, c->focus);
    sCutsceneVars[8].point[2] = 65.f;

    if (c->mode == CAMERA_MODE_RADIAL) {
        vec3f_set(areaCenter, c->areaCenX, c->areaCenY, c->areaCenZ);
        c->yaw = calculate_yaw(areaCenter, c->pos);
        c->nextYaw = c->yaw;
    }

    // Restrict the camera yaw in tight spaces
    if ((gLastCompletedStarNum == 6) && (gCurrCourseNum == COURSE_CCM)) {
        star_dance_bound_yaw(c, 0x5600, 0x800);
    }
    if ((gLastCompletedStarNum == 2) && (gCurrCourseNum == COURSE_TTM)) {
        star_dance_bound_yaw(c, 0x0, 0x800);
    }
    if ((gLastCompletedStarNum == 1) && (gCurrCourseNum == COURSE_SL)) {
        star_dance_bound_yaw(c, 0x2000, 0x800);
    }
    if ((gLastCompletedStarNum == 3) && (gCurrCourseNum == COURSE_RR)) {
        star_dance_bound_yaw(c, 0x0, 0x800);
    }
}

BAD_RETURN(s32) cutscene_dance_fly_away_approach_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    dist = approach_f32_asymptotic(dist, 600.f, 0.3f);
    pitch = approach_s16_asymptotic(pitch, 0x1000, 16);
    yaw = approach_s16_asymptotic(yaw, c->yaw, 8);
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

BAD_RETURN(s32) cutscene_dance_fly_away_focus_mario(struct Camera *c) {
    Vec3f marioPos;

    vec3f_set(marioPos, gMarioState->pos[0], gMarioState->pos[1] + 125.f, gMarioState->pos[2]);
    approach_vec3f_asymptotic(sCutsceneVars[9].point, marioPos, 0.2f, 0.2f, 0.2f);
    vec3f_copy(c->focus, sCutsceneVars[9].point);
}

/**
 * Slowly pan the camera downwards and to the camera's right, using cvar9's angle.
 */
void cutscene_pan_cvar9(struct Camera *c) {
    vec3f_copy(c->focus, sCutsceneVars[9].point);
    sCutsceneVars[9].angle[0] -= 29;
    sCutsceneVars[9].angle[1] += 29;
    pan_camera(c, sCutsceneVars[9].angle[0], sCutsceneVars[9].angle[1]);
}

/**
 * Move backwards and rotate slowly around Mario.
 */
BAD_RETURN(s32) cutscene_dance_fly_rotate_around_mario(struct Camera *c) {
    cutscene_pan_cvar9(c);
    rotate_and_move_vec3f(c->pos, gMarioState->pos, sCutsceneVars[8].point[2], 0, 0);
}

/**
 * Rotate quickly while Lakitu flies up.
 */
BAD_RETURN(s32) cutscene_dance_fly_away_rotate_while_flying(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, gMarioState->pos, 0, 0, 0x80);
}

BAD_RETURN(s32) cutscene_dance_fly_away_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x400, 0x30, 0x8000);
}

/**
 * After collecting the star, Lakitu flies upwards out of the course.
 */
BAD_RETURN(s32) cutscene_dance_fly_away(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_dance_fly_away_start, c, 0, 0);
    cutscene_event(cutscene_dance_fly_away_focus_mario, c, 0, 30);
    cutscene_event(cutscene_dance_fly_away_approach_mario, c, 0, 30);
    cutscene_event(cutscene_dance_fly_rotate_around_mario, c, 55, 124);
    cutscene_event(cutscene_dance_fly_away_rotate_while_flying, c, 55, 124);
    cutscene_event(cutscene_dance_fly_away_shake_fov, c, 40, 40);
    set_fov_function(CAM_FOV_DEFAULT);
    set_handheld_shake(HAND_CAM_SHAKE_STAR_DANCE);
}

/**
 * Jump the camera pos and focus to cvar 8 and 7.
 * Called every frame, starting after 10, so when these cvars are updated, the camera will jump.
 */
BAD_RETURN(s32) cutscene_key_dance_jump_cvar(struct Camera *c) {
    offset_rotated(c->pos, gMarioState->pos, sCutsceneVars[8].point, gMarioState->faceAngle);
    offset_rotated(c->focus, gMarioState->pos, sCutsceneVars[7].point, gMarioState->faceAngle);
}

/**
 * Jump to a closeup view of Mario and the key.
 */
BAD_RETURN(s32) cutscene_key_dance_jump_closeup(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[8].point, 38.f, 171.f, -248.f);
    vec3f_set(sCutsceneVars[7].point, -57.f, 51.f, 187.f);
}

/**
 * Jump to a view from the lower left (Mario's right).
 */
BAD_RETURN(s32) cutscene_key_dance_jump_lower_left(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[8].point, -178.f, 62.f, -132.f);
    vec3f_set(sCutsceneVars[7].point, 299.f, 91.f, 58.f);
}

/**
 * Jump to a rotated view from above.
 */
BAD_RETURN(s32) cutscene_key_dance_jump_above(UNUSED struct Camera *c) {
    gLakituState.keyDanceRoll = 0x2800;
    vec3f_set(sCutsceneVars[8].point, 89.f, 373.f, -304.f);
    vec3f_set(sCutsceneVars[7].point, 0.f, 127.f, 0.f);
}

/**
 * Finally, jump to a further view, slightly to Mario's left.
 */
BAD_RETURN(s32) cutscene_key_dance_jump_last(UNUSED struct Camera *c) {
    gLakituState.keyDanceRoll = 0;
    vec3f_set(sCutsceneVars[8].point, 135.f, 158.f, -673.f);
    vec3f_set(sCutsceneVars[7].point, -20.f, 135.f, -198.f);
}

BAD_RETURN(s32) cutscene_key_dance_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x180, 0x30, 0x8000);
}

BAD_RETURN(s32) cutscene_key_dance_handheld_shake(UNUSED struct Camera *c) {
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

BAD_RETURN(s32) cutscene_key_dance_focus_mario(struct Camera *c) {
    focus_in_front_of_mario(c, 0, 0.2f);
}

/**
 * Cutscene that plays when Mario collects a key from bowser. It's basically a sequence of four jump
 * cuts.
 */
BAD_RETURN(s32) cutscene_key_dance(struct Camera *c) {
    cutscene_event(cutscene_dance_move_to_mario, c, 0, 10);
    cutscene_event(cutscene_key_dance_focus_mario, c, 0, 10);
    cutscene_event(cutscene_key_dance_jump_closeup, c, 0, 0);
    cutscene_event(cutscene_key_dance_jump_lower_left, c, 20, 20);
    cutscene_event(cutscene_key_dance_jump_above, c, 35, 35);
    cutscene_event(cutscene_key_dance_jump_last, c, 52, 52);
    cutscene_event(cutscene_key_dance_jump_cvar, c, 11, -1);
    cutscene_event(cutscene_key_dance_shake_fov, c, 54, 54);
    cutscene_event(cutscene_key_dance_handheld_shake, c, 52, -1);
}

void cutscene_stop_dialog(UNUSED struct Camera *c) {
    cutscene_common_set_dialog_state(0);
}

BAD_RETURN(s32) cutscene_star_spawn_store_info(struct Camera *c) {
    store_info_star(c);
}

/**
 * Focus on the top of the star.
 */
BAD_RETURN(s32) cutscene_star_spawn_focus_star(struct Camera *c) {
    UNUSED f32 hMul;
    Vec3f starPos;
    UNUSED f32 vMul;

    if (gCutsceneFocus != NULL) {
        object_pos_to_vec3f(starPos, gCutsceneFocus);
        starPos[1] += gCutsceneFocus->hitboxHeight;
        approach_vec3f_asymptotic(c->focus, starPos, 0.1f, 0.1f, 0.1f);
    }
}
/**
 * Fly back to the camera's previous pos and focus.
 */
BAD_RETURN(s32) cutscene_star_spawn_fly_back(struct Camera *c) {
    retrieve_info_star(c);
}

/**
 * Plays when a star spawns (ie from a box).
 */
BAD_RETURN(s32) cutscene_star_spawn(struct Camera *c) {
    cutscene_event(cutscene_star_spawn_store_info, c, 0, 0);
    cutscene_event(cutscene_star_spawn_focus_star, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (gObjCutsceneDone) {
        // Set the timer to CUTSCENE_LOOP, which start the next shot.
        gCutsceneTimer = CUTSCENE_LOOP;
    }
}

/**
 * Move the camera back to Mario.
 */
BAD_RETURN(s32) cutscene_star_spawn_back(struct Camera *c) {
    cutscene_event(cutscene_star_spawn_fly_back, c, 0, 0);

    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

BAD_RETURN(s32) cutscene_star_spawn_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
}

/**
 * Start the red coin star spawning cutscene.
 */
BAD_RETURN(s32) cutscene_red_coin_star_start(struct Camera *c) {
    object_pos_to_vec3f(sCutsceneVars[1].point, gCutsceneFocus);
    store_info_star(c);
    // Store the default fov for after the cutscene
    sCutsceneVars[2].point[2] = sFOVState.fov;
}

/**
 * Look towards the star's x and z position
 */
BAD_RETURN(s32) cutscene_red_coin_star_focus_xz(struct Camera *c) {
    c->focus[0] = approach_f32_asymptotic(c->focus[0], gCutsceneFocus->oPosX, 0.15f);
    c->focus[2] = approach_f32_asymptotic(c->focus[2], gCutsceneFocus->oPosZ, 0.15f);
}

/**
 * Look towards the star's y position. Only active before the camera warp.
 */
BAD_RETURN(s32) cutscene_red_coin_star_focus_y(struct Camera *c) {
    c->focus[1] = approach_f32_asymptotic(c->focus[1], gCutsceneFocus->oPosY, 0.1f);
}

/**
 * Look 80% up towards the star. Only active after the camera warp.
 */
BAD_RETURN(s32) cutscene_red_coin_star_look_up_at_star(struct Camera *c) {
    c->focus[1] =
        sCutsceneVars[1].point[1] + (gCutsceneFocus->oPosY - sCutsceneVars[1].point[1]) * 0.8f;
}

/**
 * Warp the camera near the star's spawn point
 */
BAD_RETURN(s32) cutscene_red_coin_star_warp(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw, posYaw;
    struct Object *o = gCutsceneFocus;

    vec3f_set(sCutsceneVars[1].point, o->oHomeX, o->oHomeY, o->oHomeZ);
    vec3f_get_dist_and_angle(sCutsceneVars[1].point, c->pos, &dist, &pitch, &yaw);
    posYaw = calculate_yaw(sCutsceneVars[1].point, c->pos);
    yaw = calculate_yaw(sCutsceneVars[1].point, gMarioState->pos);

    if (ABS(yaw - posYaw + DEGREES(90)) < ABS(yaw - posYaw - DEGREES(90))) {
        yaw += DEGREES(90);
    } else {
        yaw -= DEGREES(90);
    }

    vec3f_set_dist_and_angle(sCutsceneVars[1].point, c->pos, 400.f, 0x1000, yaw);
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
}

/**
 * Zoom out while looking at the star.
 */
BAD_RETURN(s32) cutscene_red_coin_star_set_fov(UNUSED struct Camera *c) {
    sFOVState.fov = 60.f;
}

BAD_RETURN(s32) cutscene_red_coin_star(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_red_coin_star_start, c, 0, 0);
    cutscene_event(cutscene_red_coin_star_warp, c, 30, 30);
    cutscene_event(cutscene_red_coin_star_focus_xz, c, 0, -1);
    cutscene_event(cutscene_red_coin_star_focus_y, c, 0, 29);
    cutscene_event(cutscene_red_coin_star_look_up_at_star, c, 30, -1);
    cutscene_event(cutscene_red_coin_star_set_fov, c, 30, -1);

    if (gObjCutsceneDone) {
        // Set the timer to CUTSCENE_LOOP, which start the next shot.
        gCutsceneTimer = CUTSCENE_LOOP;
    }
}

/**
 * End the red coin star spawning cutscene
 */
BAD_RETURN(s32) cutscene_red_coin_star_end(struct Camera *c) {
    retrieve_info_star(c);
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    // Restore the default fov
    sFOVState.fov = sCutsceneVars[2].point[2];
}

/**
 * Moves the camera towards the cutscene's focus, stored in sCutsceneVars[3].point
 *
 * sCutsceneVars[3].point is used as the target point
 * sCutsceneVars[0].point is used as the current camera focus during the transition
 *
 * @param rotPitch constant pitch offset to add to the camera's focus
 * @param rotYaw constant yaw offset to add to the camera's focus
 */
void cutscene_goto_cvar_pos(struct Camera *c, f32 goalDist, s32 goalPitch, s32 rotPitch, s32 rotYaw) {
    f32 nextDist;
    s16 nextPitch, nextYaw;
    // The next 2 polar coord points are only used in CUTSCENE_PREPARE_CANNON
    f32 cannonDist;
    s16 cannonPitch, cannonYaw;
    f32 curDist;
    s32 curPitch, curYaw;
    vec3f_get_dist_and_angle(sCutsceneVars[3].point, c->pos, &nextDist, &nextPitch, &nextYaw);
    // If over 8000 units away from the cannon, just teleport there
    if ((nextDist > 8000.f) && (c->cutscene == CUTSCENE_PREPARE_CANNON)) {
        nextDist = 1000.f;
        nextPitch = 0x1000;
        vec3f_copy(sCutsceneVars[0].point, sCutsceneVars[3].point);
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;

        if (gCurrLevelNum == LEVEL_CCM) {
            nextYaw = 0x8000;
        }
    } else {
        if (c->cutscene == CUTSCENE_PREPARE_CANNON) {
            vec3f_get_dist_and_angle(c->pos, sCutsceneVars[0].point, &curDist, &curPitch, &curYaw);
            vec3f_get_dist_and_angle(c->pos, sCutsceneVars[3].point, &cannonDist, &cannonPitch,
                                     &cannonYaw);
            curDist = approach_f32_asymptotic(curDist, cannonDist, 0.1f);
            curPitch = approach_s16_asymptotic(curPitch, cannonPitch, 15);
            curYaw = approach_s16_asymptotic(curYaw, cannonYaw, 15);
            // Move the current focus, sCutsceneVars[0].point, in the direction towards the cannon
            vec3f_set_dist_and_angle(c->pos, sCutsceneVars[0].point, curDist, curPitch, curYaw);
        } else {
            approach_vec3f_asymptotic(sCutsceneVars[0].point, sCutsceneVars[3].point, 0.1f, 0.1f, 0.1f);
        }
    }

    nextDist = approach_f32_asymptotic(nextDist, goalDist, 0.05f);
    nextPitch = approach_s16_asymptotic(nextPitch, goalPitch, 0x20);

    vec3f_set_dist_and_angle(sCutsceneVars[3].point, c->pos, nextDist, nextPitch, nextYaw);
    vec3f_copy(c->focus, sCutsceneVars[0].point);

    // Apply the constant rotation given
    pan_camera(c, rotPitch, rotYaw);
    vec3f_get_dist_and_angle(c->pos, c->focus, &nextDist, &nextPitch, &nextYaw);

    if (nextPitch < -0x3000) {
        nextPitch = -0x3000;
    }
    if (nextPitch > 0x3000) {
        nextPitch = 0x3000;
    }

    vec3f_set_dist_and_angle(c->pos, c->focus, nextDist, nextPitch, nextYaw);
}

/**
 * Store the camera's pos and focus, and copy the cannon's position to cvars.
 */
BAD_RETURN(s32) cutscene_prepare_cannon_start(struct Camera *c) {
    vec3f_copy(sCameraStoreCutscene.pos, c->pos);
    vec3f_copy(sCameraStoreCutscene.focus, c->focus);
    sCameraStoreCutscene.cannonYOffset = sCannonYOffset;
    vec3f_copy(sCutsceneVars[0].point, c->focus);
    sCutsceneVars[2].point[0] = 30.f;
    // Store the cannon door's position in sCutsceneVars[3]'s point
    object_pos_to_vec3f(sCutsceneVars[3].point, gCutsceneFocus);
    vec3s_set(sCutsceneVars[5].angle, 0, 0, 0);
}

/**
 * Fly towards the cannon door.
 */
BAD_RETURN(s32) cutscene_prepare_cannon_fly_to_cannon(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 1200.f, 0x0800, 0, sCutsceneVars[5].angle[1]);
    sCutsceneVars[5].angle[1] = approach_s16_symmetric(sCutsceneVars[5].angle[1], 0x400, 17);
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

/**
 * Used in the cannon opening cutscene to fly back to the camera's last position and focus
 */
void cannon_approach_prev(f32 *value, f32 target) {
    f32 inc = ABS(target - *value) / sCutsceneVars[2].point[0];
    *value = approach_f32_symmetric(*value, target, inc);
}

/**
 * Fly or warp back to the previous pos and focus, stored in sCameraStoreCutscene.
 */
BAD_RETURN(s32) cutscene_prepare_cannon_fly_back(struct Camera *c) {
    f32 distToPrevPos = calc_abs_dist(c->pos, sCameraStoreCutscene.pos);

    if (distToPrevPos < 8000.f) {
        cannon_approach_prev(&c->pos[0], sCameraStoreCutscene.pos[0]);
        cannon_approach_prev(&c->pos[1], sCameraStoreCutscene.pos[1]);
        cannon_approach_prev(&c->pos[2], sCameraStoreCutscene.pos[2]);
        cannon_approach_prev(&c->focus[0], sCameraStoreCutscene.focus[0]);
        cannon_approach_prev(&c->focus[1], sCameraStoreCutscene.focus[1]);
        cannon_approach_prev(&c->focus[2], sCameraStoreCutscene.focus[2]);
    } else {
        // If too far away, just warp back
        vec3f_copy(c->focus, sCameraStoreCutscene.focus);
        vec3f_copy(c->pos, sCameraStoreCutscene.pos);
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    }
    if (sCutsceneVars[2].point[0] > 1.f) {
        sCutsceneVars[2].point[0] -= 1.f;
    }
}

/**
 * Cutscene that plays when the cannon is opened.
 */
BAD_RETURN(s32) cutscene_prepare_cannon(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_prepare_cannon_start, c, 0, 0);
    cutscene_event(cutscene_prepare_cannon_fly_to_cannon, c, 0, 140);
    cutscene_event(cutscene_prepare_cannon_fly_back, c, 141, -1);
}

/**
 * Stop the cannon opening cutscene.
 */
BAD_RETURN(s32) cutscene_prepare_cannon_end(struct Camera *c) {
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    vec3f_copy(c->pos, sCameraStoreCutscene.pos);
    vec3f_copy(c->focus, sCameraStoreCutscene.focus);
    sCannonYOffset = sCameraStoreCutscene.cannonYOffset;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

/**
 * Moves the camera to Mario's side when Mario starts ACT_WATER_DEATH
 * Note that ACT_WATER_DEATH only starts when Mario gets hit by an enemy under water. It does not start
 * when Mario drowns.
 */
__attribute__((always_inline))  inline  void water_death_move_to_mario_side(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    yaw = approach_s16_asymptotic(yaw, (gMarioState->faceAngle[1] - 0x3000), 8);
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Unnecessary, only used in cutscene_death_standing_goto_mario()
 */
void death_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x1000, 0x300, 0);
}

BAD_RETURN(s32) cutscene_death_standing_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[0].point, c->focus);
    vec3f_copy(sCutsceneVars[3].point, gMarioState->pos);
    sCutsceneVars[3].point[1] += 70.f;
}

/**
 * Fly to Mario and turn on handheld shake.
 */
BAD_RETURN(s32) cutscene_death_standing_goto_mario(struct Camera *c) {
    death_goto_mario(c);
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

/**
 * Cutscene that plays when Mario dies while standing.
 */
BAD_RETURN(s32) cutscene_death_standing(struct Camera *c) {
    cutscene_event(cutscene_death_standing_start, c, 0, 0);
    cutscene_event(cutscene_death_standing_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

BAD_RETURN(s32) cutscene_death_stomach_start(struct Camera *c) {
    Vec3f offset = { 0, 40.f, -60.f };

    offset_rotated(sCutsceneVars[3].point, gMarioState->pos, offset, gMarioState->faceAngle);
    vec3f_copy(sCutsceneVars[0].point, c->focus);
}

BAD_RETURN(s32) cutscene_death_stomach_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x1800, 0, -0x400);
}

/**
 * Cutscene that plays when Mario dies on his stomach.
 */
BAD_RETURN(s32) cutscene_death_stomach(struct Camera *c) {
    cutscene_event(cutscene_death_stomach_start, c, 0, 0);
    cutscene_event(cutscene_death_stomach_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

BAD_RETURN(s32) cutscene_bbh_death_start(struct Camera *c) {
    Vec3f dir = { 0, 40.f, 60.f };

    offset_rotated(sCutsceneVars[3].point, gMarioState->pos, dir, gMarioState->faceAngle);
    vec3f_copy(sCutsceneVars[0].point, c->focus);
}

BAD_RETURN(s32) cutscene_bbh_death_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x1800, 0, 0x400);
}

/**
 * Cutscene that plays when Mario dies in BBH.
 */
BAD_RETURN(s32) cutscene_bbh_death(struct Camera *c) {
    cutscene_event(cutscene_bbh_death_start, c, 0, 0);
    cutscene_event(cutscene_bbh_death_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

/**
 * Copy the camera's focus to cvar0
 */
BAD_RETURN(s32) cutscene_quicksand_death_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[0].point, c->focus);
}

/**
 * Fly closer to Mario. In WATER_DEATH, move to Mario's side.
 */
BAD_RETURN(s32) cutscene_quicksand_death_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x2800, 0x200, 0);

    if (c->cutscene == CUTSCENE_WATER_DEATH) {
        water_death_move_to_mario_side(c);
    }
}

/**
 * Cutscene that plays when Mario dies in quicksand.
 */
BAD_RETURN(s32) cutscene_quicksand_death(struct Camera *c) {
    sCutsceneVars[3].point[0] = gMarioState->pos[0];
    sCutsceneVars[3].point[1] = gMarioState->pos[1] + 20.f;
    sCutsceneVars[3].point[2] = gMarioState->pos[2];

    cutscene_event(cutscene_quicksand_death_start, c, 0, 0);
    cutscene_event(cutscene_quicksand_death_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

/**
 * Fly away from Mario near the end of the cutscene.
 */
BAD_RETURN(s32) cutscene_suffocation_fly_away(UNUSED struct Camera *c) {
    Vec3f target;
    Vec3f offset = { 0, 20.f, 120.f };

    offset_rotated(target, gMarioState->pos, offset, gMarioState->faceAngle);
    approach_vec3f_asymptotic(sCutsceneVars[3].point, target, 0.1f, 0.1f, 0.1f);
}

/**
 * Keep Lakitu above the gas level.
 */
BAD_RETURN(s32) cutscene_suffocation_stay_above_gas(struct Camera *c) {
    UNUSED f32 unused1;
    f32 gasLevel;
    UNUSED f32 unused2;

    cutscene_goto_cvar_pos(c, 400.f, 0x2800, 0x200, 0);
    gasLevel = find_poison_gas_level(gMarioState->pos[0], gMarioState->pos[2]);

    if (gasLevel != -11000.f) {
        if ((gasLevel += 130.f) > c->pos[1]) {
            c->pos[1] = gasLevel;
        }
    }
}

/**
 * Quickly rotate around Mario.
 */
BAD_RETURN(s32) cutscene_suffocation_rotate(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    yaw += 0x100;
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Cutscene that plays when Mario dies from suffocation (ie due to HMC gas).
 */
BAD_RETURN(s32) cutscene_suffocation(struct Camera *c) {
    cutscene_event(cutscene_death_stomach_start, c, 0, 0);
    cutscene_event(cutscene_suffocation_rotate, c, 0, -1);
    cutscene_event(cutscene_suffocation_stay_above_gas, c, 0, -1);
    cutscene_event(cutscene_suffocation_fly_away, c, 50, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

/**
 * cvar8 is Mario's position and faceAngle
 *
 * cvar9.point is gCutsceneFocus's position
 * cvar9.angle[1] is the yaw between Mario and the gCutsceneFocus
 */
BAD_RETURN(s32) cutscene_dialog_start(struct Camera *c) {
    s32 yaw;
    s32 yosh = (gMarioState->usedObj->behavior == segmented_to_virtual(bhvStateYosh));
    // stateyosh
    if (!yosh) {

        cutscene_soften_music(c);
    }
    set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);

#ifndef VERSION_JP
    if (c->mode == CAMERA_MODE_BOSS_FIGHT) {
        vec3f_copy(sCameraStoreCutscene.focus, c->focus);
        vec3f_copy(sCameraStoreCutscene.pos, c->pos);
    } else {
#endif
        store_info_star(c);
#ifndef VERSION_JP
    }
#endif

    // Store Mario's position and faceAngle
    sCutsceneVars[8].angle[0] = 0;
    vec3f_copy(sCutsceneVars[8].point, gMarioState->pos);
    sCutsceneVars[8].point[1] += 125.f;

    // Store gCutsceneFocus's position and yaw
    object_pos_to_vec3f(sCutsceneVars[9].point, gCutsceneFocus);
    sCutsceneVars[9].point[1] += gCutsceneFocus->hitboxHeight + 200.f;
    sCutsceneVars[9].angle[1] = calculate_yaw(sCutsceneVars[8].point, sCutsceneVars[9].point);

    yaw = calculate_yaw(gMarioState->pos, gLakituState.curPos);
    if ((yaw - sCutsceneVars[9].angle[1]) & 0x8000) {
        sCutsceneVars[9].angle[1] -= 0x6000;
    } else {
        sCutsceneVars[9].angle[1] += 0x6000;
    }
}

/**
 * Move closer to Mario and the object, adjusting to their difference in height.
 * The camera's generally ends up looking over Mario's shoulder.
 */
BAD_RETURN(s32) cutscene_dialog_move_mario_shoulder(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;
    Vec3f focus, pos;
    s32 yosh = (gMarioState->usedObj->behavior == segmented_to_virtual(bhvStateYosh));
    scale_along_line(focus, sCutsceneVars[9].point, gMarioState->pos, 0.7f);
    vec3f_get_dist_and_angle(c->pos, focus, &dist, &pitch, &yaw);
    pitch = calculate_pitch(c->pos, sCutsceneVars[9].point);
    vec3f_set_dist_and_angle(c->pos, pos, dist, pitch, yaw);

    focus[1] = focus[1] + (sCutsceneVars[9].point[1] - focus[1]) * 0.1f;
    approach_vec3f_asymptotic(c->focus, focus, 0.2f, 0.2f, 0.2f);
    if (yosh) {
        c->pos[0] += (gMarioState->usedObj->oPosX - c->pos[0]) * 0.05f;
        c->pos[2] += (gMarioState->usedObj->oPosZ - c->pos[2]) * 0.05f;
        c->focus[0] = gMarioState->usedObj->oPosX;
        c->focus[2] = gMarioState->usedObj->oPosZ;
    }
    vec3f_copy(pos, c->pos);

    // Set y pos to cvar8's y (top of focus object)
    pos[1] = sCutsceneVars[8].point[1];
    vec3f_get_dist_and_angle(sCutsceneVars[8].point, pos, &dist, &pitch, &yaw);
    yaw = approach_s16_asymptotic(yaw, sCutsceneVars[9].angle[1], 0x10);
    dist = approach_f32_asymptotic(dist, 180.f, 0.05f);
    vec3f_set_dist_and_angle(sCutsceneVars[8].point, pos, dist, pitch, yaw);

    // Move up if Mario is below the focus object, down is Mario is above
    pos[1] = sCutsceneVars[8].point[1]
             + sins(calculate_pitch(sCutsceneVars[9].point, sCutsceneVars[8].point)) * 100.f;

    c->pos[1] = approach_f32_asymptotic(c->pos[1], pos[1], 0.05f);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
}

/**
 * Create the dialog with sCutsceneDialogID
 */
BAD_RETURN(s32) cutscene_dialog_create_dialog_box(struct Camera *c) {
    if (c->cutscene == CUTSCENE_RACE_DIALOG) {
        create_dialog_box_with_response(sCutsceneDialogID);
    } else {
        create_dialog_box(sCutsceneDialogID);
    }

    //! Unused. This may have been used before sCutsceneDialogResponse was implemented.
    sCutsceneVars[8].angle[0] = 3;
}

/**
 * Cutscene that plays when Mario talks to an object.
 */
BAD_RETURN(s32) cutscene_dialog(struct Camera *c) {
    cutscene_event(cutscene_dialog_start, c, 0, 0);
    cutscene_event(cutscene_dialog_move_mario_shoulder, c, 0, -1);
    cutscene_event(cutscene_dialog_create_dialog_box, c, 10, 10);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (gDialogResponse != 0) {
        sCutsceneDialogResponse = gDialogResponse;
    }

    if ((get_dialog_id() == -1) && (sCutsceneVars[8].angle[0] != 0)) {
        if (c->cutscene != CUTSCENE_RACE_DIALOG) {
            sCutsceneDialogResponse = 3;
        }

        gCutsceneTimer = CUTSCENE_LOOP;
        retrieve_info_star(c);
        sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
        cutscene_unsoften_music(c);
    }
}

/**
 * Sets the CAM_FLAG_UNUSED_CUTSCENE_ACTIVE flag, which does nothing.
 */
BAD_RETURN(s32) cutscene_dialog_set_flag(UNUSED struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * Ends the dialog cutscene.
 */
BAD_RETURN(s32) cutscene_dialog_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    c->cutscene = 0;
    clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);
}

/**
 * Soften the music, clear cvar0
 *
 * In this cutscene, cvar0.angle[0] is used as a state variable.
 */
BAD_RETURN(s32) cutscene_read_message_start(struct Camera *c) {
    cutscene_soften_music(c);
    store_info_star(c);

    sCutsceneVars[1].angle[0] = sCUpCameraPitch;
    sCutsceneVars[1].angle[1] = sModeOffsetYaw;
    sCUpCameraPitch = -0x830;
    sModeOffsetYaw = 0;
    sCutsceneVars[0].angle[0] = 0;
}

/**
 * Cutscene that plays when Mario is reading a message (a sign or message on the wall)
 */
BAD_RETURN(s32) cutscene_read_message(struct Camera *c) {
    UNUSED u32 pad[2];

    cutscene_event(cutscene_read_message_start, c, 0, 0);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    switch (sCutsceneVars[0].angle[0]) {
        // Do nothing until message is gone.
        case 0:
            if (get_dialog_id() != -1) {
                sCutsceneVars[0].angle[0] += 1;
                set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);
            }
            break;
        // Leave the dialog.
        case 1:
            move_mario_head_c_up(c);
            update_c_up(c->focus, c->pos);

            // This could cause softlocks. If a message starts one frame after another one closes, the
            // cutscene will never end.
            if (get_dialog_id() == -1) {
                gCutsceneTimer = CUTSCENE_LOOP;
                retrieve_info_star(c);
                sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
                clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);
                // Retrieve previous state
                sCUpCameraPitch = sCutsceneVars[1].angle[0];
                sModeOffsetYaw = sCutsceneVars[1].angle[1];
                cutscene_unsoften_music(c);
            }
    }
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * Set CAM_FLAG_UNUSED_CUTSCENE_ACTIVE, which does nothing.
 */
BAD_RETURN(s32) cutscene_read_message_set_flag(UNUSED struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * End the message cutscene.
 */
BAD_RETURN(s32) cutscene_read_message_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    c->cutscene = 0;
}

/**
 * Set cvars:
 * cvar7 is Mario's pos and angle
 * cvar6 is the focus offset
 * cvar5 is the position offset
 */
BAD_RETURN(s32) cutscene_exit_succ_start(UNUSED struct Camera *c) {
    vec3f_copy(sCutsceneVars[7].point, gMarioState->pos);
    vec3s_copy(sCutsceneVars[7].angle, gMarioState->faceAngle);
    vec3f_set(sCutsceneVars[6].point, 6.f, 363.f, 543.f);
    vec3f_set(sCutsceneVars[5].point, 137.f, 226.f, 995.f);
}

/**
 * Set the camera pos depending on which level Mario exited.
 */
BAD_RETURN(s32) cutscene_non_painting_set_cam_pos(struct Camera *c) {
    UNUSED u32 unused1;
    struct Surface *floor;
    UNUSED Vec3f unused2;

    offset_rotated(c->pos, sCutsceneVars[7].point, sCutsceneVars[5].point, sCutsceneVars[7].angle);
    c->pos[1] = find_floor(c->pos[0], c->pos[1] + 1000.f, c->pos[2], &floor) + 125.f;
}

/**
 * Update the camera focus depending on which level Mario exited.
 */
BAD_RETURN(s32) cutscene_non_painting_set_cam_focus(struct Camera *c) {
    offset_rotated(c->focus, sCutsceneVars[7].point, sCutsceneVars[6].point, sCutsceneVars[7].angle);
    c->focus[1] = c->pos[1] + (gMarioState->pos[1] - c->pos[1]) * 0.2f;
}

/**
 * Focus slightly left of Mario. Perhaps to keep the bowser painting in view?
 */
BAD_RETURN(s32) cutscene_exit_bowser_succ_focus_left(UNUSED struct Camera *c) {
    sCutsceneVars[6].point[0] = approach_f32_asymptotic(sCutsceneVars[6].point[0], -24.f, 0.05f);
}

/**
 * Instead of focusing on the key, just start a pitch shake. Clever!
 * The shake lasts 32 frames.
 */
BAD_RETURN(s32) cutscene_exit_bowser_key_toss_shake(struct Camera *c) {
    //! Unnecessary check.
    if (c->cutscene == CUTSCENE_EXIT_BOWSER_SUCC) {
        set_camera_pitch_shake(0x800, 0x40, 0x800);
    }
}

/**
 * Start a camera shake when Mario lands on the ground.
 */
BAD_RETURN(s32) cutscene_exit_succ_shake_landing(UNUSED struct Camera *c) {
    set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
}

/**
 * End a non-painting exit cutscene. Used by BBH and bowser courses.
 */
BAD_RETURN(s32) cutscene_non_painting_end(struct Camera *c) {
    c->cutscene = 0;

    if (c->defMode == CAMERA_MODE_CLOSE) {
        c->mode = CAMERA_MODE_CLOSE;
    } else {
        c->mode = CAMERA_MODE_FREE_ROAM;
    }

    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    update_camera_yaw(c);
}

/**
 * Override the position offset.
 */
BAD_RETURN(s32) cutscene_exit_non_painting_succ_override_cvar(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[5].point, 137.f, 246.f, 1115.f);
}

/**
 * Cutscene that plays when Mario collects a star and leaves a non-painting course, like HMC or BBH.
 */
BAD_RETURN(s32) cutscene_exit_non_painting_succ(struct Camera *c) {
    cutscene_event(cutscene_exit_succ_start, c, 0, 0);
    cutscene_event(cutscene_exit_non_painting_succ_override_cvar, c, 0, 0);
    cutscene_event(cutscene_non_painting_set_cam_pos, c, 0, -1);
    cutscene_event(cutscene_exit_bowser_succ_focus_left, c, 18, -1);
    cutscene_event(cutscene_non_painting_set_cam_focus, c, 0, -1);
    cutscene_event(cutscene_exit_succ_shake_landing, c, 41, 41);
    update_camera_yaw(c);
}

/**
 * Set cvar7 to Mario's pos and faceAngle
 * Set cvar6 to the focus offset from Mario.
 * set cvar5 to the pos offset from Mario. (This is always overwritten)
 */
BAD_RETURN(s32) cutscene_non_painting_death_start(UNUSED struct Camera *c) {
    vec3f_copy(sCutsceneVars[7].point, gMarioState->pos);
    vec3s_copy(sCutsceneVars[7].angle, gMarioState->faceAngle);
    vec3f_set(sCutsceneVars[6].point, -42.f, 350.f, 727.f);
    // This is always overwritten, except in the unused cutscene_exit_bowser_death()
    vec3f_set(sCutsceneVars[5].point, 107.f, 226.f, 1187.f);
}

/**
 * Cutscene played when Mario dies in a non-painting course, like HMC or BBH.
 */
BAD_RETURN(s32) cutscene_non_painting_death(struct Camera *c) {
    cutscene_event(cutscene_non_painting_death_start, c, 0, 0);
    cutscene_event(cutscene_non_painting_set_cam_pos, c, 0, -1);
    cutscene_event(cutscene_non_painting_set_cam_focus, c, 0, -1);
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * Set cvars:
 * cvar3 is an offset applied to the camera's rotation around Mario. It starts at 0x1200
 * cvar 1 is more complicated:
 *      First the yaw from Mario to the camera is calculated. cvar1 is the high byte of the difference
 *      between that yaw and Mario's faceAngle plus 0x1200. The reason for taking the high byte is
 *      because cvar1 rotates until is reaches 0, so it's important that it's a multiple of 0x100.
 */
BAD_RETURN(s32) cutscene_cap_switch_press_start(struct Camera *c) {
    s32 yaw;

    store_info_star(c);
    yaw = calculate_yaw(gMarioState->pos, c->pos);
    sCutsceneVars[3].angle[1] = 0x1200;
    // Basically the amount of rotation to get from behind Mario to in front of Mario
    sCutsceneVars[1].angle[1] =
        (yaw - (gMarioState->faceAngle[1] + sCutsceneVars[3].angle[1])) & 0xFF00;
}

/**
 * Rotate around Mario. As each cvar stops updating, the rotation slows until the camera ends up in
 * front of Mario.
 */
BAD_RETURN(s32) cutscene_cap_switch_press_rotate_around_mario(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);

    // cvar3 wraps around until it reaches 0x1000
    if (sCutsceneVars[3].angle[1] != 0x1000) {
        sCutsceneVars[3].angle[1] += 0x100;
    }

    // cvar1 wraps until 0
    if (sCutsceneVars[1].angle[1] != 0) {
        sCutsceneVars[1].angle[1] += 0x100;
    }

    yaw = gMarioState->faceAngle[1] + sCutsceneVars[3].angle[1] + sCutsceneVars[1].angle[1];
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Move the camera slightly downwards.
 */
BAD_RETURN(s32) cutscene_cap_switch_press_lower_cam(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, gMarioState->pos, 0, -0x20, 0);
}

/**
 * Move the camera closer to Mario.
 */
BAD_RETURN(s32) cutscene_cap_switch_press_approach_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);
    dist = approach_f32_asymptotic(dist, 195.f, 0.2f);
    pitch = approach_s16_asymptotic(pitch, 0, 0x10);
    vec3f_set_dist_and_angle(gMarioState->pos, c->pos, dist, pitch, yaw);

    c->focus[0] = approach_f32_asymptotic(c->focus[0], gMarioState->pos[0], 0.1f);
    c->focus[1] = approach_f32_asymptotic(c->focus[1], gMarioState->pos[1] + 110.f, 0.1f);
    c->focus[2] = approach_f32_asymptotic(c->focus[2], gMarioState->pos[2], 0.1f);
}

/**
 * Pan the camera left so that Mario is on the right side of the screen when the camera stops spinning.
 */
BAD_RETURN(s32) cutscene_cap_switch_press_pan_left(struct Camera *c) {
    vec3f_copy(c->focus, gMarioState->pos);
    c->focus[1] += 110.f;
    sCutsceneVars[0].angle[1] = approach_s16_symmetric(sCutsceneVars[0].angle[1], 0x800, 0x20);
    pan_camera(c, sCutsceneVars[0].angle[0], sCutsceneVars[0].angle[1]);
}

/**
 * Create a dialog box with the cap switch's text.
 */
BAD_RETURN(s32) cutscene_cap_switch_press_create_dialog(UNUSED struct Camera *c) {
    create_dialog_box_with_response(gCutsceneFocus->oBehParams2ndByte + DIALOG_010);
}

/**
 * Cutscene that plays when Mario presses a cap switch.
 */
BAD_RETURN(s32) cutscene_cap_switch_press(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;

    cutscene_event(cutscene_cap_switch_press_start, c, 0, 0);
    cutscene_event(cutscene_cap_switch_press_approach_mario, c, 0, 30);
    cutscene_event(cutscene_cap_switch_press_pan_left, c, 0, -1);
    cutscene_event(cutscene_cap_switch_press_rotate_around_mario, c, 30, -1);
    cutscene_event(cutscene_cap_switch_press_lower_cam, c, 10, 70);
    cutscene_event(cutscene_cap_switch_press_create_dialog, c, 10, 10);
    vec3f_get_dist_and_angle(gMarioState->pos, c->pos, &dist, &pitch, &yaw);

    if (gDialogResponse != 0) {
        sCutsceneVars[4].angle[0] = gDialogResponse;
    }

    if ((get_dialog_id() == -1) && (sCutsceneVars[4].angle[0] != 0)) {
        sCutsceneDialogResponse = sCutsceneVars[4].angle[0];
        if (sCutsceneVars[4].angle[0] == 1) {
            save_file_do_save(gCurrSaveFileNum - 1);
        }
        gCutsceneTimer = CUTSCENE_STOP;
        c->cutscene = 0;
        vec3f_copy(c->focus, sCameraStoreCutscene.focus);
        vec3f_copy(c->pos, sCameraStoreCutscene.pos);
    }
}

/**
 * Sets cvars:
 * cvar0 is the camera's position
 * cvar1 is the camera's focus
 * cvar2 is the goal position
 * cvar3 is the goal focus
 */
BAD_RETURN(s32) cutscene_unlock_key_door_start(struct Camera *c) {
    Vec3f posOff, focusOff;

    vec3f_copy(sCutsceneVars[0].point, c->pos);
    vec3f_copy(sCutsceneVars[1].point, c->focus);
    vec3f_set(posOff, -206.f, 108.f, 234.f);
    vec3f_set(focusOff, 48.f, 104.f, -193.f);
    offset_rotated(sCutsceneVars[2].point, gMarioState->pos, posOff, gMarioState->faceAngle);
    offset_rotated(sCutsceneVars[3].point, gMarioState->pos, focusOff, gMarioState->faceAngle);
}

/**
 * Move the camera to the cvars position and focus, closer to Mario.
 * Gives a better view of the key.
 */
BAD_RETURN(s32) cutscene_unlock_key_door_approach_mario(struct Camera *c) {
    approach_vec3f_asymptotic(c->pos, sCutsceneVars[2].point, 0.1f, 0.1f, 0.1f);
    approach_vec3f_asymptotic(c->focus, sCutsceneVars[3].point, 0.1f, 0.1f, 0.1f);
}

/**
 * Move the camera focus up a bit, focusing on the key in the lock.
 */
BAD_RETURN(s32) cutscene_unlock_key_door_focus_lock(UNUSED struct Camera *c) {
    sCutsceneVars[3].point[1] =
        approach_f32_asymptotic(sCutsceneVars[3].point[1], gMarioState->pos[1] + 140.f, 0.07f);
}

BAD_RETURN(s32) cutscene_unlock_key_door_stub(UNUSED struct Camera *c) {
}

/**
 * Move back to the previous pos and focus, stored in cvar0 and cvar1.
 */
BAD_RETURN(s32) cutscene_unlock_key_door_fly_back(struct Camera *c) {
    approach_vec3f_asymptotic(c->pos, sCutsceneVars[0].point, 0.1f, 0.1f, 0.1f);
    approach_vec3f_asymptotic(c->focus, sCutsceneVars[1].point, 0.1f, 0.1f, 0.1f);
}

/**
 * Shake the camera's fov when the key is put in the lock.
 */
BAD_RETURN(s32) cutscene_unlock_key_door_fov_shake(UNUSED struct Camera *c) {
    cutscene_set_fov_shake_preset(1);
}

/**
 * Cutscene that plays when Mario unlocks a key door.
 */
BAD_RETURN(s32) cutscene_unlock_key_door(UNUSED struct Camera *c) {
    cutscene_event(cutscene_unlock_key_door_start, c, 0, 0);
    cutscene_event(cutscene_unlock_key_door_approach_mario, c, 0, 123);
    cutscene_event(cutscene_unlock_key_door_fly_back, c, 124, -1);
    cutscene_event(cutscene_unlock_key_door_fov_shake, c, 79, 79);
    cutscene_event(cutscene_unlock_key_door_focus_lock, c, 70, 110);
    cutscene_event(cutscene_unlock_key_door_stub, c, 112, 112);
}

/**
 * Ends the double door cutscene.
 */
BAD_RETURN(s32) cutscene_double_doors_end(struct Camera *c) {
    set_flag_post_door(c);
    c->cutscene = 0;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

/**
 * Warp the camera to Mario, then use his faceAngle to calculate the right relative position.
 *
 * cvar0.point is Mario's position
 * cvar0.angle is Mario's faceAngle
 *
 * cvar1 is the camera's position relative to Mario
 * cvar2 is the camera's focus relative to Mario
 */
BAD_RETURN(s32) cutscene_exit_painting_start(struct Camera *c) {
    struct Surface *floor;
    f32 floorHeight;

    vec3f_set(sCutsceneVars[2].point, 258.f, -352.f, 1189.f);
    vec3f_set(sCutsceneVars[1].point, 65.f, -155.f, 444.f);

    vec3f_copy(sCutsceneVars[0].point, gMarioState->pos);
    sCutsceneVars[0].angle[0] = 0;
    sCutsceneVars[0].angle[1] = gMarioState->faceAngle[1];
    sCutsceneVars[0].angle[2] = 0;
    offset_rotated(c->focus, sCutsceneVars[0].point, sCutsceneVars[1].point, sCutsceneVars[0].angle);
    offset_rotated(c->pos, sCutsceneVars[0].point, sCutsceneVars[2].point, sCutsceneVars[0].angle);
    floorHeight = find_floor(c->pos[0], c->pos[1] + 10.f, c->pos[2], &floor);

    if (floorHeight != -11000.f) {
        if (c->pos[1] < (floorHeight += 60.f)) {
            c->pos[1] = floorHeight;
        }
    }
}

/**
 * Decrease cvar2's x and z offset, moving closer to Mario.
 */
BAD_RETURN(s32) cutscene_exit_painting_move_to_mario(struct Camera *c) {
    Vec3f pos;

    //! Tricky math: Since offset_rotated() flips Z offsets, you'd expect a positive Z offset to move
    //! the camera into the wall. However, Mario's faceAngle always points into the painting, so a
    //! positive Z offset moves the camera "behind" Mario, away from the painting.
    //!
    //! In the success cutscene, when Mario jumps out face-first, only his gfx angle is updated. His
    //! actual face angle isn't updated until after the cutscene.
    sCutsceneVars[2].point[0] = approach_f32_asymptotic(sCutsceneVars[2].point[0], 178.f, 0.05f);
    sCutsceneVars[2].point[2] = approach_f32_asymptotic(sCutsceneVars[2].point[2], 889.f, 0.05f);
    offset_rotated(pos, sCutsceneVars[0].point, sCutsceneVars[2].point, sCutsceneVars[0].angle);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
}

/**
 * Move the camera down to the floor Mario lands on.
 */
BAD_RETURN(s32) cutscene_exit_painting_move_to_floor(struct Camera *c) {
    struct Surface *floor;
    Vec3f floorHeight;

    vec3f_copy(floorHeight, gMarioState->pos);
    floorHeight[1] =
        find_floor(gMarioState->pos[0], gMarioState->pos[1] + 10.f, gMarioState->pos[2], &floor);

    if (floor != NULL) {
        floorHeight[1] = floorHeight[1] + (gMarioState->pos[1] - floorHeight[1]) * 0.7f + 125.f;
        approach_vec3f_asymptotic(c->focus, floorHeight, 0.2f, 0.2f, 0.2f);

        if (floorHeight[1] < c->pos[1]) {
            c->pos[1] = approach_f32_asymptotic(c->pos[1], floorHeight[1], 0.05f);
        }
    }
}

/**
 * Cutscene played when Mario leaves a painting, either due to death or collecting a star.
 */
BAD_RETURN(s32) cutscene_exit_painting(struct Camera *c) {
    cutscene_event(cutscene_exit_painting_start, c, 0, 0);
    cutscene_event(cutscene_exit_painting_move_to_mario, c, 5, -1);
    cutscene_event(cutscene_exit_painting_move_to_floor, c, 5, -1);

    update_camera_yaw(c);
}

/**
 * Give control back to the player.
 */
BAD_RETURN(s32) cutscene_exit_painting_end(struct Camera *c) {
    c->mode = CAMERA_MODE_CLOSE;
    c->cutscene = 0;
    gCutsceneTimer = CUTSCENE_STOP;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    update_camera_yaw(c);
}

/**
 * End the cutscene, starting cannon mode.
 */
BAD_RETURN(s32) cutscene_enter_cannon_end(struct Camera *c) {
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags |= CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    c->mode = CAMERA_MODE_INSIDE_CANNON;
    c->cutscene = 0;
    sCannonYOffset = 800.f;
}

/**
 * Rotate around the cannon as it rises out of the hole.
 */
BAD_RETURN(s32) cutscene_enter_cannon_raise(struct Camera *c) {
    struct Object *o;
    UNUSED u32 pad[2];
    f32 floorHeight;
    struct Surface *floor;
    Vec3f cannonFocus;
    Vec3s cannonAngle;

    // Shake the camera when the cannon is fully raised
    cutscene_event(cutscene_shake_explosion, c, 70, 70);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sCutsceneVars[1].angle[0] = approach_s16_symmetric(&sCutsceneVars[1].angle[0], 0, 0x80);
    sCutsceneVars[2].angle[0] = approach_s16_symmetric(sCutsceneVars[2].angle[0], 0, 0x80);
    // Move the camera around the cannon, gradually rotating and moving closer
    vec3f_set_dist_and_angle(sCutsceneVars[0].point, c->pos, sCutsceneVars[1].point[2],
                             sCutsceneVars[1].angle[0], sCutsceneVars[1].angle[1]);
    sCutsceneVars[1].point[2] = approach_f32(sCutsceneVars[1].point[2], 400.f, 5.f, 5.f);
    sCutsceneVars[1].angle[1] += 0x40;
    sCutsceneVars[3].point[1] += 2.f;
    c->pos[1] += sCutsceneVars[3].point[1];

    if ((o = gPlayerCameraState.usedObj) != NULL) {
        sCutsceneVars[0].point[1] = o->oPosY;
        cannonAngle[0] = o->oMoveAnglePitch;
        cannonAngle[1] = o->oMoveAngleYaw;
        cannonAngle[2] = o->oMoveAngleRoll;
        c->focus[0] = o->oPosX;
        c->focus[1] = o->oPosY;
        c->focus[2] = o->oPosZ;
        cannonFocus[0] = 0.f;
        cannonFocus[1] = 100.f;
        cannonFocus[2] = 0.f;
        offset_rotated(c->focus, c->focus, cannonFocus, cannonAngle);
    }

    floorHeight = find_floor(c->pos[0], c->pos[1] + 500.f, c->pos[2], &floor) + 100.f;

    if (c->pos[1] < floorHeight) {
        c->pos[1] = floorHeight;
    }
}

/**
 * Start the cannon entering cutscene
 */
BAD_RETURN(s32) cutscene_enter_cannon_start(struct Camera *c) {
    UNUSED u32 cvar3Start;
    UNUSED u32 cvar4Start;
    struct Object *o;

    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    gPlayerCameraState.cameraEvent = 0;

    // Store the cannon's position and angle in cvar0
    if ((o = gPlayerCameraState.usedObj) != NULL) {
        sCutsceneVars[0].point[0] = o->oPosX;
        sCutsceneVars[0].point[1] = o->oPosY;
        sCutsceneVars[0].point[2] = o->oPosZ;
        sCutsceneVars[0].angle[0] = o->oMoveAnglePitch;
        sCutsceneVars[0].angle[1] = o->oMoveAngleYaw;
        sCutsceneVars[0].angle[2] = o->oMoveAngleRoll;
    }

    // Store the camera's polar offset from the cannon in cvar1
    vec3f_get_dist_and_angle(sCutsceneVars[0].point, c->pos, &sCutsceneVars[1].point[2],
                             &sCutsceneVars[1].angle[0], &sCutsceneVars[1].angle[1]);
    sCutsceneVars[3].point[1] = 0.f;
    //! cvar4 is unused in this cutscene
    sCutsceneVars[4].point[1] = 0.f;
}

/**
 * Store the camera's pos and focus for the door cutscene
 */
BAD_RETURN(s32) cutscene_door_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[0].point, c->pos);
    vec3f_copy(sCutsceneVars[1].point, c->focus);
}

/**
 * Fix the camera in place while the door opens.
 */
BAD_RETURN(s32) cutscene_door_fix_cam(struct Camera *c) {
    vec3f_copy(c->pos, sCutsceneVars[0].point);
    vec3f_copy(c->focus, sCutsceneVars[1].point);
}

/**
 * Loop until Mario is no longer using the door.
 */
BAD_RETURN(s32) cutscene_door_loop(struct Camera *c) {
    //! bitwise AND instead of boolean
    if ((gMarioState->action != ACT_PULLING_DOOR) & (gMarioState->action != ACT_PUSHING_DOOR)) {
        gCutsceneTimer = CUTSCENE_STOP;
        c->cutscene = 0;
    }
}

/**
 * Warp the camera behind Mario.
 */
BAD_RETURN(s32) cutscene_door_move_behind_mario(struct Camera *c) {
    Vec3f camOffset;
    s16 doorRotation;

    determine_pushing_or_pulling_door(&doorRotation);
    set_focus_rel_mario(c, 0.f, 125.f, 0.f, 0);
    vec3s_set(sCutsceneVars[0].angle, 0, gMarioState->faceAngle[1] + doorRotation, 0);
    vec3f_set(camOffset, 0.f, 125.f, 250.f);

    if (doorRotation == 0) { //! useless code
        camOffset[0] = 0.f;
    } else {
        camOffset[0] = 0.f;
    }

    offset_rotated(c->pos, gMarioState->pos, camOffset, sCutsceneVars[0].angle);
}

/**
 * Follow Mario through the door.
 */
BAD_RETURN(s32) cutscene_door_follow_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    set_focus_rel_mario(c, 0.f, 125.f, 0.f, 0);
    vec3f_get_dist_and_angle(c->focus, c->pos, &dist, &pitch, &yaw);
    dist = approach_f32_symmetric(dist, 150.f, 7.f);
    vec3f_set_dist_and_angle(c->focus, c->pos, dist, pitch, yaw);
    update_camera_yaw(c);
}

/**
 * Ends the door cutscene. Sets the camera mode to close mode unless the default is free roam.
 */
BAD_RETURN(s32) cutscene_door_end(struct Camera *c) {
    /* if (c->defMode == CAMERA_MODE_FREE_ROAM) {
         c->mode = CAMERA_MODE_FREE_ROAM;
     } else {
         c->mode = CAMERA_MODE_CLOSE;
     }*/
    c->cutscene = 0;
    c->mode = CAM_MODE_NEWCAM;
    gCutsceneTimer = CUTSCENE_STOP;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    set_flag_post_door(c);
    update_camera_yaw(c);
}

/**
 * Used for entering a room that uses a specific camera mode, like the castle lobby or BBH
 */
BAD_RETURN(s32) cutscene_door_mode(struct Camera *c) {
    UNUSED u32 pad[2];

    c->yaw = c->nextYaw;

    // Loop until Mario is no longer using the door
    if (gMarioState->action != ACT_ENTERING_STAR_DOOR && gMarioState->action != ACT_PULLING_DOOR
        && gMarioState->action != ACT_PUSHING_DOOR) {
        gCutsceneTimer = CUTSCENE_STOP;
        c->cutscene = 0;
    }
}

/******************************************************************************************************
 * Cutscenes
 ******************************************************************************************************/

/**
 * Cutscene that plays when Mario enters a door that warps to another area.
 */
struct Cutscene sCutsceneDoorWarp[] = { { cutscene_door_start, 1 },
                                        { cutscene_door_loop, CUTSCENE_LOOP } };

extern void intro_cutscenes(struct Camera *c);
struct Cutscene IntroCutscenes[] = { { intro_cutscenes, CUTSCENE_LOOP } };
/**
 * Cutscene that plays when Mario pulls open a door.
 */
struct Cutscene sCutsceneDoorPull[] = { { cutscene_door_start, 1 },
                                        { cutscene_door_fix_cam, 30 },
                                        { cutscene_door_move_behind_mario, 1 },
                                        { cutscene_door_follow_mario, 50 },
                                        { cutscene_door_end, 0 } };

/**
 * Cutscene that plays when Mario pushes open a door.
 */
struct Cutscene sCutsceneDoorPush[] = { { cutscene_door_start, 1 },
                                        { cutscene_door_fix_cam, 20 },
                                        { cutscene_door_move_behind_mario, 1 },
                                        { cutscene_door_follow_mario, 50 },
                                        { cutscene_door_end, 0 } };

/**
 * Cutscene that plays when Mario pulls open a door that has some special mode requirement on the other
 * side.
 */
struct Cutscene sCutsceneDoorPullMode[] = { { cutscene_door_start, 1 },
                                            { cutscene_door_fix_cam, 30 },
                                            { cutscene_door_mode, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario pushes open a door that has some special mode requirement on the other
 * side.
 */
struct Cutscene sCutsceneDoorPushMode[] = { { cutscene_door_start, 1 },
                                            { cutscene_door_fix_cam, 20 },
                                            { cutscene_door_mode, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario enters the cannon and it rises out of the hole.
 */
struct Cutscene sCutsceneEnterCannon[] = { { cutscene_enter_cannon_start, 1 },
                                           { cutscene_enter_cannon_raise, 121 },
                                           { cutscene_enter_cannon_end, 0 } };

/**
 * Cutscene that plays when a star spawns from ie a box or after a boss fight.
 */
struct Cutscene sCutsceneStarSpawn[] = { { cutscene_star_spawn, CUTSCENE_LOOP },
                                         { cutscene_star_spawn_back, 15 },
                                         { cutscene_star_spawn_end, 0 } };

/**
 * Cutscene for the red coin star spawning. Compared to a regular star, this cutscene can warp long
 * distances.
 */
struct Cutscene sCutsceneRedCoinStarSpawn[] = { { cutscene_red_coin_star, CUTSCENE_LOOP },
                                                { cutscene_red_coin_star_end, 0 } };

/**
 * Cutscene that plays when Mario dies and warps back to the castle.
 */
struct Cutscene sCutsceneDeathExit[] = { { cutscene_exit_painting, 118 },
                                         { cutscene_exit_painting_end, 0 } };

/**
 * Cutscene that plays when a cannon door is opened.
 */
struct Cutscene sCutscenePrepareCannon[] = { { cutscene_prepare_cannon, 170 },
                                             { cutscene_prepare_cannon_end, 0 } };

/**
 * Cutscene that plays when Mario dies while standing, or from electrocution.
 */
struct Cutscene sCutsceneStandingDeath[] = { { cutscene_death_standing, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario dies on his stomach.
 */
struct Cutscene sCutsceneDeathStomach[] = { { cutscene_death_stomach, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario dies on his back.
 */
struct Cutscene sCutsceneDeathOnBack[] = { { cutscene_bbh_death, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario dies in quicksand.
 */
struct Cutscene sCutsceneQuicksandDeath[] = {
    { cutscene_quicksand_death, CUTSCENE_LOOP },
};

/**
 * Unused cutscene for ACT_WATER_DEATH, which happens when Mario gets hit by an enemy under water.
 */
struct Cutscene sCutsceneWaterDeath[] = { { cutscene_quicksand_death, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario suffocates.
 */
struct Cutscene sCutsceneSuffocation[] = { { cutscene_suffocation, CUTSCENE_LOOP } };

// The dance cutscenes are automatically stopped since reset_camera() is called after Mario warps.

/**
 * Star dance cutscene.
 * For the default dance, the camera moves closer to Mario, then stays in place.
 * For the rotate dance, the camera moves closer and rotates clockwise around Mario.
 */
struct Cutscene sCutsceneDanceDefaultRotate[] = { { cutscene_dance_default_rotate, CUTSCENE_LOOP } };

/**
 * Star dance cutscene.
 * The camera moves closer and rotates clockwise around Mario.
 */
struct Cutscene sCutsceneDanceFlyAway[] = { { cutscene_dance_fly_away, CUTSCENE_LOOP } };

/**
 * Star dance cutscene.
 * The camera moves in for a closeup on Mario. Used in tight spaces and underwater.
 */
struct Cutscene sCutsceneDanceCloseup[] = { { cutscene_dance_closeup, CUTSCENE_LOOP } };

/**
 * Star dance cutscene.
 * The camera moves closer and rotates clockwise around Mario.
 */
struct Cutscene sCutsceneKeyDance[] = { { cutscene_key_dance, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario presses a cap switch.
 */
struct Cutscene sCutsceneCapSwitchPress[] = { { cutscene_cap_switch_press, CUTSCENE_LOOP } };

/**
 * Cutscene that plays when Mario unlocks the basement or upstairs key door.
 */
struct Cutscene sCutsceneUnlockKeyDoor[] = { { cutscene_unlock_key_door, 200 },
                                             { cutscene_double_doors_end, 0 } };

/**
 * Cutscene that plays when Mario exits a non-painting course, like HMC.
 */
struct Cutscene sCutsceneExitSpecialSuccess[] = { { cutscene_exit_non_painting_succ, 163 },
                                                  { cutscene_non_painting_end, 0 } };

/**
 * Cutscene that plays when Mario exits from dying in a non-painting course, like HMC.
 */
struct Cutscene sCutsceneNonPaintingDeath[] = { { cutscene_non_painting_death, 120 },
                                                { cutscene_non_painting_end, 0 } };

/**
 * Cutscene that plays when Mario talks to a creature.
 */
struct Cutscene sCutsceneDialog[] = { { cutscene_dialog, CUTSCENE_LOOP },
                                      { cutscene_dialog_set_flag, 12 },
                                      { cutscene_dialog_end, 0 } };

/**
 * Cutscene that plays when Mario reads a sign or message.
 */
struct Cutscene sCutsceneReadMessage[] = { { cutscene_read_message, CUTSCENE_LOOP },
                                           { cutscene_read_message_set_flag, 15 },
                                           { cutscene_read_message_end, 0 } };

/* TODO:
 * The next two arrays are both related to levels, and they look generated.
 * These should be split into their own file.
 */

/**
 * Converts the u32 given in DEFINE_COURSE to a u8 with the odd and even digits rotated into the right
 * order for sDanceCutsceneIndexTable
 */
#define DROT(value, index)                                                                             \
    ((value >> (32 - (index + 1) * 8)) & 0xF0) >> 4 | ((value >> (32 - (index + 1) * 8)) & 0x0F) << 4

#define DANCE_ENTRY(c) { DROT(c, 0), DROT(c, 1), DROT(c, 2), DROT(c, 3) },

#define DEFINE_COURSE(_0, cutscenes) DANCE_ENTRY(cutscenes)
#define DEFINE_COURSES_END()
#define DEFINE_BONUS_COURSE(_0, cutscenes) DANCE_ENTRY(cutscenes)

/**
 * Each hex digit is an index into sDanceCutsceneTable.
 *
 * 0: Lakitu flies away after the dance
 * 1: Only rotates the camera, doesn't zoom out
 * 2: The camera goes to a close up of Mario
 * 3: Bowser keys and the grand star
 * 4: Default, used for 100 coin stars, 8 red coin stars in bowser levels, and secret stars
 */
u8 sDanceCutsceneIndexTable[][4] = {
#include "levels/course_defines.h"
    { 0x44, 0x44, 0x44, 0x04 }, // (26) Why go to all this trouble to save bytes and do this?!
};
#undef DEFINE_COURSE
#undef DEFINE_COURSES_END
#undef DEFINE_BONUS_COURSE

#undef DANCE_ENTRY
#undef DROT
u8 sZoomOutAreaMasks[] = {
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 0, 0, 0, 0), // Unused         | Unused
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 0, 0, 0, 0), // Unused         | Unused
	ZOOMOUT_AREA_MASK(1, 1, 1, 1, 1, 1, 1, 1), // BBH            | CCM
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 1, 1, 0), // CASTLE_INSIDE  | HMC
	ZOOMOUT_AREA_MASK(1, 1, 1, 0, 1, 1, 1, 1), // SSL            | BOB
	ZOOMOUT_AREA_MASK(1, 1, 1, 0, 1, 0, 0, 0), // SL             | WDW
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 1, 0, 0), // JRB            | THI
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 0, 0, 0), // TTC            | RR
	ZOOMOUT_AREA_MASK(1, 1, 1, 1, 1, 1, 0, 0), // CASTLE_GROUNDS | BITDW
	ZOOMOUT_AREA_MASK(1, 0, 0, 0, 1, 0, 0, 0), // VCUTM          | BITFS
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 0, 0, 0), // SA             | BITS
	ZOOMOUT_AREA_MASK(1, 1, 1, 0, 1, 1, 1, 0), // LLL            | DDD
	ZOOMOUT_AREA_MASK(1, 0, 0, 0, 1, 0, 0, 0), // WF             | ENDING
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 0, 0, 0), // COURTYARD      | PSS
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 0, 0, 0), // COTMC          | TOTWC
	ZOOMOUT_AREA_MASK(1, 0, 0, 0, 1, 0, 0, 0), // BOWSER_1       | WMOTR
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 1, 0, 0, 0), // Unused         | BOWSER_2
	ZOOMOUT_AREA_MASK(1, 0, 0, 0, 0, 0, 0, 0), // BOWSER_3       | Unused
	ZOOMOUT_AREA_MASK(1, 0, 0, 0, 0, 0, 0, 0), // TTM            | Unused
	ZOOMOUT_AREA_MASK(0, 0, 0, 0, 0, 0, 0, 0), // Unused         | Unused
};

/**
 * Play the current cutscene until either gCutsceneTimer reaches the max time, or c->cutscene is set to
 * 0
 *
 * Note that CAM_FLAG_SMOOTH_MOVEMENT is cleared while a cutscene is playing, so cutscenes set it for
 * the duration they want the flag to be active.
 */
__attribute__((always_inline))  inline  void play_cutscene(struct Camera *c) {
    s32 cutsceneDuration;
    s32 oldCutscene;
    oldCutscene = c->cutscene;
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;

#define CUTSCENE(id, cutscene)                                                                         \
    case id:                                                                                           \
        cutsceneDuration = cutscene[sCutsceneShot].duration;                                           \
        cutscene[sCutsceneShot].shot(c);                                                               \
        break;

    switch (c->cutscene) {
        CUTSCENE(CUTSCENE_STAR_SPAWN, sCutsceneStarSpawn)
        CUTSCENE(CUTSCENE_RED_COIN_STAR_SPAWN, sCutsceneRedCoinStarSpawn)
        // CUTSCENE(CUTSCENE_ENDING, sCutsceneEnding)
        // CUTSCENE(CUTSCENE_GRAND_STAR, sCutsceneGrandStar)
        CUTSCENE(CUTSCENE_DOOR_WARP, sCutsceneDoorWarp)
        CUTSCENE(CUTSCENE_DOOR_PULL, sCutsceneDoorPull)
        CUTSCENE(CUTSCENE_DOOR_PUSH, sCutsceneDoorPush)
        CUTSCENE(CUTSCENE_DOOR_PULL_MODE, sCutsceneDoorPullMode)
        CUTSCENE(CUTSCENE_DOOR_PUSH_MODE, sCutsceneDoorPushMode)
        CUTSCENE(CUTSCENE_ENTER_CANNON, sCutsceneEnterCannon)
        // CUTSCENE(CUTSCENE_ENTER_PAINTING, sCutsceneEnterPainting)
        CUTSCENE(CUTSCENE_DEATH_EXIT, sCutsceneDeathExit)
        // CUTSCENE(CUTSCENE_EXIT_PAINTING_SUCC, sCutsceneExitPaintingSuccess)
        // CUTSCENE(CUTSCENE_UNUSED_EXIT, sCutsceneUnusedExit)
        // CUTSCENE(CUTSCENE_INTRO_PEACH, sCutsceneIntroPeach)
        // CUTSCENE(CUTSCENE_ENTER_BOWSER_ARENA, sCutsceneEnterBowserArena)
        CUTSCENE(CUTSCENE_DANCE_ROTATE, sCutsceneDanceDefaultRotate)
        CUTSCENE(CUTSCENE_DANCE_DEFAULT, sCutsceneDanceDefaultRotate)
        CUTSCENE(CUTSCENE_DANCE_FLY_AWAY, sCutsceneDanceFlyAway)
        CUTSCENE(CUTSCENE_DANCE_CLOSEUP, sCutsceneDanceCloseup)
        CUTSCENE(CUTSCENE_KEY_DANCE, sCutsceneKeyDance)
        // CUTSCENE(CUTSCENE_0F_UNUSED, sCutsceneUnused)
        // CUTSCENE(CUTSCENE_END_WAVING, sCutsceneEndWaving)
        // CUTSCENE(CUTSCENE_CREDITS, sCutsceneCredits)
        CUTSCENE(0xc9, IntroCutscenes)
        CUTSCENE(CUTSCENE_CAP_SWITCH_PRESS, sCutsceneCapSwitchPress)
        // CUTSCENE(CUTSCENE_SLIDING_DOORS_OPEN, sCutsceneSlidingDoorsOpen)
        CUTSCENE(CUTSCENE_PREPARE_CANNON, sCutscenePrepareCannon)
        CUTSCENE(CUTSCENE_UNLOCK_KEY_DOOR, sCutsceneUnlockKeyDoor)
        CUTSCENE(CUTSCENE_STANDING_DEATH, sCutsceneStandingDeath)
        // CUTSCENE(CUTSCENE_ENTER_POOL, sCutsceneEnterPool)
        CUTSCENE(CUTSCENE_DEATH_ON_STOMACH, sCutsceneDeathStomach)
        CUTSCENE(CUTSCENE_DEATH_ON_BACK, sCutsceneDeathOnBack)
        CUTSCENE(CUTSCENE_QUICKSAND_DEATH, sCutsceneQuicksandDeath)
        CUTSCENE(CUTSCENE_SUFFOCATION_DEATH, sCutsceneSuffocation)
        // CUTSCENE(CUTSCENE_EXIT_BOWSER_SUCC, sCutsceneExitBowserSuccess)
        // CUTSCENE(CUTSCENE_EXIT_BOWSER_DEATH, sCutsceneExitBowserDeath)
        CUTSCENE(CUTSCENE_EXIT_SPECIAL_SUCC, sCutsceneExitSpecialSuccess)
        // CUTSCENE(CUTSCENE_EXIT_WATERFALL, sCutsceneExitWaterfall)
        // CUTSCENE(CUTSCENE_EXIT_FALL_WMOTR, sCutsceneFallToCastleGrounds)
        CUTSCENE(CUTSCENE_NONPAINTING_DEATH, sCutsceneNonPaintingDeath)
        CUTSCENE(CUTSCENE_DIALOG, sCutsceneDialog)
        CUTSCENE(CUTSCENE_READ_MESSAGE, sCutsceneReadMessage)
        CUTSCENE(CUTSCENE_RACE_DIALOG, sCutsceneDialog)
        // CUTSCENE(CUTSCENE_ENTER_PYRAMID_TOP, sCutsceneEnterPyramidTop)
        // CUTSCENE(CUTSCENE_SSL_PYRAMID_EXPLODE, sCutscenePyramidTopExplode)
    }

#undef CUTSCENE

    if ((cutsceneDuration != 0) && !(gCutsceneTimer & CUTSCENE_STOP)) {
        //! @bug This should check for 0x7FFF (CUTSCENE_LOOP)
        //! instead, cutscenes that last longer than 0x3FFF frames will never end on their own
        if (gCutsceneTimer < 0x3FFF) {
            gCutsceneTimer += 1;
        }
        //! Because gCutsceneTimer is often set to 0x7FFF (CUTSCENE_LOOP), this conditional can only
        //! check for == due to overflow
        if (gCutsceneTimer == cutsceneDuration) {
            sCutsceneShot += 1;
            gCutsceneTimer = 0;
        }
    } else {
        gPlayerCameraState.cameraEvent = 0;
        sCutsceneShot = 0;
        gCutsceneTimer = 0;
    }

    // The cutscene just ended
    if ((c->cutscene == 0) && (oldCutscene != 0)) {
        gRecentCutscene = oldCutscene;
    }
}

/**
 * Call the event while `start` <= gCutsceneTimer <= `end`
 * If `end` is -1, call for the rest of the shot.
 */
s32 cutscene_event(CameraEvent event, struct Camera *c, s32 start, s32 end) {
    if (start <= gCutsceneTimer) {
        if (end == -1 || end >= gCutsceneTimer) {
            event(c);
        }
    }
    return 0;
}

/**
 * Start shaking the camera's field of view.
 *
 * @param shakeSpeed How fast the shake should progress through its period. The shake offset is
 *                   calculated from coss(), so this parameter can be thought of as an angular velocity.
 */
void set_fov_shake(s32 amplitude, s32 decay, s32 shakeSpeed) {
    if (amplitude > sFOVState.shakeAmplitude) {
        sFOVState.shakeAmplitude = amplitude;
        sFOVState.decay = decay;
        sFOVState.shakeSpeed = shakeSpeed;
    }
}

/**
 * Start shaking the camera's field of view, but reduce `amplitude` by distance from camera
 */
void set_fov_shake_from_point(s32 amplitude, s32 decay, s32 shakeSpeed, f32 maxDist, f32 posX, f32 posY,
                              f32 posZ) {
    amplitude = reduce_by_dist_from_camera(amplitude, maxDist, posX, posY, posZ);

    if (amplitude != 0) {
        if (amplitude
            > sFOVState.shakeAmplitude) { // literally use the function above you silly nintendo, smh
            sFOVState.shakeAmplitude = amplitude;
            sFOVState.decay = decay;
            sFOVState.shakeSpeed = shakeSpeed;
        }
    }
}

/**
 * Add a cyclic offset to the camera's field of view based on a cosine wave
 */
__attribute__((always_inline))  inline  void shake_camera_fov(struct GraphNodePerspective *perspective) {
    if (sFOVState.shakeAmplitude != 0.f) {
        sFOVState.fovOffset = coss(sFOVState.shakePhase) * sFOVState.shakeAmplitude / 0x100;
        sFOVState.shakePhase += sFOVState.shakeSpeed;
        sFOVState.shakeAmplitude =
            approach_f32_symmetric(sFOVState.shakeAmplitude, 0.f, sFOVState.decay);
        perspective->fov += sFOVState.fovOffset;
    } else {
        sFOVState.shakePhase = 0;
    }
}
/**
 * This is the default fov function. It makes fov approach 45 degrees, and it handles zooming in when
 * Mario falls a sleep.
 */
void fov_default(struct MarioState *m) {
    if ((m->action == ACT_SLEEPING) || (m->action == ACT_START_SLEEPING)) {
        sFOVState.fov =
            approach_f32_symmetric(sFOVState.fov, 30.f, absf((30.f - sFOVState.fov) / 30.f));
    } else {
        sFOVState.fov =
            approach_f32_symmetric(sFOVState.fov, 45.f, absf((45.f - sFOVState.fov) / 30.f));
    }
}
/**
 * Sets the field of view for the GraphNodeCamera
 */
Gfx *geo_camera_fov(s32 callContext, struct GraphNode *g, UNUSED void *context) {
    struct GraphNodePerspective *perspective = (struct GraphNodePerspective *) g;
    struct MarioState *m = &gMarioStates[0];
    s32 fovFunc = sFOVState.fovFunc;
    f32 targetFoV;
    if (callContext == GEO_CONTEXT_RENDER) {
        switch (fovFunc) {
            case CAM_FOV_SET_45:
                sFOVState.fov = 45.f;
                break;
            case CAM_FOV_SET_29:
                sFOVState.fov = 29.f;
                break;
            case CAM_FOV_ZOOM_30:
                sFOVState.fov =
                    approach_f32_symmetric(sFOVState.fov, 30.f, absf((30.f - sFOVState.fov) / 60.f));
                break;
            case CAM_FOV_DEFAULT:
                fov_default(m);
                break;
            case CAM_FOV_BBH:
                sFOVState.fov = 45.f;
                break;
            case CAM_FOV_APP_45:
                targetFoV = sFOVState.fov;

                if (m->area->camera->mode == CAMERA_MODE_FIXED && m->area->camera->cutscene == 0) {
                    targetFoV = 45.f;
                } else {
                    targetFoV = 45.f;
                }

                sFOVState.fov = approach_f32(sFOVState.fov, targetFoV, 2.f, 2.f);
                break;
            case CAM_FOV_SET_30:
                sFOVState.fov = 30.f;
                break;
            case CAM_FOV_APP_20:
                sFOVState.fov = approach_f32_symmetric(sFOVState.fov, 20.f, 0.3f);
                break;
            case CAM_FOV_APP_80:
    sFOVState.fov = approach_f32_symmetric(sFOVState.fov, 80.f, 3.5f);
                break;
            case CAM_FOV_APP_30:
                sFOVState.fov = approach_f32_symmetric(sFOVState.fov, 30.f, 1.f);
                break;
            case CAM_FOV_APP_60:
                sFOVState.fov = approach_f32_symmetric(sFOVState.fov, 60.f, 1.f);
                break;
                //! No default case
        }
    }

    perspective->fov = sFOVState.fov;
    shake_camera_fov(perspective);
    return NULL;
}

/**
 * Change the camera's FOV mode.
 *
 * @see geo_camera_fov
 */
void set_fov_function(s32 func) {
    sFOVState.fovFunc = func;
}

/**
 * Start a preset fov shake. Used in cutscenes
 */
void cutscene_set_fov_shake_preset(s32 preset) {
    switch (preset) {
        case 1:
            set_fov_shake(0x100, 0x30, 0x8000);
            break;
        case 2:
            set_fov_shake(0x400, 0x20, 0x4000);
            break;
    }
}

/**
 * Start a preset fov shake that is reduced by the point's distance from the camera.
 * Used in set_camera_shake_from_point
 *
 * @see set_camera_shake_from_point
 */
void set_fov_shake_from_point_preset(s32 preset, f32 posX, f32 posY, f32 posZ) {
    switch (preset) {
        case SHAKE_FOV_SMALL:
            set_fov_shake_from_point(0x100, 0x30, 0x8000, 3000.f, posX, posY, posZ);
            break;
        case SHAKE_FOV_MEDIUM:
            set_fov_shake_from_point(0x200, 0x30, 0x8000, 4000.f, posX, posY, posZ);
            break;
        case SHAKE_FOV_LARGE:
            set_fov_shake_from_point(0x300, 0x30, 0x8000, 6000.f, posX, posY, posZ);
            break;
        case SHAKE_FOV_UNUSED:
            set_fov_shake_from_point(0x800, 0x20, 0x4000, 3000.f, posX, posY, posZ);
            break;
    }
}

int timer = 0;
int timeAccumulated = 0;
int length = 0;
struct CutsceneJump *currentScene = 0;
struct CutsceneJump {
    int length;
    struct CutsceneSplinePoint *pos;
    struct CutsceneSplinePoint *foc;
    struct CutsceneJump *next;
};

struct CutsceneJump bobScene4 = { 58, bob_area_1_spline_IntroCutscenePos_003,
                                  bob_area_1_spline_IntroCutsceneFoc_003, 0 };

struct CutsceneJump bobScene3 = { 58, bob_area_1_spline_IntroCutscenePos_002,
                                  bob_area_1_spline_IntroCutsceneFoc_002, &bobScene4 };

struct CutsceneJump bobScene2 = { 58, bob_area_1_spline_IntroCutscenePos_001,
                                  bob_area_1_spline_IntroCutsceneFoc_001, &bobScene3 };

struct CutsceneJump bobScene1 = { 58, bob_area_1_spline_IntroCutscenePos,
                                  bob_area_1_spline_IntroCutsceneFoc, &bobScene2 };
// show mario with fishes and the coin formation going into the dome
// show star with close up cam, move out, show the level in the process and mario in the bottom left
// corner show air vent, move up, sway to bridge from turtle face, sway to the level main area

struct CutsceneJump wfScene4 = { 90, wf_area_1_spline_IntroCutscenePos,
                                 wf_area_1_spline_IntroCutsceneFoc, 0 };

struct CutsceneJump wfScene3 = { 179, wf_area_1_spline_IntroCutscenePos_003,
                                 wf_area_1_spline_IntroCutsceneFoc_003, &wfScene4 };

struct CutsceneJump wfScene2 = { 114, wf_area_1_spline_IntroCutscenePos_002,
                                 wf_area_1_spline_IntroCutsceneFoc_002, &wfScene3 };

struct CutsceneJump wfScene1 = { 176, wf_area_1_spline_IntroCutscenePos_001,
                                 wf_area_1_spline_IntroCutsceneFoc_001, &wfScene2 };

// rainbow grotto scene
// pan mirror room from bats to mirror
// show cloud powerup and the destination for it
// pan boo house from left to right
// show boohouse from back, move camera towards mice workers

struct CutsceneJump jrbScene5 = { 1, jrb_area_1_spline_IntroCutscenePos_004,
                                  jrb_area_1_spline_IntroCutscenePos_005, 0 };
struct CutsceneJump jrbScene4 = { 205, jrb_area_1_spline_IntroCutscenePos_003,
                                  jrb_area_1_spline_IntroCutsceneFoc_003, &jrbScene5 };
struct CutsceneJump jrbScene3 = { 160, jrb_area_1_spline_IntroCutscenePos_002,
                                  jrb_area_1_spline_IntroCutsceneFoc_002, &jrbScene4 };
struct CutsceneJump jrbScene2 = { 120, jrb_area_1_spline_IntroCutscenePos_001,
                                  jrb_area_1_spline_IntroCutsceneFoc_001, &jrbScene3 };
struct CutsceneJump jrbScene1 = { 160, jrb_area_1_spline_IntroCutscenePos,
                                  jrb_area_1_spline_IntroCutsceneFoc, &jrbScene2 };

struct CutsceneJump ccmScene4 = { 150, ccm_area_1_spline_IntroCutscenePos_003,
                                  ccm_area_1_spline_IntroCutsceneFoc_003, 0 };
struct CutsceneJump ccmScene3 = { 32, ccm_area_1_spline_IntroCutscenePos_002,
                                  ccm_area_1_spline_IntroCutsceneFoc_002, &ccmScene4 };
struct CutsceneJump ccmScene2 = { 33, ccm_area_1_spline_IntroCutscenePos_001,
                                  ccm_area_1_spline_IntroCutsceneFoc_001, &ccmScene3 };
struct CutsceneJump ccmScene1 = { 33, ccm_area_1_spline_IntroCutscenePos,
                                  ccm_area_1_spline_IntroCutsceneFoc, &ccmScene2 };

// show ship from front, teeth showing, slight pan
// show ship from side, show moon and oceanwater
// captain playing his piano, zoom out afterwards
extern struct CutsceneSplinePoint bbh_area_1_spline_IntroCutsceneFoc[];
extern struct CutsceneSplinePoint bbh_area_1_spline_IntroCutsceneFoc_001[];
extern struct CutsceneSplinePoint bbh_area_1_spline_IntroCutsceneFoc_002[];
extern struct CutsceneSplinePoint bbh_area_1_spline_IntroCutscenePos[];
extern struct CutsceneSplinePoint bbh_area_1_spline_IntroCutscenePos_001[];
extern struct CutsceneSplinePoint bbh_area_1_spline_IntroCutscenePos_002[];
struct CutsceneJump bbhScene3 = { 180, bbh_area_1_spline_IntroCutscenePos_002,
                                  bbh_area_1_spline_IntroCutsceneFoc_002, 0 };
struct CutsceneJump bbhScene2 = { 95, bbh_area_1_spline_IntroCutscenePos_001,
                                  bbh_area_1_spline_IntroCutsceneFoc_001, &bbhScene3 };
struct CutsceneJump bbhScene1 = { 105, bbh_area_1_spline_IntroCutscenePos,
                                  bbh_area_1_spline_IntroCutsceneFoc, &bbhScene2 };

extern struct CutsceneSplinePoint ending_area_1_spline_IntroCutsceneFoc[];
extern struct CutsceneSplinePoint ending_area_1_spline_IntroCutscenePos[];
extern struct CutsceneSplinePoint ending_area_1_spline_IntroCutsceneFoc_001[];
extern struct CutsceneSplinePoint ending_area_1_spline_IntroCutscenePos_001[];
struct CutsceneJump introScene1 = { 300, ending_area_1_spline_IntroCutscenePos,
                                    ending_area_1_spline_IntroCutsceneFoc, 0 };
struct CutsceneJump introScene2 = { 55, ending_area_1_spline_IntroCutscenePos_001,
                                    ending_area_1_spline_IntroCutsceneFoc_001, &introScene1 };

extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutsceneFoc_004[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutscenePos_004[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutsceneFoc_003[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutscenePos_003[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutsceneFoc_002[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutscenePos_002[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutsceneFoc_001[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutscenePos_001[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutsceneFoc[];
extern struct CutsceneSplinePoint castle_grounds_area_1_spline_IntroCutscenePos[];

struct CutsceneJump castleGroundScene6 = { 315, castle_grounds_area_1_spline_IntroCutscenePos_005,
                                           castle_grounds_area_1_spline_IntroCutsceneFoc_005, 0 };
struct CutsceneJump castleGroundScene5 = { 275, castle_grounds_area_1_spline_IntroCutscenePos_004,
                                           castle_grounds_area_1_spline_IntroCutsceneFoc_004,
                                           &castleGroundScene6 };
struct CutsceneJump castleGroundScene4 = { 115, castle_grounds_area_1_spline_IntroCutscenePos_003,
                                           castle_grounds_area_1_spline_IntroCutsceneFoc_003,
                                           &castleGroundScene5 };
struct CutsceneJump castleGroundScene3 = { 300, castle_grounds_area_1_spline_IntroCutscenePos_002,
                                           castle_grounds_area_1_spline_IntroCutsceneFoc_002,
                                           &castleGroundScene4 };
struct CutsceneJump castleGroundScene2 = { 240, castle_grounds_area_1_spline_IntroCutscenePos_001,
                                           castle_grounds_area_1_spline_IntroCutsceneFoc_001,
                                           &castleGroundScene3 };
struct CutsceneJump castleGroundScene1 = { 400, castle_grounds_area_1_spline_IntroCutscenePos,
                                           castle_grounds_area_1_spline_IntroCutsceneFoc,
                                           &castleGroundScene2 };


#include "levels/hmc/header.h"
struct CutsceneJump showcaseScene8 = { 0xFFFFFFFF, hmc_area_1_spline_APos8,
                                           hmc_area_1_spline_AFoc8,
                                           0 };
struct CutsceneJump showcaseScene7 = { 110, hmc_area_1_spline_APos7,
                                           hmc_area_1_spline_AFoc7,
                                           &showcaseScene8 };
struct CutsceneJump showcaseScene6 = { 100, hmc_area_1_spline_APos6,
                                           hmc_area_1_spline_AFoc6,
                                           &showcaseScene7 };
struct CutsceneJump showcaseScene5 = { 91, hmc_area_1_spline_APos5,
                                           hmc_area_1_spline_AFoc5,
                                           &showcaseScene6 };
struct CutsceneJump showcaseScene4 = { 145, hmc_area_1_spline_APos4,
                                           hmc_area_1_spline_AFoc4,
                                           &showcaseScene5 };
struct CutsceneJump showcaseScene3 = { 92, hmc_area_1_spline_APos3,
                                           hmc_area_1_spline_AFoc3,
                                           &showcaseScene4 };
struct CutsceneJump showcaseScene2 = { 120, hmc_area_1_spline_APos2,
                                           hmc_area_1_spline_AFoc2,
                                           &showcaseScene3 };
struct CutsceneJump showcaseScene = { 120, hmc_area_1_spline_APos1,
                                           hmc_area_1_spline_AFoc1,
                                           &showcaseScene2 };



struct CutsceneJump *LevelIntros[40] = {
    0, 0,          0,         0,           &bbhScene1, &ccmScene1,          0, &showcaseScene, 0, &bobScene1, 0,
    0, &jrbScene1, 0,         0,           0,          &castleGroundScene1, 0, 0, 0, 0,          0,
    0, 0,          &wfScene1, &introScene2

};
void spawnCutsceneStuff() { // ERROR one of these objects has their nextOBJ set to 0?
    struct Object *bowser;
    struct Object *temp;
    bowser = spawn_object_abs_with_rot(gMarioObject, 0, MODEL_BOWSER, bhvCutsceneBowser, -303, 4369,
                                       1955, 0, 116 * 0x8000 / 180, 0);
    bowser->oObjPointer2 = spawn_object_abs_with_rot(gMarioObject, 0, 0x36, bhvCutsceneObject, -1017,
                                                     3389, 2301, 0, 116 * 0x8000 / 180, 0);
    bowser->oObjPointer1 = spawn_object_abs_with_rot(gMarioObject, 0, 0x37, bhvCutsceneObject, -1017,
                                                     3389, 2301, 0, 116 * 0x8000 / 180, 0);
}

void reset_scene(struct Camera *c) {
    currentScene = 0;
    timeAccumulated = 0;
    gPlayerCameraState.cameraEvent = 0;
    gMarioState->action = gMarioState->prevAction;
    gMarioState->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    if (gCurrLevelNum == LEVEL_JRB) {
        gLakituState.pos[0] = gMarioState->pos[0];
        gLakituState.pos[1] = gMarioState->pos[1];
        gLakituState.pos[2] = gMarioState->pos[2];
        c->pos[0] = gMarioState->pos[0];
        c->pos[1] = gMarioState->pos[1];
        c->pos[2] = gMarioState->pos[2];
        gLakituState.curPos[0] = gMarioState->pos[0];
        gLakituState.curPos[1] = gMarioState->pos[1];
        gLakituState.curPos[2] = gMarioState->pos[2];
        gLakituState.goalPos[0] = gMarioState->pos[0];
        gLakituState.goalPos[1] = gMarioState->pos[1];
        gLakituState.goalPos[2] = gMarioState->pos[2];

        gLakituState.focus[0] = gMarioState->pos[0];
        gLakituState.focus[1] = gMarioState->pos[1];
        gLakituState.focus[2] = gMarioState->pos[2];
        c->focus[0] = gMarioState->pos[0];
        c->focus[1] = gMarioState->pos[1];
        c->focus[2] = gMarioState->pos[2];
        gLakituState.curFocus[0] = gMarioState->pos[0];
        gLakituState.curFocus[1] = gMarioState->pos[1];
        gLakituState.curFocus[2] = gMarioState->pos[2];
        gLakituState.goalFocus[0] = gMarioState->pos[0];
        gLakituState.goalFocus[1] = gMarioState->pos[1];
        gLakituState.goalFocus[2] = gMarioState->pos[2];
    }
}
u16 viewportClipSize = 0;
extern Vp sEndCutsceneVp;
void intro_cutscenes(struct Camera *c) {
    struct CutsceneSplinePoint *focus, *pos;
    int l;
    l = (timer + timeAccumulated) * 8 + 10;
    if (l > 120) {
        l = 120;
    }
    viewportClipSize = l;

    if (!currentScene) {
        currentScene = LevelIntros[gCurrLevelNum];
        if (!currentScene) {
            reset_scene(c);
            return;
        }
        if (gCurrLevelNum == LEVEL_CASTLE_GROUNDS) {
            spawnCutsceneStuff();
        }
    }
    if (!timer) {
        cutscene_event(cutscene_reset_spline, c, 0, 0);
        pos = currentScene->pos;
        focus = currentScene->foc;
        length = currentScene->length;
        pos = segmented_to_virtual(pos);
        focus = segmented_to_virtual(focus);
        copy_spline_segment(sCurCreditsSplinePos, pos);
        copy_spline_segment(sCurCreditsSplineFocus, focus);
        sCutsceneSplineSegment = 0;
        sCutsceneSplineSegmentProgress = 0;
        gLakituState.focHSpeed =1.f;
        gLakituState.posHSpeed =1.f;
    }

    move_point_along_spline(c->pos, sCurCreditsSplinePos, &sCutsceneSplineSegment,
                            &sCutsceneSplineSegmentProgress);
    move_point_along_spline(c->focus, sCurCreditsSplineFocus, &sCutsceneSplineSegment,
                            &sCutsceneSplineSegmentProgress);
    if (timer++ == length) {
        timeAccumulated += timer;
        currentScene = currentScene->next;
        timer = 0;
        if (!currentScene) {
            reset_scene(c);
        }
    }
}