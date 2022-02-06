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
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject

#define /*0x0F4*/ oBlink OBJECT_FIELD_S32(0x1B)
#define /*0x0F8*/ oTargetX OBJECT_FIELD_F32(0x1C)
#define /*0x0FC*/ oTargetY OBJECT_FIELD_F32(0x1E)
#define /*0x100*/ oTargetZ OBJECT_FIELD_F32(0x1F)
#define /*0x100*/ oSleepBubble OBJECT_FIELD_OBJ(0x20)

void yoshiBlinkManager(void) {
    o->oBlink++;
    if (o->oBlink > 90) {
        if (!(random_u16() & 0x03)) {
            o->oBlink = 0;
        }
    }
    if (o->oBlink < 2) {
        o->oAnimState = 1;
    } else if (o->oBlink < 4) {
        o->oAnimState = 2;
    } else {
        o->oAnimState = 0;
    }
    if (o->oBlink == 10) {
        if (!(random_u16() & 0x03)) {
            o->oBlink = 0;
        }
    }
}
/*
idle stand
look around
yoshi kinda vibing being excited
walk
fall
sit idle
jump
jumpdive
sleep
fkn dead
yoshi dancing*/

void generateTarget(void) {
    f32 x = 0;
    f32 z = 0;
    u16 targetyaw;
    f32 targetdist;
    while (sqrtf(x * x + z * z) < 250.f) {
        targetyaw = random_u16();
        targetdist = random_f32_around_zero(350.f) + 350.f;
        o->oTargetX = o->oHomeX + sins(targetyaw) * targetdist;
        o->oTargetZ = o->oHomeZ + coss(targetyaw) * targetdist;
        x = o->oTargetX - o->oPosX;
        z = o->oTargetZ - o->oPosZ;
    }
}

void yoshiWalkCheck(void) {
    if (o->header.gfx.unk38.animID == 0) {
        if (o->oTimer > 30) {
            if (!(random_u16() & 0xff)) {
                o->oAction = 1;
                generateTarget();
                return;
            }
        }
    }
    if (o->oTimer > 30) {
        if (o->header.gfx.unk38.animFrame == 49) {
            if (!(random_u16() & 0x3)) {
                cur_obj_init_animation(1);
                o->oTimer = 1;
            }
        }
    }
    if (o->header.gfx.unk38.animFrame == 89) {
        cur_obj_init_animation(0);
    }
}

void yoshiWalkToLocation(void) {
    f32 x, z;
    cur_obj_init_animation(2);
    o->header.gfx.unk38.animFrame += o->oForwardVel / 2.0f;
    x = o->oTargetX - o->oPosX;
    z = o->oTargetZ - o->oPosZ;
    o->oForwardVel =
        approach_f32(o->oForwardVel, (3.5f * (coss((atan2s(z, x) - o->oMoveAngleYaw) & 0xffff) + 1.0f)),
                     0.15f, 0.25f);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, atan2s(z, x), 0x280);
    if ((sqrtf(x * x + z * z) < 100.f) || o->oTimer > 40) {
        o->oForwardVel = approach_f32(o->oForwardVel, 0.f, 0.15f, 1.0f);
        if (o->oForwardVel < 3.0f) {
            o->oAction = 0;
            o->oForwardVel = 0;
            cur_obj_init_animation(0);
        }
    }
}

#define SAVE_FLAG_HAVE_KEY_1 /* 0x000010 */ (1 << 4)
#define SAVE_FLAG_HAVE_KEY_2 /* 0x000020 */ (1 << 5)
extern u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex);
#define STARID(level, starNum) (level * 8 + (starNum))

// unlock new mission every 3 stars
// unlock first mission at 10 stars
// each yoshi unlocks a new mission after 33 more stars
// last mission at 10 + 33 * 3 + 27 = 136 stars

/*0green - pretty formal, normal yoshi, llike in sm64
1black - wise/mature
2blue - calm and stoic
3cyan - crybaby
4orange - lazy
5pink - sassy
6purple - flamboyant
7red - easily upset, angry
8white - absent minded/introverted
9yellow - cheerful
10brown - stupid/slow*/

// text 0: default, text1: default past gate 2, text2-5: putting mario on a mission text
u8 yoshiTexts[11][6] = {
    { 176, 176, 177, 178, 179, 180 }, { 181, 181, 182, 183, 184, 185 },
    { 186, 186, 187, 188, 189, 190 }, { 191, 191, 192, 193, 194, 195 },
    { 170, 171, 172, 173, 174, 175 }, { 201, 201, 202, 203, 204, 205 },
    { 206, 206, 207, 208, 209, 210 }, { 211, 211, 212, 213, 214, 215 },
    { 216, 216, 217, 218, 219, 220 }, { 221, 221, 222, 223, 224, 225 },
    { 196, 196, 197, 198, 199, 200 },
};

u16 yoshiStars[11][4] = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    /*orange, lazy*/
    { STARID(LEVEL_VCUTM, 1), STARID(LEVEL_VCUTM, 2), STARID(LEVEL_VCUTM, 3),
      STARID(LEVEL_VCUTM, 4) }, // C1 Act 5
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

#define MISSION_START 0x8000
#define MISSIONID(level, actNum) (level * 8 + (actNum))
u16 yoshiMissions[11][4] = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    /*orange, lazy*/
    { MISSIONID(LEVEL_BOB, 5), 0, 0, 0 }, // C1 Act5
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

s16 whichMissionStar(u8 actNum) {
    u16 ID = MISSIONID(gCurrLevelNum, actNum) | 0x8000;
    u8 i, j;
    for (i = 0; i < 11; i++) {
        for (j = 0; j < 4; j++) {
            if (yoshiMissions[i][j] == ID) {
                return yoshiStars[i][j];
            }
        }
    }
    return -1;
}
void endMissionStar(u8 actNum) {
    u16 ID = MISSIONID(gCurrLevelNum, actNum) | 0x8000;
    u8 i, j;
    for (i = 0; i < 11; i++) {
        for (j = 0; j < 4; j++) {
            if (yoshiMissions[i][j] == ID) {
                yoshiMissions[i][j] &= 0x7fff;
            }
        }
    }
}
s8 checkMissionActive(u8 actNum) {
    int i, j;
    for (i = 0; i < 11; i++) {
        for (j = 0; j < 4; j++) {
            if (yoshiMissions[i][j] & MISSION_START) {
                if (((yoshiMissions[i][j] & 0x1FF) / 8) == gCurrLevelNum) {
                    if (((yoshiMissions[i][j] & 0x1FF) % 8) == (actNum + 1)) {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

extern s8 gLevelToCourseNumTable[];
u8 hasStar(u8 starID) {
    u16 staridentity = yoshiStars[o->oBehParams2ndByte][starID];
    return (save_file_get_star_flags(gCurrSaveFileNum - 1, gLevelToCourseNumTable[staridentity / 8])
            & (1 << (staridentity % 8)));
}

u16 yoshiGetText() {
    u16 textNUM = 0;
    s16 missionsunlocked = save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 0x18);
    int i;
    missionsunlocked = ((missionsunlocked + 23) - (o->oBehParams2ndByte * 3)) / 33; // ALWAYS ROUNDS
                                                                                    // DOWN
    for (i = 0; i < missionsunlocked; i++) {
        if (!hasStar(i)) {
            textNUM = i + 2;
            i = 99;
        }
    }

    if (textNUM == 0) {
        if ((save_file_get_star_flags(gCurrSaveFileNum - 1, 15) & 1)) {
            textNUM = 1;
        }
    } else {
        // spawn exclamation mark
        if (!o->oHiddenBlueCoinSwitch) {
            o->oHiddenBlueCoinSwitch = spawn_object(o, 0, bhvExclamationMark);
            o->oHiddenBlueCoinSwitch->oPosY += 250.f;
            o->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch = o;
        }
    }
    return textNUM;
}
u8 yoshiTalkToMario() {
    return talkToMario(yoshiTexts[o->oBehParams2ndByte][yoshiGetText()], 4);
}
void startMission(u16 *mission) {
    *mission |= MISSION_START;
}
void yoshiTalkAction(void) {
    o->oForwardVel = 0;
    cur_obj_init_animation(0);
    if (yoshiTalkToMario()) {
        o->oAction = o->oOpacity;
        o->oInteractStatus = 0;
        if (yoshiGetText() > 1) {
            startMission(&(yoshiMissions[o->oBehParams2ndByte][yoshiGetText() - 2]));
        }
    }
}
// action state 3 is always talking to mario
extern struct CutsceneJump *currentScene;
void yoshivillagercode(void) {
    u16 targetang2le;
    f32 x, z, minPos;
    if (mario_ready_to_speak() && (o->oInteractStatus == INT_STATUS_INTERACTED)) {
        o->oOpacity = o->oAction;
        o->oAction = 3;
    }
    yoshiBlinkManager();
    if (currentScene) {
        if (o->oBehParams2ndByte == 4) {
            // lazy, sit and sleep instead of walking
            if (o->oAnimState == 0) {
                o->oAnimState = 6;
            }
            switch (o->oAction) {
                case 0:
                    o->oMoveAngleYaw = 0x4000;
                    cur_obj_init_animation(5);
                    o->oAnimState = 8;
                    if (!o->oSleepBubble) {
                        o->oSleepBubble = spawn_object(o, MODEL_BUBBLE, bhvYoshiBubble);
                        o->oSleepBubble->parentObj = o;
                    }
                    if (o->oTimer == 1565) { // todo adjust
                        o->oAction = 1;
                        play_sound(SOUND_GENERAL_YOSHI_WAAH, gDefaultSoundArgs);
                        cur_obj_init_animation(6);
                    }
                    break;
                case 1:
                    if (o->oTimer == 38) {
                        o->oAction = 2;
                    }
                    break;
                case 2:
                    cur_obj_init_animation(1);
                    if (o->oTimer == 89) {
                        o->oAction = 3;
                    }
                    break;
                case 3:
                    cur_obj_init_animation(0);
                    break;
            }
        } else {
            switch (o->oAction) {
                case 0:
                    cur_obj_init_animation(0);
                    if (cur_obj_nearest_object_with_behavior(bhvCutsceneObjectFruit)) {
                        o->oAction = 1;
                    }
                    break;
                case 1:
                    if (o->oTimer > 25) {
                        o->oAction = 2;
                        play_sound(SOUND_GENERAL_YOSHI_WOAH, gDefaultSoundArgs);
                        o->oObjPointer1 =
                            cur_obj_nearest_object_with_behavior(bhvCutsceneObjectFruit)->oObjPointer1;
                        o->oHomeX = o->oObjPointer1->oPosX;
                        o->oHomeZ = o->oObjPointer1->oPosZ;
                    }
                    break;
                case 2:
                    cur_obj_init_animation(2);
                    o->header.gfx.unk38.animFrame += o->oForwardVel / 2.0f;
                    targetang2le = obj_angle_to_object(
                        o, cur_obj_nearest_object_with_behavior(bhvCutsceneObjectFruit));
                    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetang2le, 0x280);
                    o->oForwardVel = approach_f32(
                        o->oForwardVel,
                        (13.5f * (coss((targetang2le - o->oMoveAngleYaw) & 0xffff) + 1.0f))
                            * (cur_obj_dist_to_nearest_object_with_behavior(bhvCutsceneObjectFruit)
                               - 380.f),
                        0.5f, 0.75f);
                    if (o->oTimer > 40) {
                        if (o->oForwardVel < 2.0f) {
                            o->oForwardVel = 0;
                            cur_obj_init_animation(0);
                        }
                    }
                    if (o->oTimer > 110) {
                        o->oAction = 3;
                        o->oForwardVel = 0;
                        cur_obj_init_animation(0);
                    }
                    break;
                case 3:
                    if (o->oTimer == 18) {
                        play_sound(SOUND_GENERAL_YOSHI_WAAH, gDefaultSoundArgs);
                        cur_obj_init_animation(7);
                        o->oAnimState = 3;
                    }
                    break;
            }
            if (o->oAction > 2) {
#define downMinOffset -2050.f
                o->oPosX += o->oObjPointer1->oPosX - o->oHomeX;
                o->oPosZ += o->oObjPointer1->oPosZ - o->oHomeZ;
                o->oHomeX = o->oObjPointer1->oPosX;
                o->oHomeZ = o->oObjPointer1->oPosZ;

                cur_obj_update_floor_and_walls();
                minPos = downMinOffset * o->oObjPointer1->header.gfx.scale[1]
                         + o->oObjPointer1->oGraphYOffset + o->oObjPointer1->oPosY - 50.f;
                // 845radius
                // 720 max pos
                x = o->oPosX - o->oObjPointer1->oPosX;
                z = o->oPosZ - o->oObjPointer1->oPosZ;
                minPos += (sqrtf(x * x + z * z) / 845.f) * (sqrtf(x * x + z * z) / 845.f) * 1440.f
                          * o->oObjPointer1->header.gfx.scale[1];
                if (minPos > o->oFloorHeight) {
                    o->oFloorHeight = minPos;
                }
                o->oAnimState = 4;
                cur_obj_move_standard(-78);
                return;
            }
        }
    } else {
        switch (o->oBehParams2ndByte) {
            case 0:
            case 1:
            case 2:
            case 5:
                switch (o->oAction) {
                    case 0:
                        yoshiWalkCheck();
                        break;
                    case 1:
                        yoshiWalkToLocation();
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 3:
                // crybaby
                switch (o->oAction) {
                    case 0:
                        yoshiWalkCheck();
                        if (o->oAnimState == 0) {
                            o->oAnimState = 3;
                        }
                        break;
                    case 1:
                        yoshiWalkToLocation();
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 4:
                // lazy, sit and sleep instead of walking
                if (o->oAnimState == 0) {
                    o->oAnimState = 6;
                }
                switch (o->oAction) {
                    case 0:
                        o->oInteractType = INTERACT_IGLOO_BARRIER;
                        cur_obj_init_animation(5);
                        o->oAnimState = 8;
                        if (!o->oSleepBubble) {
                            o->oSleepBubble = spawn_object(o, MODEL_BUBBLE, bhvYoshiBubble);
                            o->oSleepBubble->parentObj = o;
                        }
                        if ((o->oDistanceToMario < 1500.f)
                            && (gMarioState->action == ACT_GROUND_POUND_LAND)) {
                            o->oAction = 1;
                            play_sound(SOUND_GENERAL_YOSHI_WAAH, gDefaultSoundArgs);
                            cur_obj_init_animation(6);
                        }
                        if (abs_angle_diff(gMarioState->faceAngle[1],
                                           obj_angle_to_object(gMarioState->marioObj, o))
                            < 0x2000) {
                            if (lateral_dist_between_objects(gMarioState->marioObj, o)
                                < (o->hitboxRadius + 50.f)) {
                                if ((gMarioState->pos[1]
                                     < (o->oPosY - o->hitboxDownOffset + o->hitboxHeight))) {
                                    if ((gMarioState->pos[1] > (o->oPosY - o->hitboxDownOffset))) {
                                        if (gMarioState->action & ACT_FLAG_ATTACKING) {
                                            o->oAction = 2;
                                            gMarioState->forwardVel = -30.f;
                                            cur_obj_play_sound_2(SOUND_ACTION_HIT_3);
                                            o->oAction = 1;
                                            play_sound(SOUND_GENERAL_YOSHI_WAAH, gDefaultSoundArgs);
                                            cur_obj_init_animation(6);
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    case 1:
                        if (o->oTimer == 36) {
                            o->oAction = 2;
                        }
                        break;
                    case 2:
                        o->oInteractType = INTERACT_TEXT;
                        cur_obj_init_animation(0);
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 6:
                // flamboyant
                if (o->oAnimState == 0) {
                    o->oAnimState = 5;
                }
                switch (o->oAction) {
                    case 0:
                        yoshiWalkCheck();
                        break;
                    case 1:
                        yoshiWalkToLocation();
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 7:
                // angry
                if (o->oAnimState == 0) {
                    o->oAnimState = 7;
                }
                switch (o->oAction) {
                    case 0:
                        yoshiWalkCheck();
                        break;
                    case 1:
                        yoshiWalkToLocation();
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 8:
                // absent minded/introverted, sit next to water
                if (o->oAnimState == 0) {
                    o->oAnimState = 1;
                }
                switch (o->oAction) {
                    case 0:
                        yoshiWalkCheck();
                        break;
                    case 1:
                        yoshiWalkToLocation();
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 9:
                // cheerful, just vibe
                switch (o->oAction) {
                    case 0:
                        cur_obj_init_animation(3);
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
            case 10:
                // cool yosh
                o->oAnimState = 10;
                switch (o->oAction) {
                    case 0:
                        yoshiWalkCheck();
                        break;
                    case 1:
                        yoshiWalkToLocation();
                        break;
                    case 2:
                        break;
                    case 3:
                        yoshiTalkAction();
                        break;
                }
                break;
        }
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
}

void yoshivillagerinitcode(void) {
    o->oMoveAngleYaw = random_u16();
    if (!((save_file_get_star_flags(gCurrSaveFileNum - 1, 15) & 1))) {
        if ((o->oBehParams2ndByte != 4) && (gMarioState->statusForCamera->cameraEvent != 0xc8)) {
            obj_mark_for_deletion(o);
        }
    }
    if (o->activeFlags) {
        yoshiGetText();
    }
}
void fatYosherInit(){
    if (!((save_file_get_star_flags(gCurrSaveFileNum - 1, 15) & 1))) {
            obj_mark_for_deletion(o);
    }
}
void parakarrycode(void) {
    if (o->header.gfx.unk38.animFrame == 14) {
        cur_obj_play_sound_2(SOUND_GENERAL_SWISH_AIR);
    }
    switch (o->oAction) {
        case 0:
            if (gMarioState->pos[2] > 100.f) {
                o->oAction = 1;
            }
            break;
        case 1:
            if (talkToMario(6, 1)) {
                o->oAction = 2;
            }
            break;
        case 2:
            o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, 0x4000, 0x200);
            o->oMoveAngleYaw = o->oFaceAngleYaw;
            if (o->oForwardVel < 30.f) {
                o->oForwardVel += 0.6f;
            }
            if (o->oVelY < 30.f) {
                o->oVelY += .4f;
            }
        obj_move_xyz_using_fvel_and_yaw();
            break;
    }
}

void yoshiExclamationmarkcode(void) {
    switch (o->oAction) {
        case 0:
            cur_obj_set_model(0xFD);
            cur_obj_scale(1.f + 0.125f * sins(o->oTimer * 512));
            if (!(o->oTimer % 10)) {
                o->oAnimState++;
            }
            if (o->oHiddenBlueCoinSwitch->oAction == 3) {
                o->oAction = 1;
            }
            break;
        case 1:
            if (o->oTimer < 3) {
                cur_obj_scale(o->header.gfx.scale[0] + 0.25f);
            } else {
                spawn_mist_particles();
                mark_obj_for_deletion(o);
            }
            break;
    }
}

extern u8 sSelectedActIndex;
void actYoshoiCode(void) {
    // CuckyDev: Star Select transition
    switch (o->oAction) {
        case 0:
            o->oAction = 1;
            o->oOpacity = -130;
            // Fallthrough
        case 1:
            o->oPosY += ((s32) o->oOpacity - o->oPosY) * 0.1f;
            o->oOpacity -= 3;
            if ((s32) o->oOpacity < -215)
                o->oOpacity = -215;
            break;
    }

    if (sSelectedActIndex != o->oOpacity) {
        cur_obj_init_animation(8);
        if (o->header.gfx.unk38.animFrame > 58) {
            o->header.gfx.unk38.animFrame = 19;
        }
        o->oAnimState = 4;
    } else {
        cur_obj_init_animation(3);
        o->oAnimState = 9;
    }
}

#define CIRCLESIZE 8300.f
#define ADD 0x10
#define CIRCLEOFFSET 0xC00
#define ADDDIR o->oMacroUnk108
#define DANEHEIGHT 850.f

#define CIRCLESIZE2 10300.f
#define CIRCLEOFFSET2 0x6E00
#define ADDDIR o->oMacroUnk108
void seagullcode(void) {
    if (!o->oTimer) {
        o->oAnimState = random_u16() % 9;
        o->oOpacity = random_u16() & 0x7FFF;
        ADDDIR = (random_u16() & 1) * 2 - 1;
        while (o->oBehParams) {
            spawn_object(o, 0x41, bhvSeagull);
            o->oBehParams--;
        }
        o->oPosY = DANEHEIGHT + random_f32_around_zero(950.f);
    }
    o->oOpacity = (u32) (o->oOpacity + ADD * ADDDIR) & 0x7FFF;
    if (gCurrAreaIndex == 1) {
        o->oFaceAngleYaw = atan2s(o->oPosZ - gLakituState.pos[2], o->oPosX - gLakituState.pos[0])
                           + 0x4000 + 0x4000 * ADDDIR;
        o->oPosX = sins(o->oOpacity + CIRCLEOFFSET) * CIRCLESIZE;
        o->oPosZ = coss(o->oOpacity + CIRCLEOFFSET) * CIRCLESIZE;
    } else {
        o->oFaceAngleYaw = atan2s(o->oPosZ - gLakituState.pos[2], o->oPosX - gLakituState.pos[0])
                           + 0x4000 + 0x4000 * ADDDIR;
        o->oPosX = sins(o->oOpacity + CIRCLEOFFSET2) * CIRCLESIZE2;
        o->oPosZ = coss(o->oOpacity + CIRCLEOFFSET2) * CIRCLESIZE2 - 3500.f;
    }
    if (!(o->oTimer & 0x01)) {
        o->oAnimState++;
    }
}
// look the same way as the karate yoshi. see the 2 twin trees? stand right between them. look at big
// vine. now do 5 backflips and groundpound. you will find a treasure.
void seaShell(void) {
    switch (o->oAction) {
        case 0:
            o->oInteractStatus = 0;
            o->oIntangibleTimer = 0;
            if (!o->oTimer) {
                o->oPosY -= 200.f;
            }
            if (lateral_dist_between_objects(gMarioState->marioObj, o) < 400.f) {
                if (gMarioState->action == ACT_GROUND_POUND_LAND) {
                    o->oAction++;
                    o->oVelY = 40.f;
                }
            }
            break;
        case 1:
            o->oPosY += o->oVelY;
            if (o->oTimer > 10) {
                cur_obj_update_floor_and_walls();
                cur_obj_move_standard(78);
            } else {
                o->oVelY -= 4.f;
            }
            if (o->oTimer > 20) {
                o->oInteractType = INTERACT_BBH_ENTRANCE;
            }
            break;
    }
}

void mechBowser(void) {
    u32 sp34;
    struct Object *flame;
    o->oInteractStatus = 0;
    o->oIntangibleTimer = 0;
    switch (o->header.gfx.unk38.animFrame) {
        case 0:
        case 79:
        case 159:
        case 211:
        case 229:
        case 270:
        case 297:
            cur_obj_play_sound_2(SOUND_GENERAL_BIG_CLOCK);
            break;
    }
    if ((o->header.gfx.unk38.animFrame > 224) && (o->header.gfx.unk38.animFrame < 274)) {
        if (o->header.gfx.unk38.animFrame < 259)
            sp34 = 15;
        else
            sp34 = 275 - o->header.gfx.unk38.animFrame; // Range: [15..2]
        o->oFlameThowerUnk110 = sp34;
        flame = spawn_object_relative(o->oBehParams2ndByte, 0, 779, 305, o, MODEL_RED_FLAME,
                                      bhvFlamethrowerFlame);
        flame->oForwardVel = 95.f;
        flame->oVelY = 20.f;
        cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
        // cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
    } else if (((o->header.gfx.unk38.animFrame > 0) && (o->header.gfx.unk38.animFrame < 65))
               || ((o->header.gfx.unk38.animFrame > 79) && (o->header.gfx.unk38.animFrame < 145))
               || ((o->header.gfx.unk38.animFrame > 159) && (o->header.gfx.unk38.animFrame < 230))
               || ((o->header.gfx.unk38.animFrame > 269) && (o->header.gfx.unk38.animFrame < 285))) {

        // cur_obj_play_sound_1(SOUND_ENV_ELEVATOR2);
        cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
    }
}

void bounceball(void) {
    o->oIntangibleTimer = 0;
    if (o->oInteractStatus) {
        gMarioState->vel[1] = 200.f;
    }
    o->oInteractStatus = 0;
}

void fatyoshercode(void) {
    s32 i;
    switch (o->oAction) {
        case 0:
            if (o->oSubAction) {
                switch (i = askMario(226, 1, 2)) {
                    case 0:
                        break;
                    case 1:
                        // yes
                        o->oAction = 1;
                        cur_obj_init_animation(1);
                        play_sound(SOUND_GENERAL_YOSHI_PAM, gDefaultSoundArgs);
                        m->action = ACT_WAITING_FOR_DIALOG;
                        break;
                    case 2:
                        // no
                        o->oAction = 2;
                        break;
                    case 3:
                        break;
                }
            } else {
                if (o->oDistanceToMario < 400.f) {
                    o->oSubAction = 1;
                }
            }
            break;
        case 1:
            if (o->header.gfx.unk38.animFrame == 9) {
                o->oForwardVel = -8.f;
            }
            if (o->header.gfx.unk38.animFrame > 32) {
                o->oForwardVel *= .8f;
            }
            if (o->oTimer == 30) {
                cur_obj_play_sound_2(SOUND_OBJ_POUNDING1_HIGHPRIO);
                spawn_mist_particles();
            }
            if (o->oTimer == 38) {
                cur_obj_play_sound_2(SOUND_OBJ_POUNDING1_HIGHPRIO);
                spawn_mist_particles();
            }
            o->hitboxHeight = 100.f;
            o->hitboxRadius = 260.f;
            if (o->oTimer == 44) {
                o->oAction = 4;
                m->action = ACT_IDLE;
                o->oDialogState = 0;
            }
        obj_move_xyz_using_fvel_and_yaw();
            break;
        case 2:
            if (o->oDistanceToMario > 700) {
                o->oAction = 0;
            }
            break;
        case 3:
            // scale normale
            o->header.gfx.scale[1] = approach_f32_symmetric(o->header.gfx.scale[1], 1.f, .05f);
            if (o->oTimer > 100) {
                o->oAction = 4;
            }
            break;
        case 4:
            if (!o->oSubAction) {
                if (talkToMarioNoRotation(227, 2)) {
                    o->oSubAction = 1;
                }
            } else if (o->oSubAction == 2) {
                if (o->oTimer < 8) {
                    o->header.gfx.scale[1] -= .05f;
                    gMarioState->vel[1] = -10.f;
                    gMarioState->pos[0] = approach_f32_symmetric(gMarioState->pos[0], o->oPosX, 5.f);
                    gMarioState->pos[2] = approach_f32_symmetric(gMarioState->pos[2], o->oPosZ, 5.f);
                } else {
                    gMarioState->action = ACT_TRIPLE_JUMP;
                    gMarioState->vel[1] = 200.f;
                    cur_obj_play_sound_2(SOUND_OBJ_SNOWMAN_BOUNCE);
                    o->oAction = 3;
                }
            } else {
                if (lateral_dist_between_objects(o, gMarioState->marioObj) < 200.f) {
                    if (-o->oPosY + gMarioState->pos[1] < 260.f) {
                        o->oSubAction = 2;
                        o->oTimer = 0;
                        o->oIntangibleTimer = -1;
                    }
                }
            }
            break;
    }
    if (o->oAction != 4) {
        o->oIntangibleTimer = 0;
    }
    vec3f_set(o->header.gfx.scale, sqrtf(1.f / o->header.gfx.scale[1]), o->header.gfx.scale[1],
              sqrtf(1.f / o->header.gfx.scale[1]));
}