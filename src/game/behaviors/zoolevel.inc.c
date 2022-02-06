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

extern Vtx tireswing_taiya1_001_mesh_layer_1_vtx_1[28];
void tireswing(void) {
    Vtx *a = segmented_to_virtual(tireswing_taiya1_001_mesh_layer_1_vtx_1);
    u32 i;
    if (gMarioState->wall) {
        if (gMarioState->wall->object == o) {
            o->oTimer = 0;
        }
    }
    if (gMarioState->particleFlags & (PARTICLE_VERTICAL_STAR | PARTICLE_TRIANGLE)) {
        if (o->oTimer < 2) {
            if ((obj_angle_to_object(o, gMarioObject) - o->oFaceAngleYaw + 0x4000) & 0x8000) {
                o->oAngleVelPitch = o->oMacroUnk108 * -0x8 - 0x300;
            } else {
                o->oAngleVelPitch = o->oMacroUnk108 * 0x8 + 0x300;
            }
        }
    }
    o->oMacroUnk108 = gMarioState->forwardVel;
    o->oFaceAnglePitch += o->oAngleVelPitch;
    o->oAngleVelPitch *= 0.98f;
    o->oAngleVelPitch -= o->oFaceAnglePitch * 0.025f;
    for (i = 0; i < 28; i++) {
        if (a[i].v.ob[1] == -207) {
            if (!a[i].v.flag) {
                a[i].v.flag = a[i].v.ob[2];
            }
            a[i].v.ob[2] = a[i].v.flag + o->oFaceAnglePitch * 0.008f;
        }

        if (a[i].v.ob[1] == -457) {
            if (!a[i].v.flag) {
                a[i].v.flag = a[i].v.ob[2];
            }
            a[i].v.ob[2] = a[i].v.flag + o->oFaceAnglePitch * 0.016f;
        }
    }
}

void tireSwingGoomba(void) {
    struct Object *spawn;
    cur_obj_init_animation(0);
    cur_obj_scale(1.5f);
    if (!o->oHiddenBlueCoinSwitch) {
        o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvTireSwing);
    }
    if (!(o->oTimer & 0x3f)) {
        if (o->oTimer & 0x40) {
            o->oHiddenBlueCoinSwitch->oAngleVelPitch += 0x080;
        } else {
            o->oHiddenBlueCoinSwitch->oAngleVelPitch -= 0x080;
        }
    }
    o->oPosX = o->oHomeX
               - sins(o->oHiddenBlueCoinSwitch->oMoveAngleYaw)
                     * sins(o->oHiddenBlueCoinSwitch->oFaceAnglePitch) * 890.f;
    o->oPosZ = o->oHomeZ
               - coss(o->oHiddenBlueCoinSwitch->oMoveAngleYaw)
                     * sins(o->oHiddenBlueCoinSwitch->oFaceAnglePitch) * 890.f;
    if (o->oDistanceToMario < 500.f) {
        mark_obj_for_deletion(o);
        spawn = spawn_object(o, MODEL_GOOMBA, bhvGoomba);
        spawn->parentObj = spawn;
    }
}

void bhv_goomba_sleep(void) {
    struct Object *spawn;
    o->oIntangibleTimer = 0;
    o->oInteractStatus = 0;
    cur_obj_init_animation(1);
    if (!o->oAction) {
        if (o->header.gfx.unk38.animFrame > 79) {
            o->header.gfx.unk38.animFrame = 0;
        }
        if (o->oDistanceToMario < 500.f) {
            o->oAction = 1;
            o->header.gfx.unk38.animFrame = 79;
        }
    }
    if (o->header.gfx.unk38.animFrame < 81) {
        o->oAnimState = 1;
    } else {
        o->oAnimState = 0;
    }
    if (o->header.gfx.unk38.animFrame > 98) {
        mark_obj_for_deletion(o);
        spawn = spawn_object(o, MODEL_GOOMBA, bhvGoomba);
        spawn->oBehParams2ndByte = 1;
        spawn->parentObj = spawn;
    }
    cur_obj_scale(3.5f);
}

void floatboat(void) {
    if (cur_obj_is_mario_on_platform()) {
        o->oVelY -= 1.0f;
    }
    o->oVelY += (o->oHomeY - o->oPosY) / 30.f;
    o->oVelY += sins(o->oTimer * 0x200) * 0.25f;
    o->oVelY *= 0.90f;
    o->oPosY += o->oVelY;
    load_object_collision_model();
}

void musclepengu(void) {
    switch (o->oAction) {
        case 0:
            // idle
            cur_obj_init_animation(0);
            if (o->oDistanceToMario < 250.f) {
                o->oAction = 1;
            }
            break;
        case 1:
            // talk
            switch (askMario(o->oBehParams & 0xFF, 1, 2)) {
                case 0:
                    break;
                case 1:
                    // yes
                    o->oAction = 2;
                    break;
                case 2:
                    // no
                    o->oAction = 3;
                    break;
                case 3:
                    break;
            }
            break;
        case 2:
            // throw
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            cur_obj_init_animation(1);
            if (o->header.gfx.unk38.animFrame == 22) {
                if (o->oDistanceToMario < 300.f) {
                    if (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x1000) {
                        if (o->oBehParams & 0x80) {
                            m->vel[1] = 80.f;
                            m->forwardVel = -30.f;
                        } else if (o->oBehParams & 0x0100) {
                            m->vel[1] = 115.f;
                            m->forwardVel = -25.f;
                        } else {
                            m->vel[1] = 95.f;
                            m->forwardVel = -115.f;
                        }
                        m->action = ACT_THROWN_BACKWARD;
                        play_sound(SOUND_MARIO_WAAAOOOW, gDefaultSoundArgs);
                        cur_obj_play_sound_2(SOUND_OBJ_HEAVEHO_TOSSED);
                        gMarioObject->oInteractStatus |= INT_STATUS_MARIO_UNK2;
                        m->faceAngle[1] = o->oMoveAngleYaw;
                    }
                }
            }
            if (o->header.gfx.unk38.animFrame >= 53) {
                o->oAction = 0;
            }
            break;
        case 3:
            // idle
            cur_obj_init_animation(0);
            if ((o->oDistanceToMario > 500.f) || (o->oTimer > 60)) {
                o->oAction = 0;
            }
            break;
    }
}

u8 gateOpen = 0;
void makeTalkable() {
    gateOpen = 0;
    o->oInteractionSubtype = INT_SUBTYPE_NPC;
    o->oIntangibleTimer = 0;
    o->oInteractType = INTERACT_TEXT;
    o->oFlags |= OBJ_FLAG_COMPUTE_ANGLE_TO_MARIO | OBJ_FLAG_HOLDABLE | OBJ_FLAG_COMPUTE_DIST_TO_MARIO
                 | OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW | OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE;
}
extern void obj_update_blinking(s32 *blinkTimer, s16 baseCycleLength, s16 cycleLengthRange,
                                s16 blinkLength);
void sneakpengu(void) {
    if (o->oBehParams >> 24) {

        smartBossActOne();
        o->oInteractStatus = 0;
        return;
    }
    obj_update_blinking(&o->oOpacity, 40, 40, 6);
    switch (o->oAction) {
        case 0:
            if (o->oSubAction) {
                if (o->oDistanceToMario < 750.f) {
                    o->oAction = 1;
                }
            } else {
                if (o->oDistanceToMario > 1150.f) {
                    o->oSubAction = 1;
                }
            }
            break;
        case 1:
            if (talkToMario(o->oBehParams & 0xFF, 4)) {
                o->oAction = 0;
            }
            break;
        case 2:
            if (o->oTimer > 10) {
                if (talkToMario(59, 4)) {
                    o->oAction = 3;
                    spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 250.f,
                                       gMarioState->pos[2]);
                }
            }
            break;
        case 3:
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x400);
            o->oInteractType = INTERACT_IGLOO_BARRIER;
            break;
    }
    o->oInteractStatus = 0;
}
u8 huntIsOn;
void smartBossActOne() {
    struct Object *banana;
    switch (o->oAction) {
        case 0:
            if (mario_ready_to_speak() && (o->oInteractStatus == INT_STATUS_INTERACTED)) {
                o->oAction = 1;
            }
            break;
        case 1:
            if (talkToMario(o->oBehParams & 0xFF, 4)) {
                o->oAction = 0;
                gateOpen |= 1 + (segmented_to_virtual(bhvBossPenguin) == o->behavior);
                if ((o->oBehParams & 0xFF) == 0x49) {
                    /*banana = spawn_object(o, 0x47, bhvBananaZoo);
                    banana->oVelY = 30.f;
                    banana->oForwardVel = 10.f;*/
                    huntIsOn = 1;
                }
            }
            break;
        case 2:
            // make text once mario comes close, give mario star afterwards, set Bparam4 to the new text
            // ID
            if (o->oDistanceToMario < 500.f) {
                o->oAction = 3;
                o->oBehParams = (o->oBehParams & 0x00FFFF00) + 0x0100007A;
            }
            break;
        case 3:
            if (talkToMario(o->oBehParams & 0xFF, 4)) {
                o->oAction = 0;
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 250.f,
                                   gMarioState->pos[2]);
            }
            break;
    }
    if (huntIsOn > 5) {
        if (!o->oOpacity) {
            if ((o->oBehParams & 0xFF) == 0x49) {
                o->oAction = 2;
            }
        }
    }
}
void smartpengu(void) {
    smartBossActOne();
    o->oInteractStatus = 0;
}

void bosspengu(void) {
    smartBossActOne();
    o->oInteractStatus = 0;
}
extern const Collision tablebroken_collision[];
void tablebreaking(void) {
    u32 *collisionData = segmented_to_virtual(tablebroken_collision);
    switch (o->oAction) {
        case 0:
            if (lateral_dist_between_objects(o, gMarioState->marioObj) < 250.f) {
                if (cur_obj_is_mario_ground_pounding_platform()) {
                    o->oAction = 1;
                    spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
                    spawn_mist_particles_variable(0, 0, 46.f);
                    spawn_triangle_break_particles(30, 138, 3.0f, 4);
                    cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION6);
                }
            }
            break;
        case 1:
            gCurrentObject->collisionData = collisionData;
            cur_obj_set_model(0x3E);
            break;
    }
    load_object_collision_model();
}

extern const Collision wariocarbroken_collision[];
void wariocar(void) {
    u32 *collisionData = segmented_to_virtual(wariocarbroken_collision);
    switch (o->oAction) {
        case 0:
            if (cur_obj_dist_to_nearest_object_with_behavior(bhvChainChomp) < 500.f){
                
                o->oAction = 1;
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION7);
            }
            break;
        case 1:
            gCurrentObject->collisionData = collisionData;
            cur_obj_set_model(0x48);
            break;
    }
    load_object_collision_model();
}

void waluigi(void) {
    switch (o->oAction) {
        case 0:
            cur_obj_init_animation(0);
            if (o->oDistanceToMario < 500.f) {
                o->oAction++;
            }
            break;
        case 1:
            cur_obj_init_animation(1);
            if (o->header.gfx.unk38.animFrame > 68) {
                o->header.gfx.unk38.animFrame = 20;
            }
            if (talkToMarioNoRotation(68, 2)) {
                o->oAction++;
            }
            break;
        case 2:
            if (cur_obj_check_if_at_animation_end()) {
                cur_obj_init_animation(0);
            }
            if (o->oDistanceToMario > 1000.f) {
                if (!o->header.gfx.unk38.animID) {
                    o->oAction = 0;
                }
            }
            break;
    }
}

void wariogate(void) {
    switch (o->oAction) {
        case 0:
            if (gateOpen == 3) {
                o->oAction++;
            }
            break;
        case 1:
            if (set_mario_npc_dialog(2) == 2 && cutscene_object(CUTSCENE_PREPARE_CANNON, o) == 1) {
                o->oAction = 2;
                play_sound(SOUND_GENERAL_WATER_LEVEL_TRIG, gDefaultSoundArgs);
            }
            break;
        case 2:
            if (o->oTimer < 100) {
                o->oPosY += 3.f;
            } else {
                gObjCutsceneDone = TRUE;
                if (cutscene_object(CUTSCENE_PREPARE_CANNON, o) == -1) {
                    set_mario_npc_dialog(0);
                    o->oAction = 3;
                    mark_obj_for_deletion(o);
                }
            }
            break;
    }
    load_object_collision_model();
}
u32 hasVision() {
    struct Surface *surfback = 0;
    Vec3f camdirback;
    Vec3f hitposback;
    Vec3f startPos;
    if (absf(o->oPosY - gMarioState->pos[1]) > 500.f) {
        return 0;
    }
    if ((abs_angle_diff(o->oFaceAngleYaw, o->oAngleToMario) > 0x1400)
        || (o->oDistanceToMario > 2200.f)) {
        return 0;
    }
    camdirback[0] = gMarioState->pos[0] - o->oPosX;
    camdirback[1] = gMarioState->pos[1] - o->oPosY;
    camdirback[2] = gMarioState->pos[2] - o->oPosZ;
    startPos[0] = o->oPosX;
    startPos[1] = o->oPosY + 75.f;
    startPos[2] = o->oPosZ;
    find_surface_on_ray(startPos, camdirback, &surfback, &hitposback, 3);
    if (surfback) {
        return 0;
    }

    camdirback[0] = -gMarioState->pos[0] + o->oPosX;
    camdirback[1] = -gMarioState->pos[1] + o->oPosY;
    camdirback[2] = -gMarioState->pos[2] + o->oPosZ;
    startPos[0] = gMarioState->pos[0];
    startPos[1] = gMarioState->pos[1] + 75.f;
    startPos[2] = gMarioState->pos[2];
    find_surface_on_ray(startPos, camdirback, &surfback, &hitposback, 3);
    if (surfback) {
        return 0;
    }

    return 1;
}

void patrolBobOmb(void) {
    struct Object *explosion;
    switch (o->oAction) {
        case 0:
            o->oAction = o->oBehParams2ndByte;
            break;
        case 1:
            // up and down
            switch (o->oSubAction) {
                case 0:
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 15.f, 2.f);
                    if (o->oTimer > (o->oBehParams & 0xFF)) {
                        o->oSubAction = 1;
                        o->oTimer = 0;
                    }
                    break;
                case 1:
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 2.f);
                    if (o->oTimer > 15) {
                        o->oMoveAngleYaw += 0x800;
                        if (o->oTimer > 30) {
                            o->oSubAction = 0;
                            o->oTimer = 0;
                        }
                    }
                    break;
            }
            break;
        case 2:
            o->oForwardVel = 16.f;
            o->oMoveAngleYaw += 0x1c0;

            //            o->oPosX = sins(o->oMoveAngleYaw + 0x4000) * 300.f + o->oHomeX;
            //            o->oPosZ = coss(o->oMoveAngleYaw + 0x4000) * 300.f + o->oHomeZ;
            // in a circle
            break;
        case 3:
            // stand
            break;
        case 4:
            if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
                o->oVelY = 60.f;
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 55.f, 2.f);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x800);
            if ((o->oTimer > 100)) {
                o->oAction = 6;
            }
            if ((o->oDistanceToMario < 200.f)) {
                o->oAction = 5;
            }
            spawn_object(o, MODEL_SMOKE, bhvBobombFuseSmoke);
            cur_obj_play_sound_1(SOUND_AIR_BOBOMB_LIT_FUSE);
            break;
        case 5:
            if (o->oTimer < 5){
                cur_obj_scale(1.0 + (f32) o->oTimer / 5.0);
            }else {
                explosion = spawn_object(o, MODEL_EXPLOSION, bhvExplosion2);
                explosion->oGraphYOffset += 100.0f;
                explosion->oDamageOrCoinValue = 4;
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            break;
        case 6:
            if (o->oTimer < 5){
                cur_obj_scale(1.0 + (f32) o->oTimer / 5.0);
            }else {
                explosion = spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
                explosion->oGraphYOffset += 100.0f;
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            break;
    }
    if (o->oAction < 4) {
        if (hasVision()) {
            o->oAction = 4;
        }
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

void pengucage(void) {
    if (cur_obj_is_mario_ground_pounding_platform()) {
        spawn_mist_particles_variable(0, 0, 46.f);
        spawn_triangle_break_particles(30, 138, 3.0f, 4);
        mark_obj_for_deletion(o);
        cur_obj_nearest_object_with_behavior(bhvSneakyPengu)->oAction = 2;
    }
    load_object_collision_model();
}

#define /*0x100*/ oSleepBubble OBJECT_FIELD_OBJ(0x20)
void wario(void) {
    switch (o->oBehParams2ndByte) {
        case 0:
            cur_obj_init_animation(0);
            o->oAnimState = 1;
            if (!o->oSleepBubble) {
                o->oSleepBubble = spawn_object(o, MODEL_BUBBLE, bhvYoshiBubble);
                o->oSleepBubble->parentObj = o;
                o->oSleepBubble->oBehParams2ndByte = 1;
            }
            break;
        case 1:

            break;
        case 2:

            break;
    }
}

void jailukiki(void) {
    cur_obj_update_floor_and_walls();
    if (!o->oAction) {
        if (gMarioState->pos[2] > (o->oPosZ - 200.f)) {
            if (talkToMario(79, 2)) {
                o->oAction = 1;
            }
        }
    } else {
        cur_obj_init_animation(4);
    }
}

// bhvBubbleSplash
// bhvWaterDroplet
void waterdrop(void) {
    if (o->oTimer < 2) {
        o->oOpacity = random_u16() % 50 + 50 + o->oBehParams2ndByte;
    }
    if (o->oTimer > o->oOpacity) {
        o->oTimer = 0;
        spawn_object(o, MODEL_BUBBLE, bhvBubbleDrop);
    }
}
s8 startWave = 0;
void bubbledrop(void) {
    int i;
    o->oPosY += o->oVelY;
    switch (o->oAction) {
        case 0:
            if (!o->oTimer) {
                cur_obj_update_floor_height();
                o->oHomeY = find_water_level(o->oPosX, o->oPosZ);
            }
            cur_obj_scale(o->oTimer * 0.05f);
            o->oGraphYOffset = -32.f * o->header.gfx.scale[0] - 8.f;
            if (o->oTimer > 20) {
                o->oAction++;
            }
            break;
        case 1:
            o->oVelY -= 4.f;
            if (o->oVelY < -75.f) {
                o->oVelY = -75.f;
            }
            if ((o->oPosY < o->oFloorHeight)||(o->oPosY < o->oHomeY)) {
                startWave = 1;
                o->oPosY = o->oFloorHeight;
                if (o->oPosY < o->oHomeY){
                o->oPosY = o->oHomeY;
                }
                cur_obj_play_sound_1(SOUND_OBJ_WALKING_WATER);
                for (i = 0; i < 5; i++) {
                    spawn_object(o, MODEL_BUBBLE, bhvBubbleExplosion);
                }
                o->oAction++;
            }
            break;
        case 2:
            mark_obj_for_deletion(o);
            break;
    }
}

void bhv_bubbleparticleinit(void) {
    obj_scale_xyz(o, 1.0f, 1.0f, 0.5f);
    o->oWaterObjUnkFC = 0x800 + (s32) (random_float() * 2048.0f);
    o->oWaterObjUnk100 = 0x800 + (s32) (random_float() * 2048.0f);
    obj_translate_xyz_random(o, 20.0f);
    o->oMoveAngleYaw = cur_obj_angle_to_home();
    o->oForwardVel = (random_f32_around_zero(1.f) + 1.f) * 10.f;
    o->oVelY = (random_f32_around_zero(1.f) + 2.f) * 4.f;
}

void bhv_bubbleparticleloop(void) {
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        mark_obj_for_deletion(o);
    }
}

extern const struct Animation dorrie_anim_sm64_anim;
extern const struct Animation dorrie_anim_eat;

extern ALIGNED8 const u8 dorriehappy[];
extern ALIGNED8 const u8 dorrie_angrydorrieeye__1__ia8[];
Gfx eyesarray[] = {
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 16, dorrie_angrydorrieeye__1__ia8),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 16, dorriehappy),
};
// if you GP her, she goes evildorrie again

void moveToBOBPOS() {
    s16 targetAngle;
    f32 dist;
    dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                 + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX));
    if (dist < 100.f) {
        o->oForwardVel *= 0.98f;
    }
    dist = dist / 700.f;
    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0100);
    o->oAngleVelYaw = o->oMoveAngleYaw - o->oFaceAngleYaw;
    o->oForwardVel = approach_f32(
        o->oForwardVel, 4.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f) * dist, 0.2f, 1.f);
}
u8 dorrieNotfriended = 1;
extern Gfx mat_dorrie_dorrieyes[];
void dorriecode(void) {
    struct Object *pipe;
    struct Object *fish;
    struct Surface *floor;
    f32 dist;
    s16 targetAngle;
    Gfx *eyes = segmented_to_virtual(mat_dorrie_dorrieyes);
    if (!dorrieNotfriended) {
        // set normal eye
        eyes[4] = eyesarray[1];
    } else {
        eyes[4] = eyesarray[0];
    }
    o->oDrawingDistance = 10000.f;
    if (cur_obj_is_mario_ground_pounding_platform()) {
        dorrieNotfriended = 1;
        cur_obj_play_sound_1(SOUND_OBJ_DORRIE);
        o->oAction = 4;
    }
    if (fish = cur_obj_nearest_object_with_behavior(bhvBetaHoldableObject)) {
        if (lateral_dist_between_objects(fish, o) < 2000.f) {
            find_floor(fish->oPosX, fish->oPosY, fish->oPosZ, &floor);
            if ((floor) && (SURFACETYPE(floor) == SURFACE_PAINTING_WARP_F9)) {
                if ((o->oAction != 3) && (o->oAction != 4)) {
                    o->oAction = 2;
                }
            }
        }
    }
    switch (o->oAction) {
        case 0:
            if (!dorrieNotfriended) {
                o->oAction = 5;
                return;
            }
            // idle, swim around, do some animations
            cur_obj_init_animation(1);
            if (o->oTimer < 2) {
                o->oTimer = 2;
                o->oOpacity = random_u16() % 150 + 150;
                o->oBobombBuddyPosXCopy = o->oHomeX + random_f32_around_zero(350.f);
                o->oBobombBuddyPosZCopy = o->oHomeZ + random_f32_around_zero(350.f);
                if (!(random_u16() % 9)) {
                    o->oAction = 1;
                    o->oOpacity = 0;
                }
            }
            if (o->oOpacity) {
                o->oOpacity--;
            } else {
                o->oTimer = 0;
            }
            dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                         + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX));
            dist = (dist) / 700.f;
            targetAngle =
                atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0040);
            o->oForwardVel = approach_f32(o->oForwardVel,
                                          2.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                              * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f) * dist,
                                          0.2f, 1.f);
            break;
        case 1:
            // play in waterfall
            o->oBobombBuddyPosXCopy = -5172.f;
            o->oBobombBuddyPosZCopy = 8674.f;
            moveToBOBPOS();

            dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                         + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX));
            if (dist < 100.f) {
                o->oOpacity++;
                o->oForwardVel *= 0.98f;
                cur_obj_init_animation(2);
                if (o->oOpacity > 0x50 * 3) {
                    o->oAction = 0;
                }
            }
            break;
        case 2:
            // go for snack
            cur_obj_init_animation(0);
            o->oBobombBuddyPosXCopy = fish->oPosX - sins(obj_angle_to_object(o, fish)) * 850.f;
            o->oBobombBuddyPosZCopy = fish->oPosZ - coss(obj_angle_to_object(o, fish)) * 850.f;
            moveToBOBPOS();
            if ((lateral_dist_between_objects(o, fish) < 900.f)
                && (lateral_dist_between_objects(o, fish) > 800.f)) {
                o->oAction = 3;
                o->oBobombBuddyPosXCopy = fish->oPosX;
                o->oBobombBuddyPosZCopy = fish->oPosZ;
            }
            break;
        case 3:
            // eat snack, let mario get on
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 2.f);
            o->oMoveAngleYaw = approach_s16_symmetric(
                o->oMoveAngleYaw,
                atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX), 0x100);
            if (!o->oTimer) {
                o->header.gfx.unk38.curAnim = segmented_to_virtual(&dorrie_anim_eat);
                o->header.gfx.unk38.animFrame = 0;
                o->header.gfx.unk38.animAccel = 0;
                o->header.gfx.unk38.animYTrans = 0;
            }
            if (o->header.gfx.unk38.animFrame > 20) {
                dorrieNotfriended = 0;
                if (fish) {
                    obj_mark_for_deletion(fish);
                }
            }
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction = 0;
            }
            break;
        case 4:
            // shake mario off
            o->oForwardVel = 0.f;
            if (!o->oTimer) {
                o->header.gfx.unk38.curAnim = segmented_to_virtual(&dorrie_anim_sm64_anim);
                o->header.gfx.unk38.animFrame = 0;
                o->header.gfx.unk38.animAccel = 0;
                o->header.gfx.unk38.animYTrans = 0;
            }
            o->header.gfx.unk38.animID = 3;
            if (o->oTimer == 54) {
                o->oAction = 0;
            }
            if (o->oTimer == 10) {
                m->vel[1] = 35.f;
                m->forwardVel = -60.f;
                m->action = ACT_THROWN_BACKWARD;
                play_sound(SOUND_MARIO_WAAAOOOW, gDefaultSoundArgs);
                cur_obj_play_sound_2(SOUND_OBJ_HEAVEHO_TOSSED);
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_UNK2;
                m->faceAngle[1] = o->oMoveAngleYaw;
                return;
            }
            break;
        case 5:
            // wait by land, after feeding her
            cur_obj_init_animation(1);
            if (cur_obj_is_mario_on_platform()
                || ((pipe = cur_obj_nearest_object_with_behavior(bhvWarpPipe))
                    && (dist_between_objects(o, gMarioState->marioObj) < 1000.f))) {
                o->oAction = 6;
                cur_obj_move_xz_using_fvel_and_yaw();
                load_object_collision_model();
                return;
            }
            o->oBobombBuddyPosXCopy = -3650.f;
            o->oBobombBuddyPosZCopy = 7298.f;
            moveToBOBPOS();
            break;
        case 6:
            // swim over to pipe after feeding her
            cur_obj_init_animation(1);
            if (!cur_obj_is_mario_on_platform()) {
                o->oAction = 5;
                cur_obj_move_xz_using_fvel_and_yaw();
                load_object_collision_model();
                return;
            }
            o->oBobombBuddyPosXCopy = -6022.f;
            o->oBobombBuddyPosZCopy = 7418.f;
            moveToBOBPOS();
            break;
    }
    if (dorrieNotfriended) {
        if (cur_obj_is_mario_on_platform()) {
            o->oAction = 4;
        }
    }
    cur_obj_move_xz_using_fvel_and_yaw();
    load_object_collision_model();
}

void sellguy(void) {
    struct Object *fish;
    switch (o->oAction) {
        case 0:
            // wait for mario close
            if (o->oDistanceToMario > 750.f) {
                o->oSubAction = 1;
            }
            if (o->oSubAction && (o->oDistanceToMario < 550.f)) {
                o->oAction = 1;
            }
            break;
        case 1:
            // ask sale
            switch (askMario(69, 0, 2)) {
                case 0:
                    break;
                case 1:
                    // yes
                    if (gMarioState->numCoins > 9) {
                        o->oAction = 3;
                        gMarioState->numCoins -= 10;
                        gHudDisplay.coins = gMarioState->numCoins;
                        play_sound(SOUND_GENERAL_COIN, gDefaultSoundArgs);
                    } else {
                        o->oAction = 4;
                    }
                    break;
                case 2:
                    // no
                    o->oAction = 2;
                    break;
                case 3:
                    break;
            }
            break;
        case 2:
            // sad you wont buy
            if (talkToMarioNoRotation(72, 2)) {
                o->oAction = 0;
            }
            break;
        case 3:
            // praises for buying
            if (talkToMarioNoRotation(70, 2)) {
                o->oAction = 0;
                fish = spawn_object(o, MODEL_FISH, bhvBetaHoldableObject);
                fish->oVelY = 50.f;
                fish->oForwardVel = 20.f;
            }
            break;
        case 4:
            // sad you have no coins
            if (talkToMarioNoRotation(71, 2)) {
                o->oAction = 0;
            }
            break;
    }
}

u32 checkMonkeFloor() {
    struct Surface *floor;
#define monkedist 100.f
    f32 floorHeight = find_floor(o->oPosX + sins(o->oAngleToMario) * monkedist, o->oPosY + 200.f,
                                 o->oPosZ + coss(o->oAngleToMario) * monkedist, &floor);
    if (gMarioState->pos[1] != gMarioState->floorHeight) {
        return 1;
    }
    if (absf(floorHeight - o->oPosY) > 100.f) {
        return 0;
    } else {
        return 1;
    }
}
f32 distSpeed(f32 distance, f32 max) {
    if (distance > max) {
        return 1;
    } else {
        return (distance / max) * (distance / max);
    }
}

static struct SpawnParticlesInfo monkeydist = {
    /* behParam:        */ 0,
    /* count:           */ 3,
    /* model:           */ MODEL_MIST,
    /* offsetY:         */ 0,
    /* forwardVelBase:  */ 15,
    /* forwardVelRange: */ 10,
    /* velYBase:        */ 0,
    /* velYRange:       */ 15,
    /* gravity:         */ -1,
    /* dragStrength:    */ 0,
    /* sizeBase:        */ 50.0f,
    /* sizeRange:       */ 16.0f,
};

f32 calcLandTime(f32 curY, f32 goalY, f32 gravity, f32 startspeed) {
    // gravity * return^2 - 2*startspeed*return + 2 * (goalY - curY)
    f32 p, q;
    p = startspeed / gravity;
    q = -2 * (goalY - curY) / gravity;
    return -p + sqrtf(p * p - q);
}

void demolishmonke(void) {
    struct Object *banana;
    struct Object *car;
    if (banana = cur_obj_nearest_object_with_behavior(bhvBananaZoo)) {
        o->oAngleToMario = obj_angle_to_object(o, banana);
    } else {
        if (o->oAction > 2) {
            o->oAction = 0;
        }
    }
    if (car = cur_obj_nearest_object_with_behavior(bhvWarioCar)) {
        if (!car->oAction) {
            if (o->oAction != 7) {
                if (dist_between_objects(car, o) < 400.f) {
                    o->oAction = 8;
                }
            }
        }
    }
    switch (o->oAction) {
        case 0:
            // idle stand
            cur_obj_init_animation(9);
            if (o->header.gfx.unk38.animFrame == 0x63) {
                switch (random_u16() % 5) {
                    case 0:
                        o->oAction = 1;
                        cur_obj_play_sound_1(SOUND_OBJ_UKIKI_CHATTER_SHORT);
                        break;
                    case 1:
                        o->oAction = 2;
                        cur_obj_play_sound_1(SOUND_OBJ_UKIKI_CHATTER_IDLE);
                        break;
                }
            }
            if (cur_obj_dist_to_nearest_object_with_behavior(bhvBananaZoo) < 1000.f) {
                o->oAction = 3;
                cur_obj_play_sound_1(SOUND_OBJ_UKIKI_CHATTER_LONG);
            }
            break;
        case 1:
            // idle handstand
            cur_obj_init_animation(10);
            if (cur_obj_check_if_near_animation_end()) {
                o->oAction = 0;
            }
            break;
        case 2:
            // idle jump up down
            cur_obj_init_animation(4);
            if (cur_obj_check_if_near_animation_end()) {
                o->oAction = 0;
            }
            break;
        case 3:
            // excited for banana
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x500);
            cur_obj_init_animation(5);
            if (cur_obj_check_if_near_animation_end()) {
                if (checkMonkeFloor()) {
                    o->oAction = 5;
                } else {
                    o->oAction = 4;
                }
            }
            break;
        case 4:
            // jump towards banana
            if (!o->oTimer) {
                o->oVelY = 70.f;
                o->oMoveAngleYaw = o->oAngleToMario;
                if (calcLandTime(o->oPosY, banana->oPosY, -4.f, o->oVelY) > 1.f) {
                    o->oForwardVel = lateral_dist_between_objects(o, banana)
                                     / calcLandTime(o->oPosY, banana->oPosY, -4.f, o->oVelY);
                } else {
                }
            }
            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                o->oAction = 5;
            }
            cur_obj_init_animation(8);
            break;
        case 5:
            // walk towards banana
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x500);
            o->oForwardVel =
                approach_f32(o->oForwardVel,
                             25.f * (coss(o->oAngleToMario - o->oMoveAngleYaw) + 1.0f)
                                 * distSpeed(lateral_dist_between_objects(o, banana), 500.f),
                             2.f, 4.f);
            if (o->oForwardVel < 8.f) {
                cur_obj_init_animation(11);
            } else if (o->oForwardVel < 16.f) {
                cur_obj_init_animation(1);
            } else {
                cur_obj_init_animation(0);
            }
            if (!checkMonkeFloor()) {
                o->oAction = 4;
            }
            if (gMarioState->heldObj != banana) {
                if (dist_between_objects(o, banana) < 150.f) {
                    o->oAction = 6;
                }
            }
            if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
                o->oAction = 4;
            }
            break;
        case 6:
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x800);
            cur_obj_init_animation(7);
            o->oForwardVel = approach_f32(o->oForwardVel, 0.f, 2.f, 4.f);
            if (o->oTimer > 0x20) {
                mark_obj_for_deletion(banana);
                o->oAction = 0;
            }
            break;
        case 7:
            o->oMoveAngleYaw =
                approach_s16_symmetric(o->oMoveAngleYaw, obj_angle_to_object(o, car), 0x400);
            cur_obj_init_animation(7);
            o->oForwardVel = approach_f32(o->oForwardVel, 0.f, 2.f, 4.f);
            if (!(o->oTimer & 0x7)) {
                cur_obj_play_sound_1(SOUND_OBJ_UKIKI_CHATTER_SHORT);
            }
            cur_obj_spawn_particles(&monkeydist);
            if (o->oTimer > 0x5f) {
                car->oAction = 1;
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION7);
            }
            if (o->oTimer > 0x60) {
                o->oAction = 0;
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 250.f,
                                   gMarioState->pos[2]);
            }
            break;
        case 8:
            // jump towards banana
            if (!o->oTimer) {
                cur_obj_play_sound_1(SOUND_OBJ_UKIKI_CHATTER_SHORT);
                o->oVelY = 60.f;
                o->oForwardVel = lateral_dist_between_objects(o, car) / 32.f;
                o->oMoveAngleYaw = obj_angle_to_object(o, car);
            }
            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                o->oAction = 7;
            }
            cur_obj_init_animation(8);
            break;
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

void chompgate(void) {
    o->oDrawingDistance = 30000.f;
    // explode if shot into
    if (gMarioState->prevAction == ACT_SHOT_FROM_CANNON) {
        if (gMarioState->wall) {
            if (gMarioState->wall->object == o) {
                if (gMarioState->action == ACT_BACKWARD_AIR_KB) {
                    play_puzzle_jingle();
                    create_sound_spawner(SOUND_GENERAL_WALL_EXPLOSION);
                    o->oInteractType = 8;
                    o->oDamageOrCoinValue = 1;
                    obj_explode_and_spawn_coins(80.0f, 0);
                    mark_obj_for_deletion(o);
                }
            }
        }
    }
}

u32 checkWarioFloor() {
    struct Surface *floor;
#define wariodist 175.f
    f32 floorHeight = find_floor(o->oPosX + sins(o->oMoveAngleYaw) * wariodist, o->oPosY + 1000.f,
                                 o->oPosZ + coss(o->oMoveAngleYaw) * wariodist, &floor);
    if ((floorHeight - o->oPosY) > 100.f) {
        return 1;
    } else {
        return 0;
    }
}

void warioTWO(void) {
    struct Surface *floor;
    struct Object *chomp;
    chomp = cur_obj_nearest_object_with_behavior(bhvChainChomp);

    obj_update_blinking(&o->oOpacity, 40, 40, 6);
    switch (o->oAction) {
        case 0:
            cur_obj_init_animation(1);
            if (talkToMario(44, 2)) {
                o->oAction = 1;
                o->oBehParams2ndByte = 0;
            }
            break;
        case 1:
            o->oBehParams2ndByte++;
            cur_obj_init_animation(2);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, 0x5E00, 0x0200);
            o->oForwardVel =
                approach_f32(o->oForwardVel, 6.5f * (coss(0x5E00 - o->oMoveAngleYaw) + 1.0f), 1.f, 1.f);
            if (o->header.gfx.unk38.animFrame > 63) {
                o->header.gfx.unk38.animFrame = 14;
            }
            if (checkWarioFloor()) {
                o->oAction = 2;
                o->oVelY = 52.f;
            }
            if (o->oBehParams2ndByte > 425) {
                o->oAction = 3;
                makeTalkable();
            }
            break;
        case 2:
            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                o->oAction = 1;
            }
            break;
        case 3:
            cur_obj_init_animation(1);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x100);
            o->oForwardVel = approach_f32(o->oForwardVel, 0.f, 1.f, 1.f);

            switch (o->oSubAction) {
                case 0:
                    if (mario_ready_to_speak() && (o->oInteractStatus == INT_STATUS_INTERACTED)) {
                        o->oSubAction = 1;
                    }
                    break;
                case 1:
                    if (talkToMario(76, 4)) {
                        o->oSubAction = 0;
                    }
                    break;
            }
            if (chomp) {
                find_floor(chomp->oPosX, chomp->oPosY, chomp->oPosZ, &floor);
                if ((floor) && (SURFACETYPE(floor) != SURFACE_PAINTING_WARP_FB)) {
                    if (chomp->oPosY > 10.f) {
                        if (lateral_dist_between_objects(chomp, o) < 2000.f) {
                            o->oAction = 4;
                        }
                    }
                }
            }
            break;
        case 4:
            if (!o->oSubAction) {
                if (talkToMario(45, 2)) {
                    o->oSubAction = 1;
                }
            } else {
                if (set_mario_npc_dialog(2) == 2 && cutscene_object(CUTSCENE_STAR_SPAWN, o) == 1) {
                    o->oAction = 5;
                    // set chain chomop to be fucking off
                    play_sound(SOUND_GENERAL_CHAIN_CHOMP2, gDefaultSoundArgs);
                    chomp->oMoveAngleYaw = obj_angle_to_object(chomp, o);
                    chomp->oSubAction = 9;
                    chomp->oIntangibleTimer = -1;
                    chomp->oChainChompMaxDistFromPivotPerChainPart = 900.0f / 5;

                    chomp->oForwardVel = 70.f;
                    chomp->oVelY = 50.f;
                    chomp->oGravity = -3.0f;
                    chomp->oChainChompTargetPitch = 0x1000;
                }
            }
            break;
        case 5:
            o->oBehParams2ndByte++;
            cur_obj_init_animation(2);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, 0xE200, 0x0400);
            o->oForwardVel =
                approach_f32(o->oForwardVel, 18.f * (coss(0xE200 - o->oMoveAngleYaw) + 1.0f), 2.f, 2.f);
            if ((o->header.gfx.unk38.animFrame += 2) > 63) {
                o->header.gfx.unk38.animFrame = 14;
            }
            if (o->oBehParams2ndByte == 500) {
                play_sound(SOUND_GENERAL_CHAIN_CHOMP2, gDefaultSoundArgs);
                chomp->oMoveAngleYaw = obj_angle_to_object(chomp, o);
                chomp->oSubAction = 9;
                chomp->oChainChompMaxDistFromPivotPerChainPart = 900.0f / 5;
                chomp->oForwardVel = 70.f;
                chomp->oVelY = 50.f;
                chomp->oGravity = -3.0f;
                chomp->oChainChompTargetPitch = 0x1000;
                chomp->oIntangibleTimer = 0;
            }
            if (o->oBehParams2ndByte > 557) {
                gObjCutsceneDone = TRUE;
                if (cutscene_object(CUTSCENE_STAR_SPAWN, o) == -1) {
                    set_mario_npc_dialog(0);
                    spawn_mist_particles_variable(0, 0, 46.f);
                    spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 250.f,
                                       gMarioState->pos[2]);
                    mark_obj_for_deletion(o);
                }
            }
            break;
    }
    o->oInteractStatus = 0;
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
}

void crabKritter(void) {
    f32 dist;
    u16 targetAngle;
    struct Surface *floor;
    if (o->oTimer < 2) {
        o->oTimer = 2;
        o->oOpacity = random_u16() % 150 + 150;
        o->oBobombBuddyPosXCopy = o->oHomeX + random_f32_around_zero(1000.f);
        o->oBobombBuddyPosZCopy = o->oHomeZ + random_f32_around_zero(1000.f);
    }
    if (o->oOpacity) {
        o->oOpacity--;
    } else {
        o->oTimer = 0;
    }
    dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                 + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX));
    dist = (dist) / 700.f;
    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x240);
    o->oForwardVel = approach_f32(o->oForwardVel,
                                  4.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                      * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f) * dist,
                                  1.1f, 2.f);
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oWallHitboxRadius = 30.0f;
    o->oBooBaseScale += o->oForwardVel;
    if (o->oBooBaseScale > 10.f) {
        o->oAnimState++;
        o->oBooBaseScale -= 10.f;
    }
    resolve_and_return_wall_collisions(&o->oPosX, 0.f, 60.f);
    o->oPosY = find_floor(o->oPosX, o->oPosY + 100.f, o->oPosZ, &floor);
}
// roam in area
// if mario close, sink until on marios height, then move up and towards mario (fixed Yspeed)
// if mario far, resume roam
extern struct Surface *resolve_and_return_wall_collisions(Vec3f pos, f32 offset, f32 radius);
void bloopercode(void) {
    f32 waterH = 0;
    switch (o->oAction) {
        case 0:
            if (o->oTimer < 2) {
                o->oAction = 2;
                o->oVelY = 40.f;
                o->oBobombBuddyPosXCopy = o->oHomeX + random_f32_around_zero(800.f);
                o->oBobombBuddyPosZCopy = o->oHomeZ + random_f32_around_zero(800.f);
                o->oBobombBuddyPosYCopy = o->oHomeY + random_f32_around_zero(200.f);
                o->header.gfx.unk38.animFrame = 0;
            }
            break;
        case 1:
            if (o->oTimer > 30) {
                o->oAction = 2;
                o->oVelY = 40.f;
                o->oBobombBuddyPosXCopy = gMarioState->pos[0];
                o->oBobombBuddyPosZCopy = gMarioState->pos[2];
                o->oBobombBuddyPosYCopy = gMarioState->pos[1];
                o->header.gfx.unk38.animFrame = 0;
                cur_obj_play_sound_2(SOUND_GENERAL_SWISH_WATER);
            }
            break;
        case 2:
            o->oPosX = approach_f32_symmetric(o->oPosX, o->oBobombBuddyPosXCopy, 12.f);
            o->oPosZ = approach_f32_symmetric(o->oPosZ, o->oBobombBuddyPosZCopy, 12.f);
            if ((absf(o->oPosX - o->oBobombBuddyPosXCopy) + absf(o->oPosZ - o->oBobombBuddyPosZCopy))
                > 50.f) {
                o->oFaceAngleYaw = approach_s16_symmetric(
                    o->oFaceAngleYaw,
                    atan2s(o->oPosZ - o->oBobombBuddyPosZCopy, o->oPosX - o->oBobombBuddyPosXCopy),
                    0x400);
            }
            if ((o->oVelY < 5.f)
                && ((o->oPosY < o->oBobombBuddyPosYCopy)
                    || ((o->oPosY - 50.f) < find_floor_height(o->oPosX, o->oPosY, o->oPosZ)))) {
                if (lateral_dist_between_objects(o, gMarioState->marioObj) < 1250.f) {
                    o->oAction = 1;
                } else {
                    o->oAction = 0;
                }
            }
            break;
    }
    o->oPosY += o->oVelY;
    o->oPosY -= 4.f;
    o->oVelY = approach_f32_symmetric(o->oVelY, 0.f, 2.f);
    if ((waterH = find_water_level(o->oPosX, o->oPosZ) - 75.f) < o->oPosY) {
        o->oPosY = waterH;
    }
    o->oWallHitboxRadius = 60.0f;
    resolve_and_return_wall_collisions(&o->oPosX, 0.f, 60.f);
}

extern const Trajectory ccm_area_3_spline_TURTLEPOINTS[];
#define POINTCOUNT 8
// swim to random point on spline or swim after other turtle
// go real slow
void smoltoisecode(void) {
    f32 dist;
    u16 targetAngle;
    u16 targetPitch;
    u8 pointgoal;
    struct Surface *floor;
    cur_obj_scale(0.1f + o->oBehParams2ndByte / 100.f);
    o->hitboxRadius = 170.f * o->header.gfx.scale[0];
    o->hitboxHeight = 70.f * o->header.gfx.scale[0];
    if (o->oTimer < 2) {
        o->oTimer = 2;
        o->oOpacity = random_u16() % 300 + 300;
        pointgoal = random_u16() % ((int) (POINTCOUNT * 1.2f));
        if (pointgoal < POINTCOUNT) {
            o->oBobombBuddyPosXCopy =
                ((s16 *) segmented_to_virtual(ccm_area_3_spline_TURTLEPOINTS))[1 + pointgoal * 4];
            o->oBobombBuddyPosYCopy =
                ((s16 *) segmented_to_virtual(ccm_area_3_spline_TURTLEPOINTS))[2 + pointgoal * 4];
            o->oBobombBuddyPosZCopy =
                ((s16 *) segmented_to_virtual(ccm_area_3_spline_TURTLEPOINTS))[3 + pointgoal * 4];
            o->oHiddenBlueCoinSwitch = 0;
        } else {
            o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvSmoltoise);
        }
    }
    if (o->oHiddenBlueCoinSwitch) {
        o->oBobombBuddyPosXCopy = o->oHiddenBlueCoinSwitch->oPosX;
        o->oBobombBuddyPosYCopy = o->oHiddenBlueCoinSwitch->oPosY;
        o->oBobombBuddyPosZCopy = o->oHiddenBlueCoinSwitch->oPosZ;
    }
    if (o->oOpacity) {
        o->oOpacity--;
    } else {
        o->oTimer = 0;
    }
    dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                 + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)
                 + (o->oBobombBuddyPosYCopy - o->oPosY) * (o->oBobombBuddyPosYCopy - o->oPosY));
    dist = (dist) / 1500.f;
    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0040);
    o->oForwardVel = approach_f32(o->oForwardVel,
                                  1.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                      * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f) * dist,
                                  0.2f, 2.f);
    targetPitch =
        -atan2s(sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                      + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)),
                o->oBobombBuddyPosYCopy - o->oPosY);
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x0040);

    o->oVelY = sins(-o->oFaceAnglePitch) * o->oForwardVel;
    o->oVelX = coss(-o->oFaceAnglePitch) * sins(o->oMoveAngleYaw) * o->oForwardVel;
    o->oVelZ = coss(-o->oFaceAnglePitch) * coss(o->oMoveAngleYaw) * o->oForwardVel;
    cur_obj_move_using_vel();
    resolve_and_return_wall_collisions(&o->oPosX, 0.f, 80.f);
}

extern const Trajectory ccm_area_3_spline_SHARKPOINTS[];
#define POINTCOUNT2 15
void zooshark(void) {
    f32 dist;
    u16 targetAngle;
    u16 targetPitch;
    u8 pointgoal;

    o->oBobombBuddyPosXCopy =
        ((s16 *) segmented_to_virtual(ccm_area_3_spline_SHARKPOINTS))[1 + o->oOpacity * 4];
    o->oBobombBuddyPosYCopy =
        ((s16 *) segmented_to_virtual(ccm_area_3_spline_SHARKPOINTS))[2 + o->oOpacity * 4];
    o->oBobombBuddyPosZCopy =
        ((s16 *) segmented_to_virtual(ccm_area_3_spline_SHARKPOINTS))[3 + o->oOpacity * 4];
    dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                 + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)
                 + (o->oBobombBuddyPosYCopy - o->oPosY) * (o->oBobombBuddyPosYCopy - o->oPosY));
    if (dist < 300.f) {
        o->oOpacity++;
        o->oOpacity = o->oOpacity % POINTCOUNT2;
    }
    dist = (dist) / 1500.f;
    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0040);
    targetPitch =
        -atan2s(sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                      + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)),
                o->oBobombBuddyPosYCopy - o->oPosY);
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x0040);

    o->oForwardVel = approach_f32(o->oForwardVel,
                                  2.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                      * (coss(targetPitch - o->oFaceAnglePitch) + 1.0f) * dist,
                                  0.2f, 2.f);
    o->oVelY = sins(-o->oFaceAnglePitch) * o->oForwardVel;
    o->oVelX = coss(-o->oFaceAnglePitch) * sins(o->oMoveAngleYaw) * o->oForwardVel;
    o->oVelZ = coss(-o->oFaceAnglePitch) * coss(o->oMoveAngleYaw) * o->oForwardVel;
    cur_obj_move_using_vel();
    resolve_and_return_wall_collisions(&o->oPosX, 0.f, 80.f);
}

extern const Trajectory ccm_area_3_spline_MANTAPOINTS[];
#define POINTCOUNT3 7
void mantacode(void) {
    f32 dist;
    u16 targetAngle;
    u16 targetPitch;
    u8 pointgoal;
    struct Surface *floor;
    f32 floorheight;
    cur_obj_scale(0.2f + o->oBehParams2ndByte / 80.f);
    o->hitboxRadius = 210.f * o->header.gfx.scale[0];
    o->hitboxHeight = 60.f * o->header.gfx.scale[0];
    if (o->oTimer < 2) {
        o->oTimer = 2;
        o->oOpacity = random_u16() % 300 + 250;
        pointgoal = random_u16() % POINTCOUNT3;
        o->oBobombBuddyPosXCopy =
            ((s16 *) segmented_to_virtual(ccm_area_3_spline_MANTAPOINTS))[1 + pointgoal * 4];
        o->oBobombBuddyPosYCopy =
            find_floor(o->oBobombBuddyPosXCopy, o->oPosY + 1000.f, o->oBobombBuddyPosZCopy, &floor)
            + 125.f + random_f32_around_zero(75.f);
        o->oBobombBuddyPosZCopy =
            ((s16 *) segmented_to_virtual(ccm_area_3_spline_MANTAPOINTS))[3 + pointgoal * 4];
    }
    if (o->oOpacity) {
        o->oOpacity--;
    } else {
        o->oTimer = 0;
    }
    dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                 + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)
                 + (o->oBobombBuddyPosYCopy - o->oPosY) * (o->oBobombBuddyPosYCopy - o->oPosY));
    dist = (dist) / 1500.f;
    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x0080);
    targetPitch =
        -atan2s(sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                      + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)),
                o->oBobombBuddyPosYCopy - o->oPosY);
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x0080);

    o->oForwardVel = approach_f32(o->oForwardVel,
                                  1.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                      * (coss(targetPitch - o->oFaceAnglePitch) + 1.0f) * dist,
                                  0.2f, 2.f);
    o->oVelY = sins(-o->oFaceAnglePitch) * o->oForwardVel;
    o->oVelX = coss(-o->oFaceAnglePitch) * sins(o->oMoveAngleYaw) * o->oForwardVel;
    o->oVelZ = coss(-o->oFaceAnglePitch) * coss(o->oMoveAngleYaw) * o->oForwardVel;
    cur_obj_move_using_vel();

    floorheight = find_floor(o->oPosX, o->oPosY + 100.f, o->oPosZ, &floor);
    if (o->oPosY < floorheight) {
        o->oPosY = floorheight;
    }
    resolve_and_return_wall_collisions(&o->oPosX, 0.f, 80.f);
}

extern const Trajectory ccm_area_3_spline_CHEEPPOINTS[];
// move to random point
// chase other cheepcheep (set other cheep to chased when close enough, stop after a bit)
// chased by other cheep, finds most distant point from other cheep
// move towards mario, if mario close

#define POINTCOUNT4 11
u32 find_furthest_point() {
    u32 i;
    f32 x, y, z;
    f32 mindist = 0.f;
    f32 curdist = 0;
    u32 curpoint = 0;
    for (i = 0; i < POINTCOUNT4; i++) {
        x = ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[1 + i * 4]
            - o->oHiddenBlueCoinSwitch->oPosX;
        y = ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[2 + i * 4]
            - o->oHiddenBlueCoinSwitch->oPosY;
        z = ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[3 + i * 4]
            - o->oHiddenBlueCoinSwitch->oPosZ;
        if ((curdist = sqrtf(x * x + y * y + z * z)) > mindist) {
            mindist = curdist;
            curpoint = i;
        }
    }
    return curpoint;
}

void cheepcheepcode(void) {
    f32 dist;
    u16 targetAngle;
    u16 targetPitch;
    u8 pointgoal;
    struct Surface *floor;
    f32 floorheight;
    f32 speedScale = 2.f;
    cur_obj_scale(0.5f + o->oBehParams2ndByte / 25.f);
    o->hitboxRadius = 60.f * o->header.gfx.scale[0];
    o->hitboxHeight = 60.f * o->header.gfx.scale[0];
    if (o->oOpacity < 2) {
        o->oOpacity = random_u16() % 300 + 250;
        pointgoal = random_u16() % ((int) (POINTCOUNT4 + 2));
        if (pointgoal < POINTCOUNT4) {
            o->oBobombBuddyPosXCopy =
                ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[1 + pointgoal * 4];
            o->oBobombBuddyPosYCopy =
                ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[2 + pointgoal * 4];
            o->oBobombBuddyPosZCopy =
                ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[3 + pointgoal * 4];
            o->oAction = 0;
        } else {
            o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvBub);
            if ((o->oHiddenBlueCoinSwitch->oAction == 0) || (o->oHiddenBlueCoinSwitch->oAction == 1)) {
                o->oAction = 2;
            } else {
                o->oAction = 0;
            }
        }
        if (o->oDistanceToMario < 500.f) {
            o->oAction = 1;
        }
    }
    if (o->oAction == 2) {
        speedScale = 16.f;
        o->oBobombBuddyPosXCopy = o->oHiddenBlueCoinSwitch->oPosX;
        o->oBobombBuddyPosYCopy = o->oHiddenBlueCoinSwitch->oPosY;
        o->oBobombBuddyPosZCopy = o->oHiddenBlueCoinSwitch->oPosZ;
        if (o->oOpacity) {
            o->oOpacity--;
            o->oHiddenBlueCoinSwitch->oOpacity = 12;
        }
    } else if (o->oAction == 1) {
        o->oBobombBuddyPosXCopy = gMarioState->pos[0];
        o->oBobombBuddyPosYCopy = gMarioState->pos[1];
        o->oBobombBuddyPosZCopy = gMarioState->pos[2];
    } else if (o->oAction == 3) {
        speedScale = 16.f;
        pointgoal = find_furthest_point();
        o->oBobombBuddyPosXCopy =
            ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[1 + pointgoal * 4];
        o->oBobombBuddyPosYCopy =
            ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[2 + pointgoal * 4];
        o->oBobombBuddyPosZCopy =
            ((s16 *) segmented_to_virtual(ccm_area_3_spline_CHEEPPOINTS))[3 + pointgoal * 4];
    }

    if (o->oOpacity) {
        o->oOpacity--;
    }
    dist = sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                 + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)
                 + (o->oBobombBuddyPosYCopy - o->oPosY) * (o->oBobombBuddyPosYCopy - o->oPosY));
    if ((dist < 700.f) && (o->oAction == 2)) {
        o->oHiddenBlueCoinSwitch->oAction = 3;
        o->oHiddenBlueCoinSwitch->oOpacity = 12;
        o->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch = o;
        speedScale = 12.f * dist / 700.f;
    }
    if ((o->oAction != 2) && (o->oAction != 3)) {
        dist = (dist) / 1500.f;
    } else {
        dist = 1.f;
    }

    targetAngle = atan2s(o->oBobombBuddyPosZCopy - o->oPosZ, o->oBobombBuddyPosXCopy - o->oPosX);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x00C0 * speedScale);
    targetPitch =
        -atan2s(sqrtf((o->oBobombBuddyPosZCopy - o->oPosZ) * (o->oBobombBuddyPosZCopy - o->oPosZ)
                      + (o->oBobombBuddyPosXCopy - o->oPosX) * (o->oBobombBuddyPosXCopy - o->oPosX)),
                o->oBobombBuddyPosYCopy - o->oPosY);
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, 0x00C0 * speedScale);

    o->oForwardVel = approach_f32(o->oForwardVel,
                                  speedScale * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f)
                                      * (coss(targetPitch - o->oFaceAnglePitch) + 1.0f) * dist,
                                  0.2f * speedScale, 2.f);
    if (o->oForwardVel > 15.f) {
        o->header.gfx.unk38.animFrame++;
    }
    if (o->oForwardVel > 30.f) {
        o->header.gfx.unk38.animFrame++;
    }
    o->oVelY = sins(-o->oFaceAnglePitch) * o->oForwardVel;
    o->oVelX = coss(-o->oFaceAnglePitch) * sins(o->oMoveAngleYaw) * o->oForwardVel;
    o->oVelZ = coss(-o->oFaceAnglePitch) * coss(o->oMoveAngleYaw) * o->oForwardVel;
    cur_obj_move_using_vel();

    floorheight = find_floor(o->oPosX, o->oPosY + 100.f, o->oPosZ, &floor);
    if (o->oPosY < floorheight) {
        o->oPosY = floorheight;
    }
    resolve_and_return_wall_collisions(&o->oPosX, 0.f, 60.f + o->oForwardVel);
}

void spawn_pebbles(s8 offsetY, s8 velYBase) {
    static struct SpawnParticlesInfo sMontyMoleRiseFromGroundParticles = {
        /* behParam:        */ 0,
        /* count:           */ 3,
        /* model:           */ MODEL_PEBBLE,
        /* offsetY:         */ 0,
        /* forwardVelBase:  */ 10,
        /* forwardVelRange: */ 5,
        /* velYBase:        */ 10,
        /* velYRange:       */ 15,
        /* gravity:         */ -4,
        /* dragStrength:    */ 0,
        /* sizeBase:        */ 10.0f,
        /* sizeRange:       */ 7.0f,
    };

    sMontyMoleRiseFromGroundParticles.offsetY = offsetY;
    sMontyMoleRiseFromGroundParticles.velYBase = velYBase;
    cur_obj_spawn_particles(&sMontyMoleRiseFromGroundParticles);
}

void skullblockcode(void) {
    switch (o->oAction) {
        case 0:
            o->oOpacity++;
            o->oAnimState = (o->oTimer & 8) >> 3;
            if (cur_obj_is_mario_on_platform() && (gMarioState->action != ACT_LEDGE_GRAB)
                && (gMarioState->action != ACT_LEDGE_CLIMB_FAST
                    && (gMarioState->action != ACT_LEDGE_CLIMB_SLOW_1)
                    && (gMarioState->action != ACT_LEDGE_CLIMB_SLOW_2))) {
                if (o->oOpacity > 8) {
                    o->oAction = 1;
                }
            } else {
                o->oOpacity = 0;
            }
            load_object_collision_model();
            break;
        case 1:
            cur_obj_scale(o->header.gfx.scale[0] - 0.01f);
            o->oAnimState = (o->oTimer & 2) >> 1;
            spawn_pebbles(65 * o->header.gfx.scale[0], 30);
            load_object_collision_model();
            if (o->oTimer > 35) {
                spawn_mist_particles_variable(0, 0, 46.0f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                o->oAction = 2;
                cur_obj_set_model(0);
            }
            break;
        case 2:
            if (o->oTimer > 200) {
                o->oAction = 0;
                cur_obj_set_model(0x51);
                cur_obj_scale(1.f);
            }
            break;
    }
}

#define RADIUS 900.f
void spikeballcode(void) {
    if (!o->oTimer) {
        o->oOpacity = 0x3800;
    }
    o->oPathedTargetPitch -= o->oOpacity / 350;
    o->oOpacity += o->oPathedTargetPitch;
    o->oPosX = o->oHomeX + sins(o->oMoveAngleYaw) * coss(o->oOpacity - 0x4000) * RADIUS;
    o->oPosZ = o->oHomeZ + coss(o->oMoveAngleYaw) * coss(o->oOpacity - 0x4000) * RADIUS;
    o->oPosY = o->oHomeY + sins(o->oOpacity - 0x4000) * RADIUS;
}

#define HALFTIME 37
void fishbonecode(void) {
    switch (o->oAction) {
        case 0:
        case 1:
            if (o->oTimer >= HALFTIME) {
                o->oAction++;
                o->oMoveAngleYaw += 0x4000;
            }
            break;
        default:
            if (o->oTimer >= HALFTIME * 2) {
                o->oAction++;
                o->oMoveAngleYaw += 0x4000;
            }
            break;
    }
    o->oForwardVel = 8.f;
    cur_obj_move_xz_using_fvel_and_yaw();
    load_object_collision_model();
}

void enginePart(void) {
    if (huntIsOn) {
        cur_obj_set_model(0x47);
        o->oFaceAngleYaw += 0x200;
        if (!(o->oTimer % 7)) {
            spawn_object(o, MODEL_NONE, bhvSparkleSpawn);
        }
        if (obj_check_if_collided_with_object(o, gMarioState->marioObj)) {
            huntIsOn++;
            cur_obj_play_sound_2(SOUND_GENERAL_SHORT_STAR);
            spawn_mist_particles_variable(0, 0, 46.f);
            obj_mark_for_deletion(o);
        }
    } else {
        cur_obj_set_model(0);
    }
}