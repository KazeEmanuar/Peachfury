#include <PR/ultratypes.h>

#include "prevent_bss_reordering.h"
#include "sm64.h"
#include "area.h"
#include "audio/data.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "camera.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "gfx_dimensions.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_actions_moving.h"
#include "mario_step.h"
#include "moving_texture.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "thread6.h"
#include "OPT_FOR_SIZE.h"

// TODO: put this elsewhere
enum SaveOption { SAVE_OPT_SAVE_AND_CONTINUE = 1, SAVE_OPT_SAVE_AND_QUIT, SAVE_OPT_CONTINUE_DONT_SAVE };

static struct Object *sIntroWarpPipeObj;
static struct Object *sEndPeachObj;
static struct Object *sEndRightToadObj;
static struct Object *sEndLeftToadObj;
static struct Object *sEndJumboStarObj;
static UNUSED s32 sUnused;
static s16 sEndPeachAnimation;
static s16 sEndToadAnims[2];

Vp sEndCutsceneVp = { { { 640, 480, 511, 0 }, { 640, 480, 511, 0 } } };
static struct CreditsEntry *sDispCreditsEntry = NULL;

// related to peach gfx?
static s8 D_8032CBE4 = 0;
static s8 D_8032CBE8 = 0;
static s8 D_8032CBEC[7] = { 2, 3, 2, 1, 2, 3, 2 };

static u8 sStarsNeededForDialog[6] = { 1, 3, 8, 30, 50, 70 };

/**
 * Data for the jumbo star cutscene. It specifies the flight path after triple
 * jumping. Each entry is one keyframe.
 * The first number is playback speed, 1000 is the maximum and means it lasts
 * 1 frame. 20 means that it lasts 1000/20 = 50 frames.
 * Speed 0 marks the last keyframe. Since the cubic spline looks 3 keyframes
 * ahead, there should be at least 2 more entries afterwards.
 * The last three numbers of each entry are x, y and z coordinates of points
 * that define the curve.
 */
static Vec4s sJumboStarKeyframes[27] = {
    { 20, 0, 678, -2916 },      { 30, 0, 680, -3500 },      { 40, 1000, 700, -4000 },
    { 50, 2500, 750, -3500 },   { 50, 3500, 800, -2000 },   { 50, 4000, 850, 0 },
    { 50, 3500, 900, 2000 },    { 50, 2000, 950, 3500 },    { 50, 0, 1000, 4000 },
    { 50, -2000, 1050, 3500 },  { 50, -3500, 1100, 2000 },  { 50, -4000, 1150, 0 },
    { 50, -3500, 1200, -2000 }, { 50, -2000, 1250, -3500 }, { 50, 0, 1300, -4000 },
    { 50, 2000, 1350, -3500 },  { 50, 3500, 1400, -2000 },  { 50, 4000, 1450, 0 },
    { 50, 3500, 1500, 2000 },   { 50, 2000, 1600, 3500 },   { 50, 0, 1700, 4000 },
    { 50, -2000, 1800, 3500 },  { 50, -3500, 1900, 2000 },  { 30, -4000, 2000, 0 },
    { 0, -3500, 2100, -2000 },  { 0, -2000, 2200, -3500 },  { 0, 0, 2300, -4000 },
};

/**
 * get_credits_str_width: Calculate width of a Credits String
 * Loop over each character in a credits string and increment the length. If the
 * character is a space, increment by 4; otherwise increment by 7. Once the next
 * character is a null character (equal to 0), stop counting the length since
 * that's the end of the string.
 */
s32 get_credits_str_width(char *str) {
    u32 c;
    s32 length = 0;

    while ((c = *str++) != 0) {
        length += (c == ' ' ? 4 : 7);
    }

    return length;
}

#define CREDIT_TEXT_MARGIN_X ((s32) (GFX_DIMENSIONS_ASPECT_RATIO * 21))
#define CREDIT_TEXT_X_LEFT GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(CREDIT_TEXT_MARGIN_X)
#define CREDIT_TEXT_X_RIGHT GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(CREDIT_TEXT_MARGIN_X)

void bhv_end_peach_loop(void) {
    cur_obj_init_animation_with_sound(sEndPeachAnimation);
    if (cur_obj_check_if_near_animation_end()) {
        // anims: 0-3, 4, 5, 6-8, 9, 10, 11
        if (sEndPeachAnimation < 3 || sEndPeachAnimation == 6 || sEndPeachAnimation == 7) {
            sEndPeachAnimation++;
        }
    }
}

void bhv_end_toad_loop(void) {
    s32 toadAnimIndex = (gCurrentObject->oPosX >= 0.0f);

    cur_obj_init_animation_with_sound(sEndToadAnims[toadAnimIndex]);
    if (cur_obj_check_if_near_animation_end()) {
        // 0-1, 2-3, 4, 5, 6, 7
        if (sEndToadAnims[toadAnimIndex] == 0 || sEndToadAnims[toadAnimIndex] == 2) {
            sEndToadAnims[toadAnimIndex]++;
        }
    }
}

// Geo switch case function for controlling Peach's eye state.
s32 geo_switch_peach_eyes(s32 run, struct GraphNode *node, UNUSED s32 a2) {
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;
    s16 timer;

    if (run) {
        if (D_8032CBE4 == 0) {
            timer = (gAreaUpdateCounter + 0x20) >> 1 & 0x1F;
            if (timer < 7) {
                switchCase->selectedCase = D_8032CBE8 * 4 + D_8032CBEC[timer];
            } else {
                switchCase->selectedCase = D_8032CBE8 * 4 + 1;
            }
        } else {
            switchCase->selectedCase = D_8032CBE8 * 4 + D_8032CBE4 - 1;
        }
    }

    return 0;
}

// save menu handler
void handle_save_menu(struct MarioState *m) {
    s32 dialogID;
    // wait for the menu to show up
    if (is_anim_past_end(m) && gSaveOptSelectIndex != 0) {
        // save and continue / save and quit
        if (gSaveOptSelectIndex == SAVE_OPT_SAVE_AND_CONTINUE
            || gSaveOptSelectIndex == SAVE_OPT_SAVE_AND_QUIT) {
            save_file_do_save(gCurrSaveFileNum - 1);

            if (gSaveOptSelectIndex == SAVE_OPT_SAVE_AND_QUIT) {
                fade_into_special_warp(-2, 0); // reset game
            }
        }

        // not quitting
        if (gSaveOptSelectIndex != SAVE_OPT_SAVE_AND_QUIT) {
            disable_time_stop();
            m->faceAngle[1] += 0x8000;
            // figure out what dialog to show, if we should
            set_mario_action(m, ACT_IDLE, 0);
        }
    }
}

/**
 * spawn_obj_at_mario_rel_yaw: Spawns object at Mario with relative yaw.
 * Spawns object with given behavior and model and copies over Mario's position
 * and yaw plus relative yaw.
 */
struct Object *spawn_obj_at_mario_rel_yaw(struct MarioState *m, s32 model,
                                          const BehaviorScript *behavior, s16 relYaw) {
    struct Object *o = spawn_object(m->marioObj, model, behavior);

    o->oFaceAngleYaw = m->faceAngle[1] + relYaw;
    o->oPosX = m->pos[0];
    o->oPosY = m->pos[1];
    o->oPosZ = m->pos[2];

    return o;
}

/**
 * cutscene_take_cap_off: Put Mario's cap on.
 * Clears "cap on head" flag, sets "cap in hand" flag, plays sound
 * SOUND_ACTION_UNKNOWN43D.
 */
void cutscene_take_cap_off(struct MarioState *m) {
    m->flags &= ~MARIO_CAP_ON_HEAD;
    m->flags |= MARIO_CAP_IN_HAND;
    play_sound(SOUND_ACTION_UNKNOWN43D, m->marioObj->header.gfx.cameraToObject);
}

/**
 * cutscene_put_cap_on: Put Mario's cap on.
 * Clears "cap in hand" flag, sets "cap on head" flag, plays sound
 * SOUND_ACTION_UNKNOWN43E.
 */
void cutscene_put_cap_on(struct MarioState *m) {
    m->flags &= ~MARIO_CAP_IN_HAND;
    m->flags |= MARIO_CAP_ON_HEAD;
    play_sound(SOUND_ACTION_UNKNOWN43E, m->marioObj->header.gfx.cameraToObject);
}

/**
 * mario_ready_to_speak: Determine if Mario is able to speak to a NPC
 * The following conditions must be met in order for Mario to be considered
 * ready to speak.
 * 1: Mario's action must be in the stationary or moving action groups, or if
 *    not, he must be in the "waiting for dialog" state.
 * 2: Mario mat not be riding a shell or be invulnerable.
 * 3: Mario must not be in first person mode.
 */
s32 mario_ready_to_speak(void) {
    u32 actionGroup = gMarioState->action & ACT_GROUP_MASK;

    if ((gMarioState->action == ACT_WAITING_FOR_DIALOG || actionGroup == ACT_GROUP_STATIONARY
         || actionGroup == ACT_GROUP_MOVING)
        && (!(gMarioState->action & (ACT_FLAG_RIDING_SHELL | ACT_FLAG_INVULNERABLE))
            && gMarioState->action != ACT_FIRST_PERSON)) {
        return TRUE;
    }

    return 0;
}

// (can) place Mario in dialog?
// initiate dialog?
// return values:
// 0 = not in dialog
// 1 = starting dialog
// 2 = speaking
s32 set_mario_npc_dialog(s32 actionArg) {
    s32 dialogState = 0;

    // in dialog
    if (gMarioState->action == ACT_READING_NPC_DIALOG) {
        if (gMarioState->actionState < 8) {
            dialogState = 1; // starting dialog
        }
        if (gMarioState->actionState == 8) {
            if (actionArg == 0) {
                gMarioState->actionState++; // exit dialog
            } else {
                dialogState = 2;
            }
        }
    } else if (actionArg != 0 && mario_ready_to_speak()) {
        gMarioState->usedObj = gCurrentObject;
        set_mario_action(gMarioState, ACT_READING_NPC_DIALOG, actionArg);
        dialogState = 1; // starting dialog
    }

    return dialogState;
}

// actionargs:
// 1 : no head turn
// 2 : look up
// 3 : look down
// actionstate values:
// 0 - 7: looking toward npc
// 8: in dialog
// 9 - 22: looking away from npc
// 23: end
s32 act_reading_npc_dialog(struct MarioState *m) {
    s32 headTurnAmount = 0;
    s16 angleToNPC;

    if (m->actionArg == 2) {
        headTurnAmount = -1024 * 8;
    }
    if (m->actionArg == 3) {
        headTurnAmount = 384 * 8;
    }
    if (m->actionArg == 4) {
        headTurnAmount = atan2s(lateral_dist_between_objects(m->marioObj, m->usedObj),
                                m->pos[1] - m->usedObj->oPosY);
    }

    if (m->actionState < 8) {
        angleToNPC = mario_obj_angle_to_object(m, m->usedObj);
        m->faceAngle[1] =
            angleToNPC - approach_s32((angleToNPC - m->faceAngle[1]) << 16 >> 16, 0, 2048, 2048);
        set_mario_animation(m, m->heldObj == NULL ? MARIO_ANIM_FIRST_PERSON
                                                  : MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
        m->actionTimer = approach_s16_symmetric(m->actionTimer, headTurnAmount, 0x400);
        if (abs_angle_diff(m->faceAngle[1], angleToNPC) < 0x200) {
            m->actionState = 8;
        } else {
            m->actionState = 2;
        }
    } else if (m->actionState >= 9 && m->actionState < 17) {
        m->actionTimer = approach_s16_symmetric(m->actionTimer, 0, 0x400);
        if (abs_angle_diff(m->actionTimer, 0) < 0x200) {
            m->actionState = 18;
        } else {
            m->actionState = 16;
        }
    } else if (m->actionState == 23) {
        if (m->flags & MARIO_CAP_IN_HAND) {
            set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
        } else {
            set_mario_action(m, m->heldObj == NULL ? ACT_IDLE : ACT_HOLD_IDLE, 0);
        }
        m->actionTimer = 0;
    }
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    vec3s_set(m->marioBodyState->headAngle, m->actionTimer, 0, 0);

    if (m->actionState != 8) {
        m->actionState++;
    }

    return FALSE;
}

// puts Mario in a state where he's waiting for (npc) dialog; doesn't do much
s32 act_waiting_for_dialog(struct MarioState *m) {
    set_mario_animation(m,
                        m->heldObj == NULL ? MARIO_ANIM_FIRST_PERSON : MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    return FALSE;
}

// makes Mario disappear and triggers warp
s32 act_disappeared(struct MarioState *m) {
    // CuckyDev: Continue moving instead of disappearing (warps)
    switch (m->actionTimer) {
        case 0: // Get routine to run
            if (m->interactObj->behavior == segmented_to_virtual(bhvWarpPipe)) {
                m->actionTimer = 2;
            } else {
                m->actionTimer = 1;
                    // Fallthrough
                case 1: // Moving (regular warp)
                    m->vel[0] *= 0.95f;
                    m->vel[1] -= 2.f;
                    m->vel[1] *= 0.95f;
                    m->vel[2] *= 0.95f;
                    perform_air_step(m, AIR_STEP_NO_GRAVITY);
                    if (m->pos[1] <= m->floorHeight + 20.0f)
                        m->pos[1] = m->floorHeight;
                    break;
            }
            // Fallthrough
        case 2: // Warp pipe (fall straight downwards)
            if (m->vel[1] > -10.0f)
                m->vel[1] = -10.0f;
            m->marioObj->header.gfx.pos[1] += (m->vel[1] *= 0.9f);
            break;
        case 3: // Submarine (disappear)
            set_mario_animation(m, MARIO_ANIM_A_POSE);
            mario_set_forward_vel(m, 0.0f);
            m->vel[1] = 0.0f;
            m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
            break;
    }

    if (m->actionArg) {
        m->actionArg--;
        if ((m->actionArg & 0xFFFF) == 0) {
            level_trigger_warp(m, m->actionArg >> 16);
        }
    }
    return FALSE;
}

s32 act_reading_automatic_dialog(struct MarioState *m) {
    u32 actionArg;

    m->actionState++;
    if (m->actionState == 2) {
        enable_time_stop();
    }
    if (m->actionState < 9) {
        set_mario_animation(m, m->prevAction == ACT_STAR_DANCE_WATER ? MARIO_ANIM_WATER_IDLE
                                                                     : MARIO_ANIM_FIRST_PERSON);
        // always look up for automatic dialogs
        m->actionTimer -= 1024;
    } else {
        // set Mario dialog
        if (m->actionState == 9) {
            actionArg = m->actionArg;
            if (GET_HIGH_U16_OF_32(actionArg) == 0) {
                create_dialog_box(GET_LOW_U16_OF_32(actionArg));
            } else {
                create_dialog_box_with_var(GET_HIGH_U16_OF_32(actionArg), GET_LOW_U16_OF_32(actionArg));
            }
        }
        // wait until dialog is done
        else if (m->actionState == 10) {
            if (get_dialog_id() >= 0) {
                m->actionState--;
            }
        }
        // look back down
        else if (m->actionState < 19) {
            m->actionTimer += 1024;
        }
        // finished action
        else if (m->actionState == 25) {
            disable_time_stop();
            if (m->prevAction == ACT_STAR_DANCE_WATER) {
                set_mario_action(m, ACT_WATER_IDLE, 0); // 100c star?
            } else {
                // make Mario walk into door after star dialog
                set_mario_action(m, m->prevAction == ACT_UNLOCKING_STAR_DOOR ? ACT_WALKING : ACT_IDLE,
                                 0);
            }
        }
    }
    // apply head turn
    vec3s_set(m->marioBodyState->headAngle, m->actionTimer, 0, 0);
    return FALSE;
}

s32 act_reading_sign(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;
    play_sound_if_no_flag(m, SOUND_ACTION_READ_SIGN, MARIO_ACTION_SOUND_PLAYED);

    switch (m->actionState) {
        // start dialog
        case 0:
            trigger_cutscene_dialog(1);
            enable_time_stop();
            // reading sign
            set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
            m->actionState = 1;
            // intentional fall through
        // turn toward sign
        case 1:
            m->faceAngle[1] += marioObj->oMarioPoleUnk108 / 11;
            m->pos[0] += marioObj->oMarioReadingSignDPosX / 11.0f;
            m->pos[2] += marioObj->oMarioReadingSignDPosZ / 11.0f;
            // create the text box
            if (m->actionTimer++ == 10) {
                create_dialog_inverted_box(m->usedObj->oBehParams2ndByte);
                m->actionState = 2;
            }
            break;
        // in dialog
        case 2:
            // dialog finished
            if (gCamera->cutscene == 0) {
                disable_time_stop();
                set_mario_action(m, ACT_IDLE, 0);
            }
            break;
    }

    m->marioBodyState->headAngle[0] = -0x2000;
    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    return FALSE;
}

s16 showStarHUD = 0;
extern u16 sCurrentMusic;
// star dance handler
void general_star_dance_handler(struct MarioState *m, s32 isInWater) {
    s32 dialogID;
    if (m->actionState == 0) {
        switch (++m->actionTimer) {
            case 1:
                spawn_object(m->marioObj, MODEL_STAR, bhvCelebrationStar);
                disable_background_sound();
                if (m->actionArg & 1) {
                    // play_course_clear();
                    play_music(SEQ_PLAYER_ENV, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_COLLECT_STAR), 0);
                } else {
                    if (gCurrLevelNum == LEVEL_BOWSER_1 || gCurrLevelNum == LEVEL_BOWSER_2) {
                        play_music(SEQ_PLAYER_ENV, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_COLLECT_KEY),
                                   0);
                    } else {
                        play_music(SEQ_PLAYER_ENV, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_COLLECT_STAR),
                                   0);
                    }
                }
                break;

            case 42:
                play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);
                break;

            case 80:
                if ((m->actionArg & 1) == 0) {
                    level_trigger_warp(m, WARP_OP_STAR_EXIT);
                } else {
                    enable_time_stop();
                    /*  create_dialog_box_with_response(gLastCompletedStarNum == 7 ? DIALOG_013
                                                                                 : DIALOG_014);*/
                    m->actionState = 1;
                }
                break;
        }
    } else if (m->actionState == 1) {
        //  if (gDialogResponse == 1) {
        save_file_do_save(gCurrSaveFileNum - 1);
        //  }
        m->actionState = 2;
    } else if (m->actionState == 2 && is_anim_at_end(m)) {
        disable_time_stop();
        enable_background_sound();
        set_mario_action(m, isInWater ? ACT_WATER_IDLE : ACT_IDLE, 0);
        func_803200E4(50);
        sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
        showStarHUD = 0;
    }
}

s16 starIDHUD = 0;
s32 act_star_dance(struct MarioState *m) {
    m->faceAngle[1] = m->area->camera->yaw;
    set_mario_animation(m, m->actionState == 2 ? MARIO_ANIM_RETURN_FROM_STAR_DANCE
                                               : MARIO_ANIM_STAR_DANCE);
    showStarHUD = 1;
    general_star_dance_handler(m, 0);
    if (m->actionState != 2 && m->actionTimer >= 40) {
        m->marioBodyState->handState = MARIO_HAND_PEACE_SIGN;
    }
    // print star name, fade in/out by actiontimer
    stop_and_set_height_to_floor(m);
    return FALSE;
}

s32 act_star_dance_water(struct MarioState *m) {
    m->faceAngle[1] = m->area->camera->yaw;
    set_mario_animation(m, m->actionState == 2 ? MARIO_ANIM_RETURN_FROM_WATER_STAR_DANCE
                                               : MARIO_ANIM_WATER_STAR_DANCE);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    general_star_dance_handler(m, 1);
    if (m->actionState != 2 && m->actionTimer >= 62) {
        m->marioBodyState->handState = MARIO_HAND_PEACE_SIGN;
    }
    return FALSE;
}

s32 act_fall_after_star_grab(struct MarioState *m) {
    if (m->pos[1] < m->waterLevel - 130) {
        play_sound(SOUND_ACTION_UNKNOWN430, m->marioObj->header.gfx.cameraToObject);
        m->particleFlags |= PARTICLE_WATER_SPLASH;
        return set_mario_action(m, ACT_STAR_DANCE_WATER, m->actionArg);
    }
    if (perform_air_step(m, 0) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, m->actionArg & 1 ? ACT_STAR_DANCE_NO_EXIT : ACT_STAR_DANCE_EXIT,
                         m->actionArg);
    }
    set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    return FALSE;
}

// general death hander
s32 common_death_handler(struct MarioState *m, s32 animation, s32 frameToDeathWarp) {
    s32 animFrame = set_mario_animation(m, animation);
    if (animFrame == frameToDeathWarp) {
        level_trigger_warp(m, WARP_OP_DEATH);
    }
    m->marioBodyState->eyeState = MARIO_EYES_DEAD;
    stop_and_set_height_to_floor(m);
    return animFrame;
}

s32 act_standing_death(struct MarioState *m) {
    if (m->input & INPUT_IN_POISON_GAS) {
        return set_mario_action(m, ACT_SUFFOCATION, 0);
    }

    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    common_death_handler(m, MARIO_ANIM_DYING_FALL_OVER, 80);
    if (m->marioObj->header.gfx.unk38.animFrame == 77) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }
    return FALSE;
}

s32 act_electrocution(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    common_death_handler(m, MARIO_ANIM_ELECTROCUTION, 43);
    return FALSE;
}

s32 act_suffocation(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    common_death_handler(m, MARIO_ANIM_SUFFOCATING, 86);
    return FALSE;
}

s32 act_death_on_back(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    if (common_death_handler(m, MARIO_ANIM_DYING_ON_BACK, 54) == 40) {
        play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }
    return FALSE;
}

s32 act_death_on_stomach(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    if (common_death_handler(m, MARIO_ANIM_DYING_ON_STOMACH, 37) == 37) {
        play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }
    return FALSE;
}
extern s16 fadeoutstarted;
s32 act_quicksand_death(struct MarioState *m) {
    if (m->actionState == 0) {
        //  set_mario_animation(m, MARIO_ANIM_DYING_IN_QUICKSAND);
        // set_anim_to_frame(m, 60);
        play_sound(SOUND_GENERAL_MOVING_WATER, m->marioObj->header.gfx.cameraToObject);
        m->actionState = 1;
        m->health -= 0x40;
    }
    if (m->actionState == 1) {
        if (m->quicksandDepth >= 100.0f) {
            play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_ACTION_SOUND_PLAYED);
        }
        if ((m->quicksandDepth += 5.0f) >= 180.0f) {
            fadeoutstarted = 1;
        }
    }
    stationary_ground_step(m);
    return FALSE;
}

s32 act_eaten_by_bubba(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_ACTION_SOUND_PLAYED);
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    m->health = 0xFF;
    if (m->actionTimer++ == 120) {
        level_trigger_warp(m, WARP_OP_DEATH);
    }
    return FALSE;
}

// set animation and forwardVel; when perform_air_step returns AIR_STEP_LANDED,
// set the new action
s32 launch_mario_until_land(struct MarioState *m, s32 endAction, s32 animation, f32 forwardVel) {
    s32 airStepLanded;
    mario_set_forward_vel(m, forwardVel);
    set_mario_animation(m, animation);
    if ((perform_air_step(m, 0) == AIR_STEP_LANDED)) {
        set_mario_action(m, endAction, 0);
    }
    return airStepLanded;
}

s32 act_unlocking_key_door(struct MarioState *m) {
    m->faceAngle[1] = m->usedObj->oMoveAngleYaw;

    m->pos[0] = m->usedObj->oPosX + coss(m->faceAngle[1]) * 75.0f;
    m->pos[2] = m->usedObj->oPosZ + sins(m->faceAngle[1]) * 75.0f;

    if (m->actionArg & 2) {
        m->faceAngle[1] += 0x8000;
    }

    if (m->actionTimer == 0) {
        spawn_obj_at_mario_rel_yaw(m, MODEL_BOWSER_KEY_CUTSCENE, bhvBowserKeyUnlockDoor, 0);
        set_mario_animation(m, MARIO_ANIM_UNLOCK_DOOR);
    }

    switch (m->marioObj->header.gfx.unk38.animFrame) {
        case 79:
            play_sound(SOUND_GENERAL_DOOR_INSERT_KEY, m->marioObj->header.gfx.cameraToObject);
            break;
        case 111:
            play_sound(SOUND_GENERAL_DOOR_TURN_KEY, m->marioObj->header.gfx.cameraToObject);
            break;
    }

    update_mario_pos_for_anim(m);
    stop_and_set_height_to_floor(m);

    if (is_anim_at_end(m)) {
        if (m->usedObj->oBehParams >> 24 == 1) {
            save_file_set_flags(SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR);
            save_file_clear_flags(SAVE_FLAG_HAVE_KEY_2);
        } else {
            save_file_set_flags(SAVE_FLAG_UNLOCKED_BASEMENT_DOOR);
            save_file_clear_flags(SAVE_FLAG_HAVE_KEY_1);
        }
        set_mario_action(m, ACT_WALKING, 0);
    }

    m->actionTimer++;
    return FALSE;
}

s32 act_unlocking_star_door(struct MarioState *m) {
    switch (m->actionState) {
        case 0:
            m->faceAngle[1] = m->usedObj->oMoveAngleYaw;
            if (m->actionArg & 2) {
                m->faceAngle[1] += 0x8000;
            }
            m->marioObj->oMarioReadingSignDPosX = m->pos[0];
            m->marioObj->oMarioReadingSignDPosZ = m->pos[2];
            set_mario_animation(m, MARIO_ANIM_SUMMON_STAR);
            m->actionState++;
            break;
        case 1:
            if (is_anim_at_end(m)) {
                spawn_object(m->marioObj, MODEL_STAR, bhvUnlockDoorStar);
                m->actionState++;
            }
            break;
        case 2:
            if (m->actionTimer++ == 70) {
                set_mario_animation(m, MARIO_ANIM_RETURN_STAR_APPROACH_DOOR);
                m->actionState++;
            }
            break;
        case 3:
            if (is_anim_at_end(m)) {
                save_file_set_flags(get_door_save_file_flag(m->usedObj));
                set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, DIALOG_038);
            }
            break;
    }

    m->pos[0] = m->marioObj->oMarioReadingSignDPosX;
    m->pos[2] = m->marioObj->oMarioReadingSignDPosZ;

    update_mario_pos_for_anim(m);
    stop_and_set_height_to_floor(m);

    return FALSE;
}

// not sure what kind of door this is
s32 act_entering_star_door(struct MarioState *m) {
    f32 targetDX;
    f32 targetDZ;
    s16 targetAngle;

    if (m->actionTimer++ == 0) {
        m->interactObj->oInteractStatus = 0x00010000;

        // ~30 degrees / 1/12 rot
        targetAngle = m->usedObj->oMoveAngleYaw + 0x1555;
        if (m->actionArg & 2) {
            targetAngle += 0x5556; // ~120 degrees / 1/3 rot (total 150d / 5/12)
        }

        // targetDX and targetDZ are the offsets to add to Mario's position to
        // have Mario stand 150 units in front of the door

        targetDX = m->usedObj->oPosX + 150.0f * sins(targetAngle) - m->pos[0];
        targetDZ = m->usedObj->oPosZ + 150.0f * coss(targetAngle) - m->pos[2];

        m->marioObj->oMarioReadingSignDPosX = targetDX / 20.0f;
        m->marioObj->oMarioReadingSignDPosZ = targetDZ / 20.0f;

        m->faceAngle[1] = atan2s(targetDZ, targetDX);
    }

    // set Mario's animation
    if (m->actionTimer < 15) {
        set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    }

    // go through door? for 20 frames
    else if (m->actionTimer < 35) {
        m->pos[0] += m->marioObj->oMarioReadingSignDPosX;
        m->pos[2] += m->marioObj->oMarioReadingSignDPosZ;

        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, 0x00028000);
    }

    else {
        m->faceAngle[1] = m->usedObj->oMoveAngleYaw;

        if (m->actionArg & 2) {
            m->faceAngle[1] += 0x8000;
        }

        m->pos[0] += 12.0f * sins(m->faceAngle[1]);
        m->pos[2] += 12.0f * coss(m->faceAngle[1]);

        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, 0x00028000);
    }

    stop_and_set_height_to_floor(m);

    if (m->actionTimer == 48) {
        set_mario_action(m, ACT_IDLE, 0);
    }

    return FALSE;
}

s32 act_going_through_door(struct MarioState *m) {
    if (m->actionTimer == 0) {
        if (m->actionArg & 1) {
            m->interactObj->oInteractStatus = 0x00010000;
            set_mario_animation(m, MARIO_ANIM_PULL_DOOR_WALK_IN);
        } else {
            m->interactObj->oInteractStatus = 0x00020000;
            set_mario_animation(m, MARIO_ANIM_PUSH_DOOR_WALK_IN);
        }
    }
    m->faceAngle[1] = m->usedObj->oMoveAngleYaw;
    m->pos[0] = m->usedObj->oPosX;
    m->pos[2] = m->usedObj->oPosZ;

    update_mario_pos_for_anim(m);
    // stop_and_set_height_to_floor(m);

    mario_set_forward_vel(m, 0.0f);
    m->vel[1] = 0.0f;

    vec3f_copy(gMarioState->marioObj->header.gfx.pos, m->pos);
    vec3s_set(gMarioState->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    if (m->actionArg & 4) {
        if (m->actionTimer == 16) {
            level_trigger_warp(m, WARP_OP_WARP_DOOR);
        }
    } else {
        if (is_anim_at_end(m)) {
            if (m->actionArg & 2) {
                m->faceAngle[1] += 0x8000;
            }
            set_mario_action(m, ACT_IDLE, 0);
        }
    }

    m->actionTimer++;
    return FALSE;
}
extern u8 prevRoom;
extern u8 curRoom;
s32 act_warp_door_spawn(struct MarioState *m) {
    struct Object *door;
    if (door = cur_obj_nearest_object_with_behavior(bhvDoorWarp2)) {
        curRoom = door->oBehParams & 0xff; // assume front entrance
        prevRoom = curRoom;
    } else if (door = cur_obj_nearest_object_with_behavior(bhvDoorWarp)) {
        curRoom = door->oBehParams & 0xff;
        prevRoom = curRoom;
    }
    if (m->actionState == 0) {
        m->actionState = 1;
        if (m->actionArg & 1) {
            m->usedObj->oInteractStatus = 0x00040000;
        } else {
            m->usedObj->oInteractStatus = 0x00080000;
        }
    } else if (m->usedObj->oAction == 0) {
        set_mario_action(m, ACT_IDLE, 0);
    }
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    stop_and_set_height_to_floor(m);
    return FALSE;
}

s32 act_emerge_from_pipe(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if (m->actionTimer++ < 11) {
        marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        return FALSE;
    }

    marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;

    play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);

    if (gCurrLevelNum == LEVEL_THI) {
        if (gCurrAreaIndex == 2) {
            play_sound_if_no_flag(m, SOUND_MENU_EXIT_PIPE, MARIO_ACTION_SOUND_PLAYED);
        } else {
            play_sound_if_no_flag(m, SOUND_MENU_ENTER_PIPE, MARIO_ACTION_SOUND_PLAYED);
        }
    }

    if (launch_mario_until_land(m, ACT_JUMP_LAND_STOP, MARIO_ANIM_SINGLE_JUMP, 8.0f)) {
        mario_set_forward_vel(m, 0.0f);
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }
    return FALSE;
}

s32 act_spawn_spin_airborne(struct MarioState *m) {
    // entered water, exit action
    if (m->pos[1] < m->waterLevel - 100) {
        return set_water_plunge_action(m);
    }

    // updates all velocity variables based on m->forwardVel
    mario_set_forward_vel(m, m->forwardVel);

    // landed on floor, play spawn land animation
    if (perform_air_step(m, 0.0) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, ACT_SPAWN_SPIN_LANDING, 0);
    }

    // is 300 units above floor, spin and play woosh sounds
    if (m->actionState == 0 && m->pos[1] - m->floorHeight > 300.0f) {
        if (set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING) == 0) { // first anim frame
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
        }
    }

    // under 300 units above floor, enter freefall animation
    else {
        m->actionState = 1;
        set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    }

    return FALSE;
}

s32 act_spawn_spin_landing(struct MarioState *m) {
    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
    }
    return FALSE;
}

/**
 * act_exit_airborne: Jump out of a level after collecting a Power Star (no
 ** sparkles)
 * Mario always faces a level entrance when he launches out of it, whether he
 * died or he collected a star/key. Because of that, we need him to move away
 * from the painting by setting his speed to -32.0f and have him face away from
 * the painting by adding 0x8000 (180 deg) to his graphics angle. We also set
 * his heal counter to 31 to restore 7.75 units of his health, and enable the
 * particle flag that generates sparkles.
 */
s32 act_exit_airborne(struct MarioState *m) {
    if (15 < m->actionTimer++
        && launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_GENERAL_FALL, -32.0f)) {
        // heal Mario
    }
    m->healCounter = 31;
    // rotate him to face away from the entrance
    m->marioObj->header.gfx.angle[1] += 0x8000;
    m->particleFlags |= PARTICLE_SPARKLES;
    return FALSE;
}

s32 act_falling_exit_airborne(struct MarioState *m) {
    if (launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
        // heal Mario
    }
    m->healCounter = 31;
    // rotate Mario to face away from the entrance
    m->marioObj->header.gfx.angle[1] += 0x8000;
    m->particleFlags |= PARTICLE_SPARKLES;
    return FALSE;
}

s32 act_exit_land_save_dialog(struct MarioState *m) {
    s32 animFrame;
    stationary_ground_step(m);
    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
    switch (m->actionState) {
        // determine type of exit
        case 0:
            set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_GENERAL_LAND
                                                     : MARIO_ANIM_LAND_FROM_SINGLE_JUMP);
            if (is_anim_past_end(m)) {
                if (gLastCompletedCourseNum != COURSE_BITDW
                    && gLastCompletedCourseNum != COURSE_BITFS) {
                    enable_time_stop();
                }

                set_menu_mode(RENDER_COURSE_DONE_SCREEN);
                gSaveOptSelectIndex = 0;

                m->actionState = 3; // star exit with cap
                if (!(m->flags & MARIO_CAP_ON_HEAD)) {
                    m->actionState = 2; // star exit without cap
                }
                if (gLastCompletedCourseNum == COURSE_BITDW
                    || gLastCompletedCourseNum == COURSE_BITFS) {
                    m->actionState = 1; // key exit
                }
            }
            break;
        // key exit
        case 1:
            animFrame = set_mario_animation(m, MARIO_ANIM_THROW_CATCH_KEY);
            switch (animFrame) {
                case -1:
                    spawn_obj_at_mario_rel_yaw(m, MODEL_BOWSER_KEY_CUTSCENE, bhvBowserKeyCourseExit,
                                               -32768);
                    //! fall through
                case 67:
                    play_sound(SOUND_ACTION_KEY_SWISH, m->marioObj->header.gfx.cameraToObject);
                    //! fall through
                case 83:
                    play_sound(SOUND_ACTION_PAT_BACK, m->marioObj->header.gfx.cameraToObject);
                    //! fall through
                case 111:
                    play_sound(SOUND_ACTION_UNKNOWN45C, m->marioObj->header.gfx.cameraToObject);
                    // no break
            }
            handle_save_menu(m);
            break;
        // exit without cap
        case 2:
            animFrame = set_mario_animation(m, MARIO_ANIM_MISSING_CAP);
            if ((animFrame >= 18 && animFrame < 55) || (animFrame >= 112 && animFrame < 134)) {
                m->marioBodyState->handState = MARIO_HAND_OPEN;
            }
            if (!(animFrame < 109) && animFrame < 154) {
                m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            }

            handle_save_menu(m);
            break;
        // exit with cap
        case 3:
            animFrame = set_mario_animation(m, MARIO_ANIM_TAKE_CAP_OFF_THEN_ON);
            switch (animFrame) {
                case 12:
                    cutscene_take_cap_off(m);
                    break;
                case 37:
                // fall through
                case 53:
                    play_sound(SOUND_ACTION_BRUSH_HAIR, m->marioObj->header.gfx.cameraToObject);
                    break;
                case 82:
                    cutscene_put_cap_on(m);
                    break;
            }
            handle_save_menu(m);
            break;
    }

    m->marioObj->header.gfx.angle[1] += 0x8000;
    return FALSE;
}

s32 act_death_exit(struct MarioState *m) {
    if (15 < m->actionTimer++
        && launch_mario_until_land(m, ACT_DEATH_EXIT_LAND, MARIO_ANIM_GENERAL_FALL, -32.0f)) {
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#ifdef VERSION_SH
        queue_rumble_data(5, 80);
#endif
        // restore 7.75 units of health
    }
    m->healCounter = 31;
    // one unit of health
    m->health = 0x0100;
    return FALSE;
}

s32 act_unused_death_exit(struct MarioState *m) {
    if (launch_mario_until_land(m, ACT_FREEFALL_LAND_STOP, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
        // restore 7.75 units of health
    }
    // one unit of health
    m->healCounter = 31;
    m->health = 0x0100;
    return FALSE;
}

s32 act_falling_death_exit(struct MarioState *m) {
    if (launch_mario_until_land(m, ACT_DEATH_EXIT_LAND, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
#ifdef VERSION_JP
        play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
#else
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#endif
#ifdef VERSION_SH
        queue_rumble_data(5, 80);
#endif
        // restore 7.75 units of health
    }
    // one unit of health
    m->healCounter = 31;
    m->health = 0x0100;
    return FALSE;
}

// waits 11 frames before actually executing, also has reduced fvel
s32 act_special_exit_airborne(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);

    if (m->actionTimer++ < 11) {
        marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        return FALSE;
    }

    if (launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_SINGLE_JUMP, -24.0f)) {
        // heal Mario
        m->actionArg = 1;
    }
    m->healCounter = 31;

    m->particleFlags |= PARTICLE_SPARKLES;
    // rotate Mario to face away from the entrance
    marioObj->header.gfx.angle[1] += 0x8000;
    // show Mario
    marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;

    return FALSE;
}

s32 act_special_death_exit(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if (m->actionTimer++ < 11) {
        marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        return FALSE;
    }

    if (launch_mario_until_land(m, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_AIR_KB, -24.0f)) {
#ifdef VERSION_SH
        queue_rumble_data(5, 80);
#endif
        m->healCounter = 31;
    }
    // show Mario
    marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    // one unit of health
    m->health = 0x0100;

    return FALSE;
}

s32 act_spawn_no_spin_airborne(struct MarioState *m) {
    launch_mario_until_land(m, ACT_SPAWN_NO_SPIN_LANDING, MARIO_ANIM_GENERAL_FALL, 0.0f);
    if (m->pos[1] < m->waterLevel - 100) {
        set_water_plunge_action(m);
    }
    return FALSE;
}

s32 act_spawn_no_spin_landing(struct MarioState *m) {
    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
    set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
    stop_and_set_height_to_floor(m);
    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
    }
    return FALSE;
}

s32 act_bbh_enter_spin(struct MarioState *m) {
    f32 floorDist;
    f32 scale;
    f32 cageDX;
    f32 cageDZ;
    f32 cageDist;
    f32 forwardVel;

    cageDX = m->usedObj->oPosX - m->pos[0];
    cageDZ = m->usedObj->oPosZ - m->pos[2];
    cageDist = sqrtf(cageDX * cageDX + cageDZ * cageDZ);

    if (cageDist > 20.0f) {
        forwardVel = 10.0f;
    } else {
        forwardVel = cageDist / 2.0f;
    }
    if (forwardVel < 0.5f) {
        forwardVel = 0.0f;
    }

    switch (m->actionState) {
        case 0:
            floorDist = 512.0f - (m->pos[1] - m->floorHeight);
            m->vel[1] = floorDist > 0 ? sqrtf(4.0f * floorDist + 1.0f) - 1.0f : 2.0f;

            m->actionState = 1;
            m->actionTimer = 100;
            // fall through

        case 1:
            m->faceAngle[1] = atan2s(cageDZ, cageDX);
            mario_set_forward_vel(m, forwardVel);

            if (set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING) == 0) {
                play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
            }

            m->flags &= ~MARIO_UNKNOWN_08;
            perform_air_step(m, 0);
            if (m->vel[1] <= 0) {
                m->actionState = 2;
            }
            break;

        case 2:
            // fall through
        case 3:
            m->faceAngle[1] = atan2s(cageDZ, cageDX);
            mario_set_forward_vel(m, forwardVel);
            m->flags &= ~MARIO_UNKNOWN_08;
            if (perform_air_step(m, 0) == AIR_STEP_LANDED) {
                level_trigger_warp(m, WARP_OP_UNKNOWN_02);
#ifdef VERSION_SH
                queue_rumble_data(15, 80);
#endif
                m->actionState = 4;
            }
            if (m->actionState == 2) {
                if (m->marioObj->header.gfx.unk38.animFrame == 0) {
                    m->actionState = 3;
                }
            } else {
                play_sound_if_no_flag(m, SOUND_ACTION_SHRINK_INTO_BBH, MARIO_ACTION_SOUND_PLAYED);
                set_mario_animation(m, MARIO_ANIM_DIVE);
                m->marioObj->header.gfx.angle[0] = atan2s(m->forwardVel, -m->vel[1]);
            }
            m->squishTimer = 0xFF;
            if (m->actionTimer >= 11) {
                m->actionTimer -= 6;
                scale = m->actionTimer / 100.0f;
                vec3f_set(m->marioObj->header.gfx.scale, scale, scale, scale);
            }
            break;

        case 4:
            stop_and_set_height_to_floor(m);
            m->marioObj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            break;
    }

    return FALSE;
}

s32 act_bbh_enter_jump(struct MarioState *m) {
    f32 cageDX;
    f32 cageDZ;
    f32 cageDist;

    play_mario_action_sound(m, SOUND_ACTION_TERRAIN_JUMP, 1);
    play_mario_jump_sound(m);

    if (m->actionState == 0) {
        cageDX = m->usedObj->oPosX - m->pos[0];
        cageDZ = m->usedObj->oPosZ - m->pos[2];
        cageDist = sqrtf(cageDX * cageDX + cageDZ * cageDZ);

        m->vel[1] = 60.0f;
        m->faceAngle[1] = atan2s(cageDZ, cageDX);
        mario_set_forward_vel(m, cageDist / 20.0f);

        m->flags &= ~MARIO_UNKNOWN_08;
        m->actionState = 1;
    }

    set_mario_animation(m, MARIO_ANIM_DOUBLE_JUMP_RISE);
    perform_air_step(m, 0);

    if (m->vel[1] <= 0.0f) {
        set_mario_action(m, ACT_BBH_ENTER_SPIN, 0);
    }

    return FALSE;
}

s32 act_teleport_fade_out(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_ACTION_TELEPORT, MARIO_ACTION_SOUND_PLAYED);
    set_mario_animation(m, m->prevAction == ACT_CROUCHING ? MARIO_ANIM_CROUCHING
                                                          : MARIO_ANIM_FIRST_PERSON);

#ifdef VERSION_SH
    if (m->actionTimer == 0) {
        queue_rumble_data(30, 70);
        func_sh_8024C89C(2);
    }
#endif

    m->flags |= MARIO_TELEPORTING;

    if (m->actionTimer < 32) {
        m->fadeWarpOpacity = (-m->actionTimer << 3) + 0xF8;
    }

    if (m->actionTimer++ == 20) {
        level_trigger_warp(m, WARP_OP_TELEPORT);
    }

    stop_and_set_height_to_floor(m);

    return FALSE;
}

s32 act_teleport_fade_in(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_ACTION_TELEPORT, MARIO_ACTION_SOUND_PLAYED);
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);

#ifdef VERSION_SH
    if (m->actionTimer == 0) {
        queue_rumble_data(30, 70);
        func_sh_8024C89C(2);
    }
#endif

    if (m->actionTimer < 32) {
        m->flags |= MARIO_TELEPORTING;
        m->fadeWarpOpacity = m->actionTimer << 3;
    } else {
        m->flags &= ~MARIO_TELEPORTING;
    }

    if (m->actionTimer++ == 32) {
        if (m->pos[1] < m->waterLevel - 100) {
            set_mario_action(m, ACT_WATER_IDLE, 0);
        } else {
            set_mario_action(m, ACT_IDLE, 0);
        }
    }

    stop_and_set_height_to_floor(m);

    return FALSE;
}

s32 act_shocked(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_ACTION_SOUND_PLAYED);
    play_sound(SOUND_MOVING_SHOCKED, m->marioObj->header.gfx.cameraToObject);
    set_camera_shake_from_hit(SHAKE_SHOCK);

    if (set_mario_animation(m, MARIO_ANIM_SHOCKED) == 0) {
        m->actionTimer++;
        m->flags |= MARIO_METAL_SHOCK;
    }

    if (m->actionArg == 0) {
        mario_set_forward_vel(m, 0.0f);
        if (perform_air_step(m, 1) == AIR_STEP_LANDED) {
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            m->actionArg = 1;
        }
    } else {
        if (m->actionTimer >= 6) {
            m->invincTimer = 30;
            set_mario_action(m, m->health < 0x0100 ? ACT_ELECTROCUTION : ACT_IDLE, 0);
        }
        stop_and_set_height_to_floor(m);
    }

    return FALSE;
}

s32 act_squished(struct MarioState *m) {
    f32 squishAmount;
    f32 spaceUnderCeil;
    s16 surfAngle;
    s32 underSteepSurf = FALSE; // seems to be responsible for setting velocity?

    if ((spaceUnderCeil = m->ceilHeight - m->floorHeight) < 0) {
        spaceUnderCeil = 0;
    }

    switch (m->actionState) {
        case 0:
            if (spaceUnderCeil > 160.0f) {
                m->squishTimer = 0;
                return set_mario_action(m, ACT_IDLE, 0);
            }

            m->squishTimer = 0xFF;

            if (spaceUnderCeil >= 10.1f) {
                // Mario becomes a pancake
                squishAmount = spaceUnderCeil / 160.0f;
                vec3f_set(m->marioObj->header.gfx.scale, 2.0f - squishAmount, squishAmount,
                          2.0f - squishAmount);
            } else {
                if (m->invincTimer == 0) {
                    // cap on: 3 units; cap off: 4.5 units
                    m->hurtCounter += m->flags & MARIO_CAP_ON_HEAD ? 12 : 18;
                    play_sound_if_no_flag(m, SOUND_MARIO_ATTACKED, MARIO_MARIO_SOUND_PLAYED);
                }

                // Both of the 1.8's are really floats, but one of them has to
                // be written as a double for this to match on EU.
                vec3f_set(m->marioObj->header.gfx.scale, 1.8, 0.05f, 1.8f);
#ifdef VERSION_SH
                queue_rumble_data(10, 80);
#endif
                m->actionState = 1;
            }
            break;
        case 1:
            if (spaceUnderCeil >= 30.0f) {
                m->actionState = 2;
            }
            break;
        case 2:
            m->actionTimer++;
            if (m->actionTimer >= 15) {
                // 1 unit of health
                if (m->health < 0x0100) {
                    level_trigger_warp(m, WARP_OP_DEATH);
                    // woosh, he's gone!
                    set_mario_action(m, ACT_DISAPPEARED, 0);
                } else if (m->hurtCounter == 0) {
                    // un-squish animation
                    m->squishTimer = 30;
                    set_mario_action(m, ACT_IDLE, 0);
                }
            }
            break;
    }

    // steep floor
    if (m->floor != NULL && m->floor->normal.y < 0.5f) {
        surfAngle = atan2s(m->floor->normal.z, m->floor->normal.x);
        underSteepSurf = TRUE;
    }
    // steep ceiling
    if (m->ceil != NULL && -0.5f < m->ceil->normal.y) {
        surfAngle = atan2s(m->ceil->normal.z, m->ceil->normal.x);
        underSteepSurf = TRUE;
    }

    if (underSteepSurf) {
        m->vel[0] = sins(surfAngle) * 10.0f;
        m->vel[2] = coss(surfAngle) * 10.0f;
        m->vel[1] = 0;

        // check if there's no floor 10 units away from the surface
        if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
            // instant un-squish
            m->squishTimer = 0;
            set_mario_action(m, ACT_IDLE, 0);
            return FALSE;
        }
    }

    // squished for more than 10 seconds, so kill Mario
    if (m->actionArg++ > 300) {
        // 0 units of health
        m->health = 0x00FF;
        m->hurtCounter = 0;
        level_trigger_warp(m, WARP_OP_DEATH);
        // woosh, he's gone!
        set_mario_action(m, ACT_DISAPPEARED, 0);
    }
    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    return FALSE;
}

s32 act_putting_on_cap(struct MarioState *m) {
    s32 animFrame = set_mario_animation(m, MARIO_ANIM_PUT_CAP_ON);

    if (animFrame == 0) {
        enable_time_stop();
    }

    if (animFrame == 28) {
        cutscene_put_cap_on(m);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
        disable_time_stop();
    }

    stationary_ground_step(m);
    return FALSE;
}

void stuck_in_ground_handler(struct MarioState *m, s32 animation, s32 unstuckFrame, s32 target2,
                             s32 target3, s32 endAction) {
    s32 animFrame = set_mario_animation(m, animation);

    if (m->input & INPUT_A_PRESSED) {
        m->actionTimer++;
        if (m->actionTimer >= 5 && animFrame < unstuckFrame - 1) {
            animFrame = unstuckFrame - 1;
            set_anim_to_frame(m, animFrame);
        }
    }
    if (gCurrLevelNum != LEVEL_VCUTM) {
        stop_and_set_height_to_floor(m);
    }

    if (animFrame == -1) {
        m->terrainSoundAddend = 0;
        play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_STUCK_IN_GROUND, 1);
    } else if (animFrame == unstuckFrame) {
#ifdef VERSION_SH
        queue_rumble_data(5, 80);
#endif
        play_sound_and_spawn_particles(m, SOUND_ACTION_UNSTUCK_FROM_GROUND, 1);
    } else if (animFrame == target2 || animFrame == target3) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, endAction, 0);
    }
}

s32 act_head_stuck_in_ground(struct MarioState *m) {
    stuck_in_ground_handler(m, MARIO_ANIM_HEAD_STUCK_IN_GROUND, 96, 105, 135, ACT_IDLE);
    return FALSE;
}

s32 act_butt_stuck_in_ground(struct MarioState *m) {
    stuck_in_ground_handler(m, MARIO_ANIM_BOTTOM_STUCK_IN_GROUND, 127, 136, -2, ACT_GROUND_POUND_LAND);
    return FALSE;
}

s32 act_feet_stuck_in_ground(struct MarioState *m) {
    stuck_in_ground_handler(m, MARIO_ANIM_LEGS_STUCK_IN_GROUND, 116, 129, -2, ACT_IDLE);
    return FALSE;
}

/**
 * advance_cutscene_step: Advances the current step in the current cutscene.
 * Resets action state and action timer, adds 1 to the action arg (responsible
 * for keeping track of what step of the cutscene Mario is in.)
 */
void advance_cutscene_step(struct MarioState *m) {
    m->actionState = 0;
    m->actionTimer = 0;
    m->actionArg++;
}

#ifdef VERSION_EU
#define TIMER_SPAWN_PIPE 47
#else
#define TIMER_SPAWN_PIPE 37
#endif

#undef TIMER_SPAWN_PIPE

#ifdef VERSION_EU
#define TIMER_RAISE_PIPE 28
#else
#define TIMER_RAISE_PIPE 38
#endif

#undef TIMER_RAISE_PIPE

static s32 act_intro_cutscene(struct MarioState *m) {
    return FALSE;
}

// jumbo star cutscene: Mario lands after grabbing the jumbo star
static void jumbo_star_cutscene_falling(struct MarioState *m) {
}

// jumbo star cutscene: Mario takes off
static s32 jumbo_star_cutscene_taking_off(struct MarioState *m) {
}

// jumbo star cutscene: Mario flying
static s32 jumbo_star_cutscene_flying(struct MarioState *m) {
}

enum { JUMBO_STAR_CUTSCENE_FALLING, JUMBO_STAR_CUTSCENE_TAKING_OFF, JUMBO_STAR_CUTSCENE_FLYING };

static s32 act_jumbo_star_cutscene(struct MarioState *m) {
}

// not sure what this does, returns the height of the floor.
// (animation related?)
static f32 end_obj_set_visual_pos(struct Object *o) {
    struct Surface *surf;
    Vec3s sp24;
    f32 sp20;
    f32 sp1C;
    f32 sp18;

    find_mario_anim_flags_and_translation(o, o->header.gfx.angle[1], sp24);

    sp20 = o->header.gfx.pos[0] + sp24[0];
    sp1C = o->header.gfx.pos[1] + 10.0f;
    sp18 = o->header.gfx.pos[2] + sp24[2];

    return find_floor(sp20, sp1C, sp18, &surf);
}

// make Mario fall and soften wing cap gravity
static void end_peach_cutscene_mario_falling(struct MarioState *m) {
}

// set Mario on the ground, wait and spawn the jumbo star outside the castle.
static void end_peach_cutscene_mario_landing(struct MarioState *m) {
}

// raise hand animation, lower hand animation, do some special effects
static void end_peach_cutscene_summon_jumbo_star(struct MarioState *m) {
}

// free peach from the stained glass window
static void end_peach_cutscene_spawn_peach(struct MarioState *m) {
}

// descend peach
static void end_peach_cutscene_descend_peach(struct MarioState *m) {
}

// Mario runs to peach
static void end_peach_cutscene_run_to_peach(struct MarioState *m) {
}

// dialog 1
// "Mario!"
// "The power of the Stars is restored to the castle..."
static void end_peach_cutscene_dialog_1(struct MarioState *m) {
}

static void end_peach_cutscene_dialog_2(struct MarioState *m) {
}

static void end_peach_cutscene_kiss_from_peach(struct MarioState *m) {
}

static void end_peach_cutscene_star_dance(struct MarioState *m) {
}

// dialog 3
// "Listen everybody"
// "let's bake a delicious cake..."
// "...for Mario..."
static void end_peach_cutscene_dialog_3(struct MarioState *m) {
}

// "Mario!"
static void end_peach_cutscene_run_to_castle(struct MarioState *m) {
}

static void end_peach_cutscene_fade_out(struct MarioState *m) {
}

static s32 act_end_peach_cutscene(struct MarioState *m) {
    return FALSE;
}
static s32 act_credits_cutscene(struct MarioState *m) {
}

static s32 act_end_waving_cutscene(struct MarioState *m) {

    set_mario_animation(m, MARIO_ANIM_CREDITS_WAVING);
    stop_and_set_height_to_floor(m);

    m->marioObj->header.gfx.angle[1] += 0x8000;
    m->marioObj->header.gfx.pos[0] -= 60.0f;
    m->marioBodyState->handState = MARIO_HAND_RIGHT_OPEN;

    if (m->actionTimer++ == 300) {
        level_trigger_warp(m, WARP_OP_CREDITS_END);
    }

    return FALSE;
}

static s32 check_for_instant_quicksand(struct MarioState *m) {
    if (SURFACETYPE(m->floor) == SURFACE_INSTANT_QUICKSAND && m->action & ACT_FLAG_INVULNERABLE
        && m->action != ACT_QUICKSAND_DEATH) {
        return drop_and_set_mario_action(m, ACT_QUICKSAND_DEATH, 0);
    }
    return FALSE;
}

s32 mario_execute_cutscene_action(struct MarioState *m) {
    s32 cancel;

    if (check_for_instant_quicksand(m)) {
        return TRUE;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_DISAPPEARED:                cancel = act_disappeared(m);                break;
        case ACT_INTRO_CUTSCENE:             cancel = act_intro_cutscene(m);             break;
        case ACT_STAR_DANCE_EXIT:            cancel = act_star_dance(m);                 break;
        case ACT_STAR_DANCE_NO_EXIT:         cancel = act_star_dance(m);                 break;
        case ACT_STAR_DANCE_WATER:           cancel = act_star_dance_water(m);           break;
        case ACT_FALL_AFTER_STAR_GRAB:       cancel = act_fall_after_star_grab(m);       break;
        case ACT_READING_AUTOMATIC_DIALOG:   cancel = act_reading_automatic_dialog(m);   break;
        case ACT_READING_NPC_DIALOG:         cancel = act_reading_npc_dialog(m);         break;
        case ACT_READING_SIGN:               cancel = act_reading_sign(m);               break;
        case ACT_JUMBO_STAR_CUTSCENE:        cancel = act_jumbo_star_cutscene(m);        break;
        case ACT_WAITING_FOR_DIALOG:         cancel = act_waiting_for_dialog(m);         break;
        case ACT_STANDING_DEATH:             cancel = act_standing_death(m);             break;
        case ACT_QUICKSAND_DEATH:            cancel = act_quicksand_death(m);            break;
        case ACT_ELECTROCUTION:              cancel = act_electrocution(m);              break;
        case ACT_SUFFOCATION:                cancel = act_suffocation(m);                break;
        case ACT_DEATH_ON_STOMACH:           cancel = act_death_on_stomach(m);           break;
        case ACT_DEATH_ON_BACK:              cancel = act_death_on_back(m);              break;
        case ACT_EATEN_BY_BUBBA:             cancel = act_eaten_by_bubba(m);             break;
        case ACT_END_PEACH_CUTSCENE:         cancel = act_end_peach_cutscene(m);         break;
        case ACT_CREDITS_CUTSCENE:           cancel = act_credits_cutscene(m);           break;
        case ACT_END_WAVING_CUTSCENE:        cancel = act_end_waving_cutscene(m);        break;
        case ACT_PULLING_DOOR:
        case ACT_PUSHING_DOOR:               cancel = act_going_through_door(m);         break;
        case ACT_WARP_DOOR_SPAWN:            cancel = act_warp_door_spawn(m);            break;
        case ACT_EMERGE_FROM_PIPE:           cancel = act_emerge_from_pipe(m);           break;
        case ACT_SPAWN_SPIN_AIRBORNE:        cancel = act_spawn_spin_airborne(m);        break;
        case ACT_SPAWN_SPIN_LANDING:         cancel = act_spawn_spin_landing(m);         break;
        case ACT_EXIT_AIRBORNE:              cancel = act_exit_airborne(m);              break;
        case ACT_EXIT_LAND_SAVE_DIALOG:      cancel = act_exit_land_save_dialog(m);      break;
        case ACT_DEATH_EXIT:                 cancel = act_death_exit(m);                 break;
        case ACT_UNUSED_DEATH_EXIT:          cancel = act_unused_death_exit(m);          break;
        case ACT_FALLING_DEATH_EXIT:         cancel = act_falling_death_exit(m);         break;
        case ACT_SPECIAL_EXIT_AIRBORNE:      cancel = act_special_exit_airborne(m);      break;
        case ACT_SPECIAL_DEATH_EXIT:         cancel = act_special_death_exit(m);         break;
        case ACT_FALLING_EXIT_AIRBORNE:      cancel = act_falling_exit_airborne(m);      break;
        case ACT_UNLOCKING_KEY_DOOR:         cancel = act_unlocking_key_door(m);         break;
        case ACT_UNLOCKING_STAR_DOOR:        cancel = act_unlocking_star_door(m);        break;
        case ACT_ENTERING_STAR_DOOR:         cancel = act_entering_star_door(m);         break;
        case ACT_SPAWN_NO_SPIN_AIRBORNE:     cancel = act_spawn_no_spin_airborne(m);     break;
        case ACT_SPAWN_NO_SPIN_LANDING:      cancel = act_spawn_no_spin_landing(m);      break;
        case ACT_BBH_ENTER_JUMP:             cancel = act_bbh_enter_jump(m);             break;
        case ACT_BBH_ENTER_SPIN:             cancel = act_bbh_enter_spin(m);             break;
        case ACT_TELEPORT_FADE_OUT:          cancel = act_teleport_fade_out(m);          break;
        case ACT_TELEPORT_FADE_IN:           cancel = act_teleport_fade_in(m);           break;
        case ACT_SHOCKED:                    cancel = act_shocked(m);                    break;
        case ACT_SQUISHED:                   cancel = act_squished(m);                   break;
        case ACT_HEAD_STUCK_IN_GROUND:       cancel = act_head_stuck_in_ground(m);       break;
        case ACT_BUTT_STUCK_IN_GROUND:       cancel = act_butt_stuck_in_ground(m);       break;
        case ACT_FEET_STUCK_IN_GROUND:       cancel = act_feet_stuck_in_ground(m);       break;
        case ACT_PUTTING_ON_CAP:             cancel = act_putting_on_cap(m);             break;
    }
    /* clang-format on */

    if (!cancel) {
        if (m->input & INPUT_IN_WATER) {
            m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
        }
    }

    return cancel;
}
