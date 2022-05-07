#include "texscroll.h"
#include <ultra64.h>

#include "sm64.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "level_update.h"
#include "game_init.h"
#include "level_update.h"
#include "main.h"
#include "engine/math_util.h"
#include "engine/graph_node.h"
#include "area.h"
#include "save_file.h"
#include "sound_init.h"
#include "mario.h"
#include "camera.h"
#include "object_list_processor.h"
#include "ingame_menu.h"
#include "obj_behaviors.h"
#include "save_file.h"
#include "debug_course.h"
#include "level_table.h"
#include "course_table.h"
#include "thread6.h"
#include "water.h"
#include "OPT_FOR_SIZE.h"

#define PLAY_MODE_NORMAL 0
#define PLAY_MODE_PAUSED 2
#define PLAY_MODE_CHANGE_AREA 3
#define PLAY_MODE_CHANGE_LEVEL 4

#define WARP_TYPE_NOT_WARPING 0
#define WARP_TYPE_CHANGE_LEVEL 1
#define WARP_TYPE_CHANGE_AREA 2
#define WARP_TYPE_SAME_AREA 3

#define WARP_NODE_F0 0xF0
#define WARP_NODE_DEATH 0xF1
#define WARP_NODE_F2 0xF2
#define WARP_NODE_WARP_FLOOR 0xF3
#define WARP_NODE_CREDITS_START 0xF8
#define WARP_NODE_CREDITS_NEXT 0xF9
#define WARP_NODE_CREDITS_END 0xFA

#define WARP_NODE_CREDITS_MIN 0xF8

struct MarioState gMarioStates[1];
struct HudDisplay gHudDisplay;
s16 sCurrPlayMode;
s16 sTransitionTimer;
void (*sTransitionUpdate)(s16 *);
struct WarpDest sWarpDest;
s16 D_80339EE0;
s16 sDelayedWarpOp;
s16 sDelayedWarpTimer;
s16 sSourceWarpNodeId;
s32 sDelayedWarpArg;

struct MarioState *gMarioState = &gMarioStates[0];
s8 D_8032C9E0 = 0;

__attribute__((always_inline)) inline u32 pressed_pause(void) {
    u32 val4 = get_dialog_id() >= 0;
    u32 intangible = (gMarioState->action & ACT_FLAG_INTANGIBLE) != 0;

    if (!intangible && !val4 && !gWarpTransition.isActive && sDelayedWarpOp == WARP_OP_NONE
        && (gPlayer1Controller->buttonPressed & START_BUTTON)) {
        return TRUE;
    }

    return FALSE;
}

__attribute__((always_inline)) inline void set_play_mode(s32 playMode) {
    sCurrPlayMode = playMode;
}

void warp_special(s32 arg) {
    sCurrPlayMode = PLAY_MODE_CHANGE_LEVEL;
    D_80339EE0 = arg;
}

void fade_into_special_warp(u32 arg, u32 color) {
    /*if (color != 0) {
        color = 0xFF;
    }*/

    fadeout_music(190);
    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x10, color, color, color);
    level_set_transition(30, NULL);

    warp_special(arg);
}
void init_door_warp(struct SpawnInfo *spawnInfo, u32 arg1) {
    if (arg1 & 0x00000002) {
        spawnInfo->startAngle[1] += 0x8000;
    }

    spawnInfo->startPos[0] += 300.0f * sins(spawnInfo->startAngle[1]);
    spawnInfo->startPos[2] += 300.0f * coss(spawnInfo->startAngle[1]);
}
/*
void set_mario_initial_cap_powerup(struct MarioState *m) {
    u32 capCourseIndex = gCurrCourseNum - COURSE_CAP_COURSES;

    switch (capCourseIndex) {
        case COURSE_COTMC - COURSE_CAP_COURSES:
            // give mario cloud cap
            break;

        case COURSE_TOTWC - COURSE_CAP_COURSES:
            m->flags |= MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
            m->capTimer = 1200;
            break;

        case COURSE_VCUTM - COURSE_CAP_COURSES:
            // give mario freeze cap
            break;
    }
}*/

void set_mario_initial_action(struct MarioState *m, u32 spawnType, u32 actionArg) {
    switch (spawnType) {
        case MARIO_SPAWN_DOOR_WARP:
            set_mario_action(m, ACT_WARP_DOOR_SPAWN, actionArg);
            break;
        case MARIO_SPAWN_UNKNOWN_02:
            set_mario_action(m, ACT_IDLE, 0);
            break;
        case MARIO_SPAWN_UNKNOWN_03:
            set_mario_action(m, ACT_EMERGE_FROM_PIPE, 0);
            break;
        case MARIO_SPAWN_TELEPORT:
            set_mario_action(m, ACT_TELEPORT_FADE_IN, 0);
            break;
        case MARIO_SPAWN_INSTANT_ACTIVE:
            set_mario_action(m, ACT_IDLE, 0);
            break;
        case MARIO_SPAWN_AIRBORNE:
            set_mario_action(m, ACT_SPAWN_NO_SPIN_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_HARD_AIR_KNOCKBACK:
            set_mario_action(m, ACT_HARD_BACKWARD_AIR_KB, 0);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE:
            set_mario_action(m, ACT_SPAWN_SPIN_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_DEATH:
            set_mario_action(m, ACT_FALLING_DEATH_EXIT, 0);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE:
            set_mario_action(m, ACT_SPAWN_SPIN_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_FLYING:
            set_mario_action(m, ACT_SHOT_FROM_CANNON, 2);
            m->vel[1] = 30.f;
            m->forwardVel = 70.f;
            break;
        case MARIO_SPAWN_SWIMMING:
            set_mario_action(m, ACT_WATER_IDLE, 1);
            break;
        case MARIO_SPAWN_PAINTING_STAR_COLLECT:
            set_mario_action(m, ACT_EXIT_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_PAINTING_DEATH:
            set_mario_action(m, ACT_DEATH_EXIT, 0);
            break;
        case MARIO_SPAWN_AIRBORNE_STAR_COLLECT:
            set_mario_action(m, ACT_FALLING_EXIT_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_AIRBORNE_DEATH:
            set_mario_action(m, ACT_UNUSED_DEATH_EXIT, 0);
            break;
        case MARIO_SPAWN_LAUNCH_STAR_COLLECT:
            set_mario_action(m, ACT_SPECIAL_EXIT_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_LAUNCH_DEATH:
            set_mario_action(m, ACT_SPECIAL_DEATH_EXIT, 0);
            break;
    }

    //  set_mario_initial_cap_powerup(m);
}

extern s16 newcam_yaw;
void init_mario_after_warp(void) {
    struct ObjectWarpNode *spawnNode = area_get_warp_node(sWarpDest.nodeId);
    u32 marioSpawnType = get_mario_spawn_type(spawnNode->object);

    if (gMarioState->action != ACT_UNINITIALIZED) {
        gPlayerSpawnInfos[0].startPos[0] = (s16) spawnNode->object->oPosX;
        gPlayerSpawnInfos[0].startPos[1] = (s16) spawnNode->object->oPosY;
        gPlayerSpawnInfos[0].startPos[2] = (s16) spawnNode->object->oPosZ;

        gPlayerSpawnInfos[0].startAngle[0] = 0;
        gPlayerSpawnInfos[0].startAngle[1] = spawnNode->object->oMoveAngleYaw;
        gPlayerSpawnInfos[0].startAngle[2] = 0;

        if (marioSpawnType == MARIO_SPAWN_DOOR_WARP) {
            /*  if (sWarpDest.arg & 0x00000002) {
                  gPlayerSpawnInfos[0].startAngle[1] += 0x8000;
              }*/
            gPlayerSpawnInfos[0].startPos[0] += 120.0f * sins(gPlayerSpawnInfos[0].startAngle[1]);
            gPlayerSpawnInfos[0].startPos[2] += 120.0f * coss(gPlayerSpawnInfos[0].startAngle[1]);
        }

        if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL || sWarpDest.type == WARP_TYPE_CHANGE_AREA) {
            gPlayerSpawnInfos[0].areaIndex = sWarpDest.areaIdx;
            load_mario_area();
        }

        init_mario();
        set_mario_initial_action(gMarioState, marioSpawnType, sWarpDest.arg);

        gMarioState->interactObj = spawnNode->object;
        gMarioState->usedObj = spawnNode->object;
        gMarioState->wall = 0;
    }

    reset_camera(gCurrentArea->camera);
    if (spawnNode && (spawnNode->object)) {
        newcam_yaw = ((spawnNode->object->oBehParams & 0xFF000000) >> 16); // puppycam newcam
    } else {
        newcam_yaw = 0;
    }
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;

    switch (marioSpawnType) {
        case MARIO_SPAWN_UNKNOWN_03:
            play_transition(WARP_TRANSITION_FADE_FROM_STAR, 0x10, 0x00, 0x00, 0x00);
            break;
        case MARIO_SPAWN_DOOR_WARP:
            play_transition(WARP_TRANSITION_FADE_FROM_CIRCLE, 0x10, 0x00, 0x00, 0x00);
            break;
        case MARIO_SPAWN_TELEPORT:
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x14, 0xFF, 0xFF, 0xFF);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE:
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x1A, 0xFF, 0xFF, 0xFF);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE:
            play_transition(WARP_TRANSITION_FADE_FROM_CIRCLE, 0x10, 0x00, 0x00, 0x00);
            break;
        case MARIO_SPAWN_UNKNOWN_27:
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x10, 0x00, 0x00, 0x00);
            break;
        default:
            play_transition(WARP_TRANSITION_FADE_FROM_STAR, 0x10, 0x00, 0x00, 0x00);
            break;
    }

    set_background_music(gCurrentArea->musicParam, gCurrentArea->musicParam2);

    /*if (gMarioState->flags & (MARIO_WING_CAP)) {
        play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP));
    }*/
}

// used for warps inside one level
void warp_area(void) {
    if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        if (sWarpDest.type == WARP_TYPE_CHANGE_AREA) {
            unload_mario_area();
            load_area(sWarpDest.areaIdx);
        }

        init_mario_after_warp();
    }
}

// used for warps between levels
void warp_level(void) {
    gCurrLevelNum = sWarpDest.levelNum;

    load_area(sWarpDest.areaIdx);
    init_mario_after_warp();
}
/*
void warp_credits(void) {
}*/

void check_instant_warp(void) {
    s32 cameraAngle;
    struct Surface *floor = gMarioState->floor;

    // if ((floor = gMarioState->floor) != NULL) {
    s32 index = SURFACETYPE(floor) - SURFACE_INSTANT_WARP_1B;
    if (index >= INSTANT_WARP_INDEX_START && index < INSTANT_WARP_INDEX_STOP) {
        struct InstantWarp *warp = &gCurrentArea->instantWarps[index];
        if (gCurrLevelNum == LEVEL_JRB && (gMarioState->pos[1] > 1500.f)) {
            return;
        }

        if (warp->id != 0) {
            gMarioState->pos[0] += warp->displacement[0];
            gMarioState->pos[1] += warp->displacement[1];
            gMarioState->pos[2] += warp->displacement[2];
            gMarioState->peakHeight += warp->displacement[1];
            gMarioState->marioObj->oPosX = gMarioState->pos[0];
            gMarioState->marioObj->oPosY = gMarioState->pos[1];
            gMarioState->marioObj->oPosZ = gMarioState->pos[2];

            cameraAngle = gMarioState->area->camera->yaw;

            change_area(warp->area);
            gMarioState->area = gCurrentArea;

            warp_camera(warp->displacement[0], warp->displacement[1], warp->displacement[2]);

            gMarioState->area->camera->yaw = cameraAngle;
        }
    }
    // }
}

s32 music_changed_through_warp(s32 arg) {
    struct ObjectWarpNode *warpNode = area_get_warp_node(arg);
    s32 levelNum = warpNode->node.destLevel & 0x7F;
    u16 val8 = gAreas[warpNode->node.destArea].musicParam;
    u16 val6 = gAreas[warpNode->node.destArea].musicParam2;

    return levelNum == gCurrLevelNum && val8 == gCurrentArea->musicParam
           && val6 == gCurrentArea->musicParam2 && (get_current_background_music() == val6);
}

/**
 * Set the current warp type and destination level/area/node.
 */
void initiate_warp(s32 destLevel, s32 destArea, s32 destWarpNode, s32 arg3) {
    s32 currCourseNum = gLevelToCourseNumTable[(destLevel & 0x7F) - 1];
    if (destWarpNode >= WARP_NODE_CREDITS_MIN) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (destLevel != gCurrLevelNum) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (destArea != gCurrentArea->index) {
        sWarpDest.type = WARP_TYPE_CHANGE_AREA;
    } else {
        sWarpDest.type = WARP_TYPE_SAME_AREA;
    }

    sWarpDest.levelNum = destLevel;
    sWarpDest.areaIdx = destArea;
    sWarpDest.nodeId = destWarpNode;
    sWarpDest.arg = arg3;

    if ((sWarpDest.levelNum == gWarpCheckpoint.levelID) && (sWarpDest.levelNum != gCurrLevelNum)) {
        if (gWarpCheckpoint.courseNum != COURSE_NONE && gSavedCourseNum == currCourseNum
            && gWarpCheckpoint.actNum == gCurrActNum) {
            sWarpDest.areaIdx = gWarpCheckpoint.areaNum;
            sWarpDest.nodeId = gWarpCheckpoint.warpNode;
            D_8032C9E0 = TRUE;
        }
    }
}

// From Surface 0xD3 to 0xFC
#define PAINTING_WARP_INDEX_START 0x00 // Value greater than or equal to Surface 0xD3
#define PAINTING_WARP_INDEX_FA 0x2A    // THI Huge Painting index left
#define PAINTING_WARP_INDEX_END 0x2D   // Value less than Surface 0xFD

/**
 * If there is not already a delayed warp, schedule one. The source node is
 * based on the warp operation and sometimes Mario's used object.
 * Return the time left until the delayed warp is initiated.
 */
s32 level_trigger_warp(struct MarioState *m, s32 warpOp) {
    s32 val04 = TRUE;

    if (sDelayedWarpOp == WARP_OP_NONE) {
        m->invincTimer = -1;
        sDelayedWarpArg = 0;
        sDelayedWarpOp = warpOp;

        switch (warpOp) {
            case WARP_OP_DEMO_NEXT:
            case WARP_OP_DEMO_END:
                do {
                    sDelayedWarpTimer = 20;
                } while (0);
                sSourceWarpNodeId = WARP_NODE_F0;
                gSavedCourseNum = COURSE_NONE;
                val04 = FALSE;
                play_transition(WARP_TRANSITION_FADE_INTO_STAR, 0x14, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_END:
                sDelayedWarpTimer = 60;
                sSourceWarpNodeId = WARP_NODE_F0;
                val04 = FALSE;
                gSavedCourseNum = COURSE_NONE;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x3C, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_STAR_EXIT:
                sDelayedWarpTimer = 32;
                sSourceWarpNodeId = WARP_NODE_F0;
                gSavedCourseNum = COURSE_NONE;
                play_transition(WARP_TRANSITION_FADE_INTO_MARIO, 0x20, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_DEATH:
                sDelayedWarpTimer = 48;
                sSourceWarpNodeId = WARP_NODE_DEATH;
                play_transition(WARP_TRANSITION_FADE_INTO_BOWSER, 0x30, 0x00, 0x00, 0x00);
                play_sound(SOUND_MENU_BOWSER_LAUGH, gDefaultSoundArgs);
                break;

            case WARP_OP_WARP_FLOOR:
                sSourceWarpNodeId = WARP_NODE_WARP_FLOOR;
                if (area_get_warp_node(sSourceWarpNodeId) == NULL) {
                    sSourceWarpNodeId = WARP_NODE_DEATH;
                }
                sDelayedWarpTimer = 20;
                play_transition(WARP_TRANSITION_FADE_INTO_CIRCLE, 0x14, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_UNKNOWN_01: // enter totwc
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = WARP_NODE_F2;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x1E, 0xFF, 0xFF, 0xFF);
#ifndef VERSION_JP
                play_sound(SOUND_MENU_STAR_SOUND, gDefaultSoundArgs);
#endif
                break;

            case WARP_OP_UNKNOWN_02: // bbh enter
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = (m->usedObj->oBehParams & 0x00FF0000) >> 16;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x1E, 0xFF, 0xFF, 0xFF);
                break;

            case WARP_OP_TELEPORT:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = (m->usedObj->oBehParams & 0x00FF0000) >> 16;
                val04 = !music_changed_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x14, 0xFF, 0xFF, 0xFF);
                break;

            case WARP_OP_WARP_DOOR:
                sDelayedWarpTimer = 20;
                sDelayedWarpArg = m->actionArg;
                sSourceWarpNodeId = (m->usedObj->oBehParams & 0x00FF0000) >> 16;
                val04 = !music_changed_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_CIRCLE, 0x14, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_WARP_OBJECT:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = (m->usedObj->oBehParams & 0x00FF0000) >> 16;
                val04 = !music_changed_through_warp(sSourceWarpNodeId);
                if ((m->usedObj->oBehParams & 0x04)) {
                    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x14, 0xFF, 0xFF, 0xFF);
                } else {
                    play_transition(WARP_TRANSITION_FADE_INTO_STAR, 0x14, 0x00, 0x00, 0x00);
                }
                break;

            case WARP_OP_CREDITS_START:
                sDelayedWarpTimer = 30;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x1E, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_NEXT:

            case 0x20:
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = (m->usedObj->oBehParams & 0x00FF0000) >> 16;
                val04 = !music_changed_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x18, 0x00, 0x00, 0x00);
                break;
        }

        if (val04) {
            fadeout_music((3 * sDelayedWarpTimer / 2) * 8 - 2);
        }
    }

    return sDelayedWarpTimer;
}

/**
 * If a delayed warp is ready, initiate it.
 */
__attribute__((always_inline)) inline void initiate_delayed_warp(void) {
    struct ObjectWarpNode *warpNode;
    s32 destWarpNode;

    if (sDelayedWarpOp != WARP_OP_NONE && --sDelayedWarpTimer == 0) {
        reset_dialog_render_state();

        switch (sDelayedWarpOp) {
            case WARP_OP_GAME_OVER:
                save_file_reload();
                warp_special(-3);
                break;

            case WARP_OP_CREDITS_END:
                warp_special(-1);
                sound_banks_enable(0x03F0);
                break;

            case WARP_OP_CREDITS_START:
                break;

            case WARP_OP_CREDITS_NEXT:
                break;

            default:
                warpNode = area_get_warp_node(sSourceWarpNodeId);

                initiate_warp(warpNode->node.destLevel & 0x7F, warpNode->node.destArea,
                              warpNode->node.destNode, sDelayedWarpArg);

                check_if_should_set_warp_checkpoint(&warpNode->node);
                if (sWarpDest.type != WARP_TYPE_CHANGE_LEVEL) {
                    level_set_transition(2, NULL);
                }
                break;
        }
    }
}

__attribute__((always_inline)) inline void update_hud_values(void) {
    s32 numHealthWedges = gMarioState->health > 0 ? gMarioState->health >> 8 : 0;

    if (gHudDisplay.coins < gMarioState->numCoins) {
        if (gGlobalTimer & 0x00000001) {
            u32 coinSound;
            if (gMarioState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
                coinSound = SOUND_GENERAL_COIN_WATER;
            } else {
                coinSound = SOUND_GENERAL_COIN;
            }

            gHudDisplay.coins += 1;
            play_sound(coinSound, gMarioState->marioObj->header.gfx.cameraToObject);
        }
    }

    if (numHealthWedges > gHudDisplay.wedges) {
        play_sound(SOUND_MENU_POWER_METER, gDefaultSoundArgs);
    }
    gHudDisplay.wedges = numHealthWedges;

    if (gMarioState->hurtCounter > 0) {
        gHudDisplay.flags |= HUD_DISPLAY_FLAG_EMPHASIZE_POWER;
    } else {
        gHudDisplay.flags &= ~HUD_DISPLAY_FLAG_EMPHASIZE_POWER;
    }
}

/**
 * Update objects, HUD, and camera. This update function excludes things like
 * endless staircase, warps, pausing, etc. This is used when entering a painting,
 * presumably to allow painting and camera updating while avoiding triggering the
 * warp twice.
 */

__attribute__((always_inline)) inline s32 play_mode_normal(void) {
    int i = 0;

    warp_area();
    check_instant_warp();
    area_update_objects();
    if (gCurrentArea != NULL) {
        update_camera(gCurrentArea->camera);
    }
    update_hud_values();

    initiate_delayed_warp();

    // If either initiate_painting_warp or initiate_delayed_warp initiated a
    // warp, change play mode accordingly.
    if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL) {
        set_play_mode(PLAY_MODE_CHANGE_LEVEL);
    } else if (sTransitionTimer != 0) {
        set_play_mode(PLAY_MODE_CHANGE_AREA);
    } else if (pressed_pause()) {
        lower_background_noise(1);
#ifdef VERSION_SH
        cancel_rumble();
#endif
        gCameraMovementFlags |= CAM_MOVE_PAUSE_SCREEN;
        set_play_mode(PLAY_MODE_PAUSED);
    }

    return 0;
}
extern f32 backupPos[3];
__attribute__((always_inline)) inline s32 play_mode_paused(void) {
    if (gPauseScreenMode == 0) {
        set_menu_mode(RENDER_PAUSE_SCREEN);
    } else if (gPauseScreenMode == 1) {
        raise_background_noise(1);
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        set_play_mode(PLAY_MODE_NORMAL);
    } else {
        // Exit level
        backupPos[0] = 100.f;
        backupPos[1] = 100.f;
        backupPos[2] = 100.f;
        save_file_erase(0);
        save_file_reload();
        fade_into_special_warp(-2, 0); // reset game
                                       /*save_file_erase(1);
                                       save_file_erase(2);
                                       save_file_erase(3);*/
        /*initiate_warp(LEVEL_HMC, 1, 0x0A, 1);
        fade_into_special_warp(0, 0);*/
        gSavedCourseNum = COURSE_NONE;

        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
    }

    return 0;
}

/**
 * Set the transition, which is a period of time after the warp is initiated
 * but before it actually occurs. If updateFunction is not NULL, it will be
 * called each frame during the transition.
 */
__attribute__((always_inline)) inline void level_set_transition(s32 length,
                                                                void (*updateFunction)(s16 *)) {
    sTransitionTimer = length;
    sTransitionUpdate = updateFunction;
}

/**
 * Play the transition and then return to normal play mode.
 */
__attribute__((always_inline)) inline s32 play_mode_change_area(void) {
    //! This maybe was supposed to be sTransitionTimer == -1? sTransitionUpdate
    // is never set to -1.
    if (sTransitionUpdate == (void (*)(s16 *)) - 1) {
        update_camera(gCurrentArea->camera);
    } else if (sTransitionUpdate != NULL) {
        sTransitionUpdate(&sTransitionTimer);
    }

    if (sTransitionTimer > 0) {
        sTransitionTimer -= 1;
    }

    //! If sTransitionTimer is -1, this will miss.
    if (sTransitionTimer == 0) {
        sTransitionUpdate = NULL;
        set_play_mode(PLAY_MODE_NORMAL);
    }

    return 0;
}

/**
 * Play the transition and then return to normal play mode.
 */
__attribute__((always_inline)) inline s32 play_mode_change_level(void) {
    if (sTransitionUpdate != NULL) {
        sTransitionUpdate(&sTransitionTimer);
    }

    //! If sTransitionTimer is -1, this will miss.
    if (--sTransitionTimer == -1) {
        sTransitionTimer = 0;
        sTransitionUpdate = NULL;

        if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
            return sWarpDest.levelNum;
        } else {
            return D_80339EE0;
        }
    }

    return 0;
}
#include "buffers/buffers.h"
extern u32 gametime;
extern u8 volGoalMaybe;
__attribute__((always_inline)) inline s32 update_level(void) {
    s32 changeLevel;

    switch (sCurrPlayMode) {
        case PLAY_MODE_NORMAL:
            if (gCurrLevelNum != LEVEL_PSS) {
                gSaveBuffer.files[0][0].savestateTimer++;
            }
            changeLevel = play_mode_normal();
            scroll_textures();
            water_scroll(); // CuckyDev: Scroll water
            break;
        case PLAY_MODE_PAUSED:
            gSaveBuffer.files[0][0].savestateTimer++;
            read_controller_inputs();
            changeLevel = play_mode_paused();
            break;
        case PLAY_MODE_CHANGE_AREA:
            // read_controller_inputs();
            changeLevel = play_mode_change_area();
            break;
        case PLAY_MODE_CHANGE_LEVEL:
            // read_controller_inputs();
            changeLevel = play_mode_change_level();
            break;
    }

    if (changeLevel) {
        volGoalMaybe = 0;
        enable_background_sound();
    }

    return changeLevel;
}
extern void setLightDirection(u8 x, u8 y, u8 z);
extern Lights1 *lightsIn[];
extern Lights1 *lightsOut[];
extern Gfx mat_mario_body[];
extern Gfx swappied[];
void reset_mario_colors() { // reset lights
    Lights1 *i;
    Lights1 *j;
    int k;
    Gfx *a = segmented_to_virtual(mat_mario_body);
    for (k = 0; k < 15; k++) {
        i = (Lights1 *) (lightsIn[k]);
        j = (Lights1 *) segmented_to_virtual((int) lightsOut[k] & 0x7fFFFFFF);
        j->a.l.col[0] = i->a.l.col[0];
        j->a.l.col[1] = i->a.l.col[1];
        j->a.l.col[2] = i->a.l.col[2];
        j->a.l.colc[0] = j->a.l.col[0];
        j->a.l.colc[1] = j->a.l.col[1];
        j->a.l.colc[2] = j->a.l.col[2];

        j->l->l.col[0] = i->l->l.col[0];
        j->l->l.col[1] = i->l->l.col[1];
        j->l->l.col[2] = i->l->l.col[2];
        j->l->l.colc[0] = j->l->l.col[0];
        j->l->l.colc[1] = j->l->l.col[1];
        j->l->l.colc[2] = j->l->l.col[2];
    }
    setLightDirection(0x28, 0x28, 0x28);
    a[4] = swappied[0];  // palete
    a[11] = swappied[1]; // texture
}

extern void reset_scene(struct Camera *c);
__attribute__((always_inline)) inline s32 init_level(void) {
    s32 val4 = 0;
    // custom reset stuff
    if (gMarioState) {
        if (gMarioState->area) {
            if (gMarioState->area->camera) {
                reset_scene(gMarioState->area->camera);
            }
        }
    }
    set_play_mode(PLAY_MODE_NORMAL);

    sDelayedWarpOp = WARP_OP_NONE;
    sTransitionTimer = 0;
    D_80339EE0 = 0;

    gHudDisplay.flags = HUD_DISPLAY_DEFAULT;

    if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        /*   if (sWarpDest.nodeId >= WARP_NODE_CREDITS_MIN) {
               warp_credits();
           } else {*/
        warp_level();
        //}
    } else {
        if (gPlayerSpawnInfos[0].areaIndex >= 0) {
            load_mario_area();
            init_mario();
        }

        if (gCurrentArea != NULL) {
            reset_camera(gCurrentArea->camera);

            if (gMarioState->action != ACT_UNINITIALIZED) {
                set_mario_action(gMarioState, ACT_IDLE, 0);
            }
        }

        if (val4 != 0) {
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x5A, 0xFF, 0xFF, 0xFF);
        } else {
            play_transition(WARP_TRANSITION_FADE_FROM_STAR, 0x10, 0xFF, 0xFF, 0xFF);
        }

        set_background_music(gCurrentArea->musicParam, gCurrentArea->musicParam2);
    }
#ifdef VERSION_SH
    cancel_rumble();
#endif

    if (gMarioState->action == ACT_INTRO_CUTSCENE) {
        sound_banks_disable(0x0330);
    }

    return 1;
}

int AreaTimer;
int LevelTimer;
int AreaID;
int levelID;
s8 missionActive = -1;
/**
 * Initialize the current level if initOrUpdate is 0, or update the level if it is 1.
 */
Gfx yoshistarColors[12] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),         gsDPSetPrimColor(0, 0, 127, 254, 127, 255),
    gsDPSetPrimColor(0, 0, 50, 50, 50, 255),    gsDPSetPrimColor(0, 0, 127, 127, 254, 255),
    gsDPSetPrimColor(0, 0, 0, 254, 254, 255),   gsDPSetPrimColor(0, 0, 254, 127, 0, 255),
    gsDPSetPrimColor(0, 0, 254, 0, 254, 255),   gsDPSetPrimColor(0, 0, 127, 0, 127, 255),
    gsDPSetPrimColor(0, 0, 254, 127, 127, 255), gsDPSetPrimColor(0, 0, 254, 254, 200, 255),
    gsDPSetPrimColor(0, 0, 254, 254, 127, 255), gsDPSetPrimColor(0, 0, 254, 50, 50, 255)
};
extern Gfx mat_yoshistar_sm64_material[];     //(0x0C)
extern Gfx mat_yoshistar_sm64_material_002[]; // c
__attribute__((always_inline)) inline void colorYoshiStars() {
    Gfx *a = segmented_to_virtual(mat_yoshistar_sm64_material);
    Gfx *b = segmented_to_virtual(mat_yoshistar_sm64_material_002);
    a[0x0C] = yoshistarColors[missionActive + 1];
    b[0x0C] = yoshistarColors[missionActive + 1];
}
s8 orangeYoshApressCount = -1;
s8 orangeYoshApressCounts[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
s32 lvl_init_or_update(s16 initOrUpdate, UNUSED s32 unused) {
    s32 result = 0;
    switch (initOrUpdate) {
        case 0:
            result = init_level();
            AreaTimer = 0;
            LevelTimer = 0;
            AreaID = gCurrentArea->index;
            levelID = gCurrLevelNum;
            orangeYoshApressCount = -1;
            colorYoshiStars();
            switch (missionActive) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    orangeYoshApressCount = orangeYoshApressCounts[gCurrLevelNum];
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                    break;
                case 9:
                    break;
                case 10:
                    break;
                default:
                    break;
            }
            break;
        case 1:
            result = update_level();
            AreaTimer++;
            if (AreaID != gCurrentArea->index) {
                AreaTimer = 0;
                AreaID = gCurrentArea->index;
            }
            LevelTimer++;
            if (levelID != gCurrLevelNum) {
                LevelTimer = 0;
                AreaTimer = 0;
                levelID = gCurrLevelNum;
            }
            if (gCurrLevelNum == LEVEL_PSS) {
                obj_mark_for_deletion(gMarioObject);
            }
            break;
    }
    return result;
}

Lights1 mario_body_lights2 = gdSPDefLights1(0x0, 0x0, 0x7F, 0x0, 0x0, 0xFE, 0x28, 0x28, 0x28);
Lights1 mario_cap_lights2 = gdSPDefLights1(0x7F, 0x0, 0x0, 0xFE, 0x0, 0x0, 0x28, 0x28, 0x28);
Lights1 mario_face_0___eye_open_lights2 =
    gdSPDefLights1(0x7E, 0x60, 0x3C, 0xFD, 0xC0, 0x79, 0x28, 0x28, 0x28);
Lights1 mario_mustache_lights2 = gdSPDefLights1(0x7E, 0x60, 0x3C, 0xFD, 0xC0, 0x79, 0x28, 0x28, 0x28);
Lights1 mario_hair_lights2 = gdSPDefLights1(0x39, 0x2, 0x0, 0x73, 0x5, 0x0, 0x28, 0x28, 0x28);
Lights1 mario_sideburns_lights2 = gdSPDefLights1(0x7E, 0x60, 0x3C, 0xFD, 0xC0, 0x79, 0x28, 0x28, 0x28);
Lights1 mario_face_1___eye_half_lights2 =
    gdSPDefLights1(0x7E, 0x60, 0x3C, 0xFD, 0xC0, 0x79, 0x28, 0x28, 0x28);
Lights1 mario_face_2___eye_closed_lights2 =
    gdSPDefLights1(0x7E, 0x60, 0x3C, 0xFD, 0xC0, 0x79, 0x28, 0x28, 0x28);
Lights1 mario_face_7___eye_X_lights2 =
    gdSPDefLights1(0x7E, 0x60, 0x3C, 0xFD, 0xC0, 0x79, 0x28, 0x28, 0x28);
Lights1 mario_gloves_lights2 = gdSPDefLights1(0x7F, 0x7F, 0x7F, 0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);
Lights1 mario_shoes_lights2 = gdSPDefLights1(0x39, 0xD, 0x7, 0x72, 0x1B, 0xE, 0x28, 0x28, 0x28);

extern Lights1 mario_body_lights;
extern Lights1 mario_cap_lights;
extern Lights1 mario_face_0___eye_open_lights;
extern Lights1 mario_hair_lights;
extern Lights1 mario_mustache_lights;
extern Lights1 mario_sideburns_lights;
extern Lights1 mario_face_1___eye_half_lights;
extern Lights1 mario_face_2___eye_closed_lights;
extern Lights1 mario_face_7___eye_X_lights;
extern Lights1 mario_gloves_lights;
extern Lights1 mario_shoes_lights;

Lights1 backupLights1 = gdSPDefLights1(0x0, 0x0, 0x7F, 0x0, 0x0, 0xFE, 0x28, 0x28, 0x28);
Lights1 backupLights2 = gdSPDefLights1(0x0, 0x0, 0x7F, 0x0, 0x0, 0xFE, 0x28, 0x28, 0x28);
Lights1 backupLights3 = gdSPDefLights1(0x0, 0x0, 0x7F, 0x0, 0x0, 0xFE, 0x28, 0x28, 0x28);
extern Lights1 mario_body_lights;
extern Lights1 mario_cap_lights;

extern int stated;
extern Gfx mario_000_displaylist_mesh_layer_1_tri_1[];
extern Vtx mario_000_displaylist_mesh_layer_1_vtx_4[41];
Gfx backupHat1 = gsSPVertex(mario_000_displaylist_mesh_layer_1_vtx_4 + 0, 32, 0);
Gfx backupHat2 = gsSP2Triangles(0, 1, 2, 0, 2, 3, 0, 0);

__attribute__((always_inline)) inline void init_lights() {
    Gfx *a = (Gfx *) segmented_to_virtual(mario_000_displaylist_mesh_layer_1_tri_1);
    Lights1 *i = (Lights1 *) segmented_to_virtual(&mario_body_lights);
    Lights1 *j = (Lights1 *) segmented_to_virtual(&mario_cap_lights);
    Lights1 *k = (Lights1 *) segmented_to_virtual(&mario_shoes_lights);
    backupLights1 = *i;
    backupLights2 = *j;
    backupLights3 = *k;
    backupHat1 = a[0];
    backupHat2 = a[1];
    mario_body_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_cap_lights);
    mario_cap_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_face_0___eye_open_lights);
    mario_face_0___eye_open_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_mustache_lights);
    mario_mustache_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_hair_lights);
    mario_hair_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_sideburns_lights);
    mario_sideburns_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_face_1___eye_half_lights);
    mario_face_1___eye_half_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_face_2___eye_closed_lights);
    mario_face_2___eye_closed_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_face_7___eye_X_lights);
    mario_face_7___eye_X_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_gloves_lights);
    mario_gloves_lights2 = *i;
    i = (Lights1 *) segmented_to_virtual(&mario_shoes_lights);
    mario_shoes_lights2 = *i;
}

s32 lvl_init_from_save_file(UNUSED s16 arg0, s32 levelNum) {
    init_lights();
    stated = 0;
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;

    gCurrLevelNum = levelNum;
    gCurrCourseNum = COURSE_NONE;
    gSavedCourseNum = COURSE_NONE;

    init_mario_from_save_file();
    disable_warp_checkpoint();
    save_file_move_cap_to_default_location();

    return levelNum;
}
extern u8 prevRoom;
extern u8 curRoom;
extern s8 powerupSystem2;
s32 lvl_set_current_level(UNUSED s16 arg0, s32 levelNum) {
    s32 val4 = D_8032C9E0;
    powerupSystem2 = 0;

    D_8032C9E0 = 0;
    gCurrLevelNum = levelNum;
    gCurrCourseNum = gLevelToCourseNumTable[levelNum - 1];
    missionActive = -1;
    if (gCurrCourseNum == COURSE_NONE) {
        return 0;
    }
    gMarioState->numKeys = 0;

    if (gCurrLevelNum != LEVEL_BOWSER_1 && gCurrLevelNum != LEVEL_BOWSER_2
        && gCurrLevelNum != LEVEL_BOWSER_3) {
        prevRoom = 0;
        curRoom = 0;
        reset_mario_colors();
        gMarioState->numCoins = 0;
        gHudDisplay.coins = 0;
        gCurrCourseStarFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1);
    }

    if (gSavedCourseNum != gCurrCourseNum) {
        gSavedCourseNum = gCurrCourseNum;
        disable_warp_checkpoint();
    }

    if (gCurrCourseNum > COURSE_STAGES_MAX || val4 != 0) {
        return 0;
    }

    return 1;
}
void init_lights();