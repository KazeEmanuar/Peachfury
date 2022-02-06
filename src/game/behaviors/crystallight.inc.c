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

extern Lights1 mario_body_lights2;
extern Lights1 mario_cap_lights2;
extern Lights1 mario_face_0___eye_open_lights2;
extern Lights1 mario_hair_lights2;
extern Lights1 mario_mustache_lights2;
extern Lights1 mario_sideburns_lights2;
extern Lights1 mario_face_1___eye_half_lights2;
extern Lights1 mario_face_2___eye_closed_lights2;
extern Lights1 mario_face_7___eye_X_lights2;
extern Lights1 mario_gloves_lights2;
extern Lights1 mario_shoes_lights2;
extern Lights1 cloudcapLights12;
extern Lights1 cloudcapLights22;
extern Lights1 cloudcapLights1;
extern Lights1 cloudcapLights2;

Lights1 fakeLights = gdSPDefLights1(0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);
Lights1 fakeLights2 = gdSPDefLights1(0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

extern Lights1 backupLights1;
extern Lights1 backupLights2;
Lights1 *lightsIn[] = { &mario_body_lights2,
                        &mario_cap_lights2,
                        &mario_face_0___eye_open_lights2,
                        &mario_hair_lights2,
                        &mario_mustache_lights2,
                        &mario_sideburns_lights2,
                        &mario_sideburns_lights2,
                        &mario_face_1___eye_half_lights2,
                        &mario_face_2___eye_closed_lights2,
                        &mario_face_7___eye_X_lights2,
                        &mario_gloves_lights2,
                        &mario_shoes_lights2,
                        &cloudcapLights12,
                        &cloudcapLights22,
                        &fakeLights2 };
Lights1 *lightsOut[] = { &backupLights1,
                         &backupLights2,
                         &mario_face_0___eye_open_lights,
                         &mario_hair_lights,
                         &mario_mustache_lights,
                         &mario_sideburns_lights,
                         &mario_sideburns_lights,
                         &mario_face_1___eye_half_lights,
                         &mario_face_2___eye_closed_lights,
                         &mario_face_7___eye_X_lights,
                         &mario_gloves_lights,
                         &mario_shoes_lights,
                         &cloudcapLights1,
                         &cloudcapLights2,
                         &fakeLights };
//(Lights1 *)(((int)&backupLights1) & 0x7fFFFFFF),

extern Gfx mat_cloudcapShade__03___Default_f3d[];

#define BRIGHTNESS 1.0f
void addLights(const BehaviorScript *behavior, u16 *RGBMult) {
    uintptr_t *behaviorAddr = segmented_to_virtual(behavior);
    struct Object *obj;
    struct ObjectNode *listHead;
    int i;
    u16 RGBMultAdd[3] = { 0x0, 0x0, 0x0 };
    /*Gfx *j = (Gfx *) segmented_to_virtual(&mat_cloudcapShade__03___Default_f3d[0x12]);
    j->words.w1 = &fakeLights;*/
    listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                f32 objDist = obj->oDistanceToMario;
                f32 maxdist = (((obj->oBehParams & 0x7f000000) >> 24) * 20.f) + 1200.f;
                if (objDist < maxdist) {
                    f32 multiplier = (BRIGHTNESS * (1.0f - (objDist / maxdist)));
                    RGBMultAdd[0] = ((obj->oBehParams & 0x00FF0000) >> 16) * multiplier;
                    RGBMultAdd[1] = ((obj->oBehParams & 0x0000FF00) >> 8) * multiplier;
                    RGBMultAdd[2] = ((obj->oBehParams & 0x000000FF)) * multiplier;
                    for (i = 0; i < 3; i++) {
                        RGBMult[i] += RGBMultAdd[i];
                        if (RGBMult[i] > 0xff) {
                            RGBMult[i] = 0xff;
                        }
                    }
                }
            }
        }
        obj = (struct Object *) obj->header.next;
    }
}

#define DIVISOR 255.f

void crystallightcode(void) {
    int k;
    u16 RGBMult[3] = { 0x7f, 0x7f, 0x7f };

    if (o->oBehParams & 0x80000000) {
        f32 multiplicators[3];
        Lights1 *i;
        Lights1 *j;
        addLights(bhvLight, RGBMult);
        multiplicators[0] = (((float) RGBMult[0]) / DIVISOR);
        multiplicators[1] = (((float) RGBMult[1]) / DIVISOR);
        multiplicators[2] = (((float) RGBMult[2]) / DIVISOR);
        for (k = 0; k < 15; k++) {
            i = (Lights1 *) (lightsIn[k]);
            j = (Lights1 *) segmented_to_virtual((int) lightsOut[k] & 0x7fFFFFFF);
            j->a.l.col[0] = i->a.l.col[0] * multiplicators[0];
            j->a.l.col[1] = i->a.l.col[1] * multiplicators[1];
            j->a.l.col[2] = i->a.l.col[2] * multiplicators[2];
            j->a.l.colc[0] = j->a.l.col[0];
            j->a.l.colc[1] = j->a.l.col[1];
            j->a.l.colc[2] = j->a.l.col[2];

            j->l->l.col[0] = i->l->l.col[0] * multiplicators[0];
            j->l->l.col[1] = i->l->l.col[1] * multiplicators[1];
            j->l->l.col[2] = i->l->l.col[2] * multiplicators[2];
            j->l->l.colc[0] = j->l->l.col[0];
            j->l->l.colc[1] = j->l->l.col[1];
            j->l->l.colc[2] = j->l->l.col[2];
        }
    }
}

struct Object *MirrorArray[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int mirrorcount = sizeof(MirrorArray) / 4;

Vec3f mirrorbooPos = { 0, 0, 0 };
#define MINZMIRROR -6000
#define MAXZMIRROR -208
#define MINXMIRROR -14704
#define MAXXMIRROR -4633
void mirrorcode(void) {
    struct Object *obj;
    struct ObjectNode *listHead;
    int i, j;
    if (!o->oTimer) {
        for (i = 0; i < mirrorcount; i++) {
            MirrorArray[i] = 0;
        }
    }
    for (i = 0; i < mirrorcount; i++) {
        if (MirrorArray[i]) {
            MirrorArray[i]->activeFlags = 0;
        }
        MirrorArray[i] = 0;
    }
    if (o->oDistanceToMario < 10000.f) {
        i = 0;
        for (j = 1; j < 16; j++) {
            listHead = &gObjectLists[j];
            obj = (struct Object *) listHead->next;
            if (obj) {
                while (obj != (struct Object *) listHead) {
                    if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                        if (obj->header.gfx.sharedChild) {
                            if (obj->oBehParams != 0xDEADBEEF) {
                                if (!(o->oBehParams & 0xff)) {
                                    if ((obj->oPosZ > MINZMIRROR) && (obj->oPosZ < MAXZMIRROR)
                                        && (obj->oPosX > MINXMIRROR) && (obj->oPosX < MAXXMIRROR)) {
                                        MirrorArray[i] = spawn_object(obj, 0, bhvMirroredObject);
                                        MirrorArray[i]->oPosZ = MAXZMIRROR - (obj->oPosZ - MAXZMIRROR);
                                        MirrorArray[i]->oPosY = obj->oPosY;
                                        MirrorArray[i]->oPosX = obj->oPosX;

                                        MirrorArray[i]->header.gfx.sharedChild =
                                            obj->header.gfx.sharedChild;
                                        MirrorArray[i]->header.gfx.scale[0] = obj->header.gfx.scale[0];
                                        MirrorArray[i]->header.gfx.scale[1] = obj->header.gfx.scale[1];
                                        MirrorArray[i]->header.gfx.scale[2] = -obj->header.gfx.scale[2];
                                        MirrorArray[i]->header.gfx.node.flags =
                                            obj->header.gfx.node.flags;
                                        MirrorArray[i]->oFaceAngleYaw = -obj->oFaceAngleYaw;
                                        MirrorArray[i]->oAnimState = obj->oAnimState;
                                        if (j == 6) {
                                            MirrorArray[i]->oAnimState = 16 - obj->oAnimState;
                                        }
                                        MirrorArray[i]->header.gfx.unk38.animFrame =
                                            obj->header.gfx.unk38.animFrame;
                                        MirrorArray[i]->header.gfx.unk38.animYTrans =
                                            obj->header.gfx.unk38.animYTrans;
                                        MirrorArray[i]->header.gfx.unk38.curAnim =
                                            obj->header.gfx.unk38.curAnim;
                                        MirrorArray[i]->header.gfx.unk38.animTimer =
                                            obj->header.gfx.unk38.animTimer;
                                        MirrorArray[i]->header.gfx.unk38.animFrameAccelAssist =
                                            obj->header.gfx.unk38.animFrameAccelAssist;

                                        if (obj->behavior == segmented_to_virtual(bhvGoomba)) {
                                            MirrorArray[i]->header.gfx.sharedChild =
                                                gLoadedGraphNodes[0x3c];
                                        }
                                        if (obj->behavior == segmented_to_virtual(bhvBoonardHelp)) {
                                            vec3f_copy(mirrorbooPos, &MirrorArray[i]->oPosX);
                                            mirrorbooPos[1] -= 50.f;
                                        }
                                        MirrorArray[i]->oBehParams = 0xDEADBEEF;
                                        i++;
                                        if (i == mirrorcount) {
                                            return;
                                        }
                                    }
                                } else {
                                    if ((obj->oPosZ > -5000.f) && (obj->oPosZ < 5000.f)
                                        && (obj->oPosX > -5000.f) && (obj->oPosX < 5000.f)
                                        && (obj->oPosY >= -433.f)) {
                                        MirrorArray[i] = spawn_object(obj, 0, bhvMirroredObject);
                                        MirrorArray[i]->oPosZ = obj->oPosZ;
                                        MirrorArray[i]->oPosY = -433.f - (obj->oPosY + 433.f);
                                        MirrorArray[i]->oPosX = obj->oPosX;

                                        MirrorArray[i]->header.gfx.sharedChild =
                                            obj->header.gfx.sharedChild;
                                        MirrorArray[i]->header.gfx.scale[0] = obj->header.gfx.scale[0];
                                        MirrorArray[i]->header.gfx.scale[1] = obj->header.gfx.scale[1];
                                        MirrorArray[i]->header.gfx.scale[2] = obj->header.gfx.scale[2];
                                        MirrorArray[i]->header.gfx.node.flags =
                                            obj->header.gfx.node.flags;
                                        MirrorArray[i]->oFaceAngleYaw = obj->oFaceAngleYaw;
                                        MirrorArray[i]->oFaceAngleRoll = -obj->oFaceAngleRoll - 0x8000;
                                        MirrorArray[i]->oAnimState = obj->oAnimState;
                                        if (j == 6) {
                                            MirrorArray[i]->oAnimState =
                                                (16 - obj->oAnimState + 5) % 16;
                                            MirrorArray[i]->header.gfx.scale[1] =
                                                -obj->header.gfx.scale[1];
                                            MirrorArray[i]->header.gfx.scale[0] =
                                                -obj->header.gfx.scale[0];
                                        }
                                        MirrorArray[i]->header.gfx.unk38.animFrame =
                                            obj->header.gfx.unk38.animFrame;
                                        MirrorArray[i]->header.gfx.unk38.animYTrans =
                                            obj->header.gfx.unk38.animYTrans;
                                        MirrorArray[i]->header.gfx.unk38.curAnim =
                                            obj->header.gfx.unk38.curAnim;
                                        MirrorArray[i]->header.gfx.unk38.animTimer =
                                            obj->header.gfx.unk38.animTimer;
                                        MirrorArray[i]->header.gfx.unk38.animFrameAccelAssist =
                                            obj->header.gfx.unk38.animFrameAccelAssist;

                                        MirrorArray[i]->oBehParams = 0xDEADBEEF;
                                        i++;
                                        if (i == mirrorcount) {
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    obj = (struct Object *) obj->header.next;
                }
            }
        }
    }
}

int buttontimer = 0;
extern Gfx mat_jrb_dl_buttonanimation[];
extern u8 jrb_dl_button1_rgba16[];
extern const u8 button2[];
void yoshicart(void) {
    f32 calcheight;
    s16 goalpitch;
    f32 calcheightroll;
    s16 goalroll;
    s16 goalyaw;
    f32 floorHeight;
    f32 speed;
    f32 x, y, z;
    int i;
    buttontimer++;
    if (!(buttontimer & 0x0F)) {
        Gfx *a = segmented_to_virtual(mat_jrb_dl_buttonanimation);
        if (buttontimer & 0x10) {
            a[5].words.w1 = jrb_dl_button1_rgba16;
        } else {
            a[5].words.w1 = button2;
        }
    }
    o->oAnimState = !(buttontimer & 0xfE);
    switch (o->oAction) {
        case 0:
            o->header.gfx.unk38.animFrame = 0;
            load_object_collision_model();
            if (cur_obj_is_mario_on_platform()) {
                o->oAction++;
                gMarioState->action = ACT_RIDE_MINECART;
            }
            break;
        case 1:
            o->header.gfx.unk38.animFrame = gMarioState->marioObj->header.gfx.unk38.animFrame;
            if (gMarioState->controller->buttonPressed & B_BUTTON) {
                o->oForwardVel += 4.0f;
                o->oOpacity += 5;
                if (o->oOpacity > 5) {
                    o->oOpacity = 5;
                };
            }
            if (o->oOpacity) {
                o->oOpacity--;
                gMarioState->marioObj->header.gfx.unk38.animFrame++;
                if (gMarioState->marioObj->header.gfx.unk38.animFrame == 9) {
                    cur_obj_play_sound_2(SOUND_GENERAL_OPEN_IRON_DOOR);
                }
            }
            o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 0, 0.005f);
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0, 0.05f);
            calcheight = o->oPosY;
            if (o->oForwardVel > 80.f) {
                o->oForwardVel = 80.f;
            }
            o->oForwardVel = o->oForwardVel / 4.0f;
            o->oVelY = o->oVelY / 4.0f;
            for (i = 0; i < 4; i++) {
                cur_obj_move_using_fvel_and_gravity();
                cur_obj_resolve_wall_collisions();
            }
            o->oVelY = o->oVelY * 4.0f;
            o->oForwardVel = o->oForwardVel * 4.0f;
            o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloorPointer);
            if (o->oFloorPointer) {
                o->oForwardVel +=
                    sqrtf(1.f - o->oFloorPointer->normal.y)
                    * coss((atan2s(o->oFloorPointer->normal.z, o->oFloorPointer->normal.x) - o->oMoveAngleYaw) & 0xffff)
                    * 0.75f;
            }
            o->oFloorPointer = 0;
            if (o->oPosY < o->oFloorHeight) {
                if (!(o->oMoveFlags & OBJ_MOVE_ON_GROUND)) {
                    if (o->oVelY < -20.f) {
                        cur_obj_play_sound_2(SOUND_GENERAL_METAL_POUND);
                        spawn_object(o, 0, bhvHorStarParticleSpawner);
                    }
                }
                o->oPosY = o->oFloorHeight;
                o->oVelY = 0;
                o->oMoveFlags |= OBJ_MOVE_ON_GROUND;
                /*o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, m->intendedYaw,
                0x120); o->oFaceAngleRoll += (o->oFaceAngleYaw - o->oMoveAngleYaw);*/
            } else if ((o->oMoveFlags & OBJ_MOVE_ON_GROUND)
                       && ((absf(o->oPosY - o->oFloorHeight) < 70.f))) {
                o->oPosY = o->oFloorHeight;
                o->oVelY = 0;
                o->oMoveFlags |= OBJ_MOVE_ON_GROUND;
                /*  o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, m->intendedYaw,
                  0x120); o->oFaceAngleRoll += (o->oFaceAngleYaw - o->oMoveAngleYaw);*/
            } else {
                o->oMoveFlags &= ~OBJ_MOVE_ON_GROUND;
            }
            if (absf(o->oVelY) < 8.f) {
                calcheight = o->oPosY - calcheight;
                if (o->oForwardVel > 1.0f) {

                    goalpitch = -(calcheight / o->oForwardVel * 0x2000);
                } else {
                    goalpitch = o->oFaceAnglePitch;
                }
            } else {
                goalpitch = 0;
            }
            if (absf(o->oVelY) < 8.f) {
                floorHeight = find_floor(o->oPosX + sins(o->oMoveAngleYaw + 0x4000) * 20.f, o->oPosY,
                                         o->oPosZ + coss(o->oMoveAngleYaw + 0x4000) * 20.f, &o->oFloorPointer);
                calcheightroll = find_floor(o->oPosX + sins(o->oMoveAngleYaw - 0x4000) * 20.f, o->oPosY,
                                            o->oPosZ + coss(o->oMoveAngleYaw - 0x4000) * 20.f, &o->oFloorPointer);
                goalroll = (((floorHeight - calcheightroll) / 40.f) * 0x2000);
            } else {
                goalroll = 0;
                o->oAngleVelPitch *= 0.90f;
                o->oAngleVelRoll *= 0.90f;
                o->oFaceAnglePitch *= 0.90f;
                o->oFaceAngleRoll *= 0.90f;
            }
            if (abs_angle_diff(o->oWallAngle + 0x4000, o->oMoveAngleYaw)
                < abs_angle_diff(o->oWallAngle - 0x4000, o->oMoveAngleYaw)) {
                goalyaw = o->oWallAngle + 0x4000;
            } else {
                goalyaw = o->oWallAngle - 0x4000;
            }
            o->oAngleVelYaw = goalyaw - o->oMoveAngleYaw;
            o->oMoveAngleYaw = goalyaw;
            o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, o->oMoveAngleYaw,
                                                      0x0200 * (o->oForwardVel / 20.f));
            o->oAngleVelPitch += (goalpitch - o->oFaceAnglePitch) / 10;
            o->oFaceAngleRoll += (goalroll - o->oFaceAngleRoll) / 10;
            o->oAngleVelPitch *= 0.70f;
            o->oAngleVelRoll *= 0.70f;
            o->oFaceAnglePitch += o->oAngleVelPitch;
            o->oFaceAngleRoll += o->oAngleVelRoll;
            if ((o->oPosZ < -6541.f)) {
                if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
                    o->oAction++;
                    gMarioState->action = ACT_THROWN_FORWARD;
                    gMarioState->vel[1] = o->oForwardVel / 1.414f;
                    gMarioState->forwardVel = o->oForwardVel / 1.414f;
                    gMarioState->pos[1] += 100.f;
                    o->oAngleVelPitch = 0x1000;
                    cur_obj_play_sound_2(SOUND_GENERAL_BREAK_BOX);
                    cur_obj_play_sound_2(SOUND_GENERAL_POUND_ROCK2);
                    o->oForwardVel = 0;
                    o->oVelY = 40.f;
                } else {
                    o->oPosZ = -6541.f;
                    o->oMoveAngleYaw = 0x8000;
                    o->oFaceAngleYaw = 0x8000;
                }
            }
            break;
        case 2:
            o->oFaceAnglePitch += o->oAngleVelPitch;
            o->oFaceAngleRoll = approach_s16_symmetric(o->oFaceAngleRoll, 0, 0x200);
            o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0x200);
            o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 0, 0x200);
            o->oForwardVel = o->oForwardVel / 4.0f;
            o->oVelY = o->oVelY / 4.0f;
            for (i = 0; i < 4; i++) {
                cur_obj_move_using_fvel_and_gravity();
                cur_obj_resolve_wall_collisions();
            }
            o->oVelY = o->oVelY * 4.0f;
            o->oForwardVel = o->oForwardVel * 4.0f;
            o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloorPointer);
            if (o->oPosY < o->oFloorHeight) {
                o->oPosY = o->oFloorHeight;
                o->oVelY = 0;
            }
            if (o->header.gfx.unk38.animFrame > 0) {
                o->header.gfx.unk38.animFrame--;
            }
            load_object_collision_model();
            break;
    }
}

// mine with pickaxe
// run around (not holding anything)
// hold hat
// stand around with crystal
extern struct CutsceneJump castleGroundScene1;
void mouser(void) {
    cur_obj_scale(1.4f);
    o->oAnimState = o->oBehParams & 0xFF;
    switch (o->oBehParams & 0xFF) {
        case 0:
            if (!o->oTimer) {
                o->header.gfx.unk38.animFrame = random_u16() & 0x1f;
            }
            o->oInteractType = 0x40000000;
            cur_obj_init_animation(1);
            if (currentScene != &castleGroundScene1 && o->header.gfx.unk38.animFrame == 7) {
                cur_obj_play_sound_2(SOUND_OBJ_BULLY_METAL);
            }
            break;
        case 1:
            o->oGravity = -4.f;
            o->oInteractType = 0x40000000;
            o->oForwardVel = 40.f;
            if (o->oTimer > 22) {
                o->oMoveAngleYaw += 0x8000;
                o->oTimer = 0;
            }
            cur_obj_init_animation(0);
            cur_obj_update_floor_and_walls();
            cur_obj_move_standard(-78);
            break;
        case 2:
            o->hitboxRadius = 200.f;
            o->hitboxHeight = 120.f;
            bhv_bobomb_buddy_loop();
            o->oGraphYOffset = -50.f;
            cur_obj_scale(2.0f);
            cur_obj_init_animation(2);
            break;
        case 3:
        case 4:
            cur_obj_init_animation(3);
            bhv_bobomb_buddy_loop();
            break;
    }
    //  cur_obj_update_floor_and_walls();
    //  cur_obj_move_standard(-78);
}

void breakbridge(void) {
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_ground_pounding_platform()) {
                if (lateral_dist_between_objects(o, gMarioState->marioObj) < 250.f) {
                    cur_obj_play_sound_2(SOUND_GENERAL_CLOSE_WOOD_DOOR);
                    cur_obj_play_sound_2(SOUND_GENERAL2_WALL_EXPLOSION);
                    o->oAction++;
                    spawn_mist_particles_variable(0, 0, 74.f);
                    spawn_triangle_break_particles(30, 138, 3.0f, 4);
                }
            }
            load_object_collision_model();
            break;
        case 1:
            cur_obj_init_animation(0);
            if (o->oTimer < 6) {
                load_object_collision_model();
            }
            break;
    }
    //  cur_obj_update_floor_and_walls();
    //  cur_obj_move_standard(-78);
}

extern Gfx mat_boonard_booeyes[];
extern Gfx mat_boonard_boomouth[];
extern Gfx mat_boonard_boobrows[];
extern Gfx mat_boonard_boostache[];

void boo_spawn_sparkles(f32 a) {
    struct Object *sp1C;
    sp1C = spawn_object(o, MODEL_NONE, bhvSparkleSpawn);
    sp1C->oPosY += a;
}

void setTransparency(int i) {
    Gfx *a;
    a = segmented_to_virtual(mat_boonard_booeyes);
    a[0x12].words.w1 = 0xffffff00 + i;
    a = segmented_to_virtual(mat_boonard_boomouth);
    a[0x12].words.w1 = 0xffffff00 + i;
    a = segmented_to_virtual(mat_boonard_boobrows);
    a[0xc].words.w1 = 0xffffff00 + i;
    a = segmented_to_virtual(mat_boonard_boostache);
    a[0x13].words.w1 = 0xffffff00 + i;
}
void positionBoo() {
    o->oMoveAngleYaw = o->oHiddenBlueCoinSwitch->oFaceAngleYaw;
    vec3f_copy(&o->oPosX, gMarioState->pos);
    o->oPosX += sins(o->oMoveAngleYaw) * 250.f;
    o->oPosZ += coss(o->oMoveAngleYaw) * 250.f;
    o->oPosY += 100.f;
    o->oMoveAngleYaw = o->oAngleToMario;
}
// 0x12
void boonard(void) {
    int i;
    switch (o->oAction) {
        case 0:
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            if (o->oBobombBuddyRole == 0x3F) {
                o->oAction = 6;
            }
            setTransparency(0x7f);
            if (o->oOpacity) {
                o->oAction = 2;
            }
            break;
        case 1:
            if (o->oHiddenBlueCoinSwitch->oBehParams & 0xFF) {
                i = askMario(o->oOpacity, 1, 4);
                switch (i) {
                    case 0:

                        break;
                    case 1:
                        // yes
                        o->oOpacity = (o->oHiddenBlueCoinSwitch->oBehParams >> 8) & 0xff;
                        o->oHiddenBlueCoinSwitch->oBehParams =
                            o->oHiddenBlueCoinSwitch->oBehParams & 0xFFFFFF00;
                        break;
                    case 2:
                        // no
                        o->oOpacity = o->oHiddenBlueCoinSwitch->oBehParams & 0xff;
                        o->oHiddenBlueCoinSwitch->oBehParams =
                            o->oHiddenBlueCoinSwitch->oBehParams & 0xFFFFFF00;
                        break;
                    case 3:

                        break;
                }
            } else {
                if (talkToMario(o->oOpacity, 4)) {
                    o->oAction = 4;
                    o->oOpacity = 0;
                }
            }
            break;
        case 2:
            setTransparency(0x7f - (o->oTimer * 0xF) - 7);
            if ((0x7f - (o->oTimer * 0xF) - 7) == 0) {
                positionBoo();
                o->oAction = 3;
            }
            break;
        case 3:
            setTransparency((o->oTimer * 0xF) + 7);
            if (((o->oTimer * 0xF) + 7) > 126) {
                o->oAction = 1;
            }
            break;

        case 4:
            setTransparency(0x7f - (o->oTimer * 0xF) - 7);
            if ((0x7f - (o->oTimer * 0xF) - 7) == 0) {
                vec3f_copy(&o->oPosX, &o->oHomeX);
                o->oAction = 5;
            }
            break;
        case 5:
            if (o->oTimer > 13) {
                setTransparency(((o->oTimer - 14) * 0xF) + 7);
                if (((o->oTimer * 0xF) + 7) > 126) {
                    o->oAction = 0;
                }
            }
            break;
        case 6:
            if (talkToMario(140, 4)) {
                o->oAction = 0;
                o->oBobombBuddyRole = 0x20;
                spawn_default_star(0, -350.0f, 0.0f);
            }
            break;
    }
}

void boonardboss(void) {
    switch (o->oAction) {
        case 0:
            // talk to mario for intro
            setTransparency(0x7f);
            if ((o->oTimer > 110) && (!gCurrentArea->camera->cutscene)) {
                if (talkToMario(134, 4)) {
                    o->oAction = 1;
                    o->oHealth = 0;
                }
            }
            break;
        case 1:
            // disappear
            cur_obj_move_using_vel();
            setTransparency(0x7f - (o->oTimer * 0xF) - 7);
            o->oPosY -= 10.f;
            if ((0x7f - (o->oTimer * 0xF) - 7) <= 0) {
                o->oAction = 2;
                cur_obj_set_model(0);
            }
            break;
        case 2:
            // wait disappeared
            if (o->oTimer > (0x1c0 - o->oHealth * 0xc0)) {
                o->oAction = 3 + o->oHealth;
                o->oMoveAngleYaw = atan2s(gMarioState->pos[2], gMarioState->pos[0]) - 0x8000;
                vec3f_copy(&o->oPosX, gMarioState->pos);
                o->oPosY = 100.f;
                o->oForwardVel = 700.f;
                cur_obj_move_xz_using_fvel_and_yaw();
                o->oForwardVel = 0.f;
                o->oMoveAngleYaw = obj_angle_to_object(o, gMarioState->marioObj);
                cur_obj_move_xz_using_fvel_and_yaw();
            }
            break;
        case 3:
            if (!o->oTimer) {
                o->oHiddenBlueCoinSwitch = spawn_object(o, 0x42, bhvTNT);
            }
            vec3f_copy(&o->oHiddenBlueCoinSwitch->oPosX, &o->oPosX);
            o->oHiddenBlueCoinSwitch->oVelY = 0;
            cur_obj_set_model(0x3b);
            // reappear, talk about how he found TNT just once during this fight
            if (!o->oSubAction) {
                setTransparency(((o->oTimer - 14) * 0xF) + 7);
                if (((o->oTimer * 0xF) + 7) > 126) {
                    o->oSubAction = 1;
                }
            } else {
                if (talkToMario(133, 4)) {
                    o->oAction = 5;
                    o->oHealth = 1;
                }
            }
            break;
        case 4:
            // reappear, no TNT talk
            if (!o->oTimer) {
                o->oHiddenBlueCoinSwitch = spawn_object(o, 0x42, bhvTNT);
            }
            vec3f_copy(&o->oHiddenBlueCoinSwitch->oPosX, &o->oPosX);
            o->oHiddenBlueCoinSwitch->oVelY = 0;
            cur_obj_set_model(0x3b);
            setTransparency(((o->oTimer - 14) * 0xF) + 7);
            if (((o->oTimer * 0xF) + 7) > 126) {
                o->oAction = 5;
            }
            break;
        case 5:
            // follow mario a bit, drop tnt
            vec3f_copy(&o->oHiddenBlueCoinSwitch->oPosX, &o->oPosX);
            o->oHiddenBlueCoinSwitch->oVelY = 0;
            if ((o->oTimer < 60) || gMarioState->heldObj) {
                o->oHomeX = sins(o->oAngleToMario - 0x8000) * 500.f + gMarioState->pos[0];
                o->oHomeY = 120.f + gMarioState->pos[1];
                o->oHomeZ = coss(o->oAngleToMario - 0x8000) * 500.f + gMarioState->pos[2];
                o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x300);
                o->oForwardVel = approach_f32_symmetric(
                    o->oForwardVel,
                    (coss(o->oMoveAngleYaw - atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX) + 1.0f)
                     * 28.f * (o->oDistanceToMario > 500.f) * (o->oDistanceToMario / 2000.f)),
                    0.4f); // todo do this again, the brackets are all wrong
                o->oForwardVel *= 0.99f;
                o->oVelY = approach_f32_symmetric(o->oVelY, (o->oHomeY - o->oPosY) * 0.2f, 2.0f);
                o->oVelY *= 0.95f;
                cur_obj_move_xz_using_fvel_and_yaw();
                o->oPosY += o->oVelY;
            } else {
                o->oAction = 1;
            }
            break;
        case 6:
            cur_obj_set_model(0x3b);
            setTransparency(0x7f);
            if (talkToMario(131, 4)) {
                o->oAction = 7;
            }
            break;
        case 7:
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x300);
            setTransparency(0x7f - (o->oTimer * 8));
            if (o->oTimer > 14) {
                cur_obj_set_model(0);
            }
            break;
    }
    if (!cur_obj_nearest_object_with_behavior(bhvPiranhaBoss)) {
        if (o->oAction < 6) {
            o->oAction = 6;
            if (o->oAction < 3) {
                o->oMoveAngleYaw = atan2s(gMarioState->pos[2], gMarioState->pos[0]) - 0x8000;
                vec3f_copy(&o->oPosX, gMarioState->pos);
                o->oPosY = 100.f;
                o->oForwardVel = 700.f;
                cur_obj_move_xz_using_fvel_and_yaw();
                o->oForwardVel = 0.f;
                o->oMoveAngleYaw = obj_angle_to_object(o, gMarioState->marioObj);
                cur_obj_move_xz_using_fvel_and_yaw();
            }
        }
    } else if (cur_obj_nearest_object_with_behavior(bhvPiranhaBoss)->oAction == 6) {
        o->oAction = 2;
        o->oTimer = 0;
    }
}

void nipperdoor(void) {
    if (o->oAction) {
        o->oPosY -= 8.f;
        if (o->oTimer > 80) {
            obj_mark_for_deletion(o);
        }
    }
    if (cur_obj_dist_to_nearest_object_with_behavior(bhvNipperKey) < 700.f) {
        o->oAction = 1;
        cur_obj_nearest_object_with_behavior(bhvNipperKey)->oAction = 2;
        cur_obj_play_sound_2(SOUND_GENERAL_STAR_DOOR_OPEN);
    }
    load_object_collision_model();
}

void nipperkey(void) {
    switch (o->oAction) {
        case 0:
            boo_spawn_sparkles(25.f);
            o->oFaceAngleYaw += 0x300;
            if (vec3f_dist(mirrorbooPos, &o->oPosX) < 320.f) {
                o->oAction++;
                cur_obj_play_sound_2(SOUND_GENERAL_UNKNOWN3_2);
            }
            break;
        case 1:
            vec3f_copy(&o->oPosX, &cur_obj_nearest_object_with_behavior(bhvBoonardHelp)->oPosX);
            o->oPosY -= 60.f;
            break;
        case 2:
            obj_mark_for_deletion(o);
            spawn_object(o, MODEL_NONE, bhvStarKeyCollectionPuffSpawner);
            break;
    }
}

void boonardhelp(void) {
    setTransparency(0xA0);
    switch (o->oAction) {
        case 0:
            if (o->oTimer > 10) {
                o->oAction++;
            }
            break;
        case 1:
            if (talkToMario(135, 4)) {
                o->oAction++;
            }
            break;
        case 2:
            o->oHomeX = sins(o->oAngleToMario - 0x8000) * 500.f + gMarioState->pos[0];
            o->oHomeY = 120.f + gMarioState->pos[1];
            o->oHomeZ = coss(o->oAngleToMario - 0x8000) * 500.f + gMarioState->pos[2];
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x300);
            break;
    }
    o->oForwardVel = approach_f32_symmetric(
        o->oForwardVel,
        (coss(o->oMoveAngleYaw - atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX) + 1.0f) * 28.f
         * (o->oDistanceToMario > 500.f) * (o->oDistanceToMario / 2000.f)),
        0.4f);
    o->oForwardVel *= 0.99f;
    o->oVelY = approach_f32_symmetric(o->oVelY, (o->oHomeY - o->oPosY) * 0.2f, 2.0f);
    o->oVelY *= 0.95f;
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oPosY += o->oVelY;
}

void boonardtrigger(void) {
    if (!o->oOpacity) {
        o->oTimer = random_u16();
    }
    if (!o->oAction) {
        if (!(o->oTimer & 3)) {
            boo_spawn_sparkles(25.f);
        }
        if (o->oDistanceToMario < 100.f) {
            if (!cur_obj_nearest_object_with_behavior(bhvBoonard)->oOpacity) {
                cur_obj_nearest_object_with_behavior(bhvBoonard)->oOpacity = o->oBehParams2ndByte;
                cur_obj_nearest_object_with_behavior(bhvBoonard)->oBobombBuddyRole |=
                    o->oBehParams >> 24;
                cur_obj_nearest_object_with_behavior(bhvBoonard)->oHiddenBlueCoinSwitch = o;
                o->oAction = 1;
            }
        }
    }
}
f32 dist_sq(float px, float py, float pz, float lx1, float ly1, float lz1) {
    f32 x, y, z;
    x = px - lx1;
    y = py - ly1;
    z = pz - lz1;
    return (x * x + y * y + z * z);
}

f32 constrain(float f) {
    if (f < 0) {
        return 0;
    }
    if (f > 1) {
        return 1;
    }
    return f;
}
float dist_to_segment(float px, float py, float pz, float lx1, float ly1, float lz1, float lx2,
                      float ly2, float lz2) {
    float line_dist = dist_sq(lx1, ly1, lz1, lx2, ly2, lz2);
    float t;
    if (line_dist == 0) {
        return dist_sq(px, py, pz, lx1, ly1, lz1);
    }
    t = ((px - lx1) * (lx2 - lx1) + (py - ly1) * (ly2 - ly1) + (pz - lz1) * (lz2 - lz1)) / line_dist;
    t = constrain(t);
    return sqrtf(
        dist_sq(px, py, pz, lx1 + t * (lx2 - lx1), ly1 + t * (ly2 - ly1), lz1 + t * (lz2 - lz1)));
}
extern Vec3f joinPosition[6][4];
void vinewhip(void) {
    int i;
    switch (o->oAction) {
        case 0:
            cur_obj_init_animation(0);
            // o->header.gfx.unk38.animFrame = 0;
            if (!o->oTimer) {
                cur_obj_scale(0.f);
            }
            cur_obj_scale(o->header.gfx.scale[1] + 0.05f);
            o->oAction += (o->oTimer > 19);
            break;
        case 1:
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction++;
            }
            if (o->oTimer == 10) {
                play_sound(SOUND_GENERAL_SWISH_AIR, gDefaultSoundArgs);
            }
            if (o->oTimer == 26) {
                play_sound(SOUND_GENERAL_POUND_ROCK, gDefaultSoundArgs);
            }
            break;
        case 2:
            if (o->oTimer > 19) {
                cur_obj_scale(o->header.gfx.scale[1] - 0.02f);
            }
            if (o->oTimer > 69) {
                obj_mark_for_deletion(o);
            }
            break;
    }
    for (i = 0; i < 4; i++) {
        if (dist_to_segment(gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2],
                            joinPosition[i][o->oAnimState][0], joinPosition[i][o->oAnimState][1],
                            joinPosition[i][o->oAnimState][2], joinPosition[i + 1][o->oAnimState][0],
                            joinPosition[i + 1][o->oAnimState][1],
                            joinPosition[i + 1][o->oAnimState][2])
            < 150.f) {
            if (m->action != ACT_LAVA_BOOST) {
                o->oTimer = 0;
                drop_and_set_mario_action(m, ACT_LAVA_BOOST, 0);
                gMarioState->hurtCounter = 8;
            }
        }
    }
}

void spawn_earth() {
    static struct SpawnParticlesInfo sMontyMoleRiseFromGroundParticles = {
        /* behParam:        */ 0,
        /* count:           */ 25,
        /* model:           */ MODEL_SAND_DUST,
        /* offsetY:         */ 0,
        /* forwardVelBase:  */ 30,
        /* forwardVelRange: */ 15,
        /* velYBase:        */ 25,
        /* velYRange:       */ 8,
        /* gravity:         */ -3,
        /* dragStrength:    */ 0,
        /* sizeBase:        */ 15.0f,
        /* sizeRange:       */ 7.0f,
    };

    cur_obj_spawn_particles(&sMontyMoleRiseFromGroundParticles);
}
#include "audio/load.h"

static s8 endvinesanim;

/*
#define SEQUENCE_PLAYERS 3
extern struct SequencePlayer;
extern struct SequencePlayer gSequencePlayers[SEQUENCE_PLAYERS];*/
extern f32 find_nearest_TNT(struct Object *child, struct Object **tnt);
extern Vtx jrb_dl_spinningdiscs_mesh_vtx_0[30];
void piranhaboss(void) {
    Vtx *a = segmented_to_virtual(jrb_dl_spinningdiscs_mesh_vtx_0);
    int i;
    u16 dir;
    f32 mag = 1150.f;
    struct Object *spawn;
    struct Object *TNT;
    for (i = 0; i < 30; i++) {
        dir = atan2s(a[i].n.tc[0], a[i].n.tc[1]);
        // mag = sqrtf(a[i].n.tc[0]*a[i].n.tc[0] + a[i].n.tc[1]*a[i].n.tc[1]);
        dir += 0x800;
        a[i].n.tc[1] = sins(dir) * mag;
        a[i].n.tc[0] = coss(dir) * mag;
    }
    if (o->oAction != 0) {
#define VOLUME 2.5f
        /*  struct SequencePlayer *seqPlayer = &gSequencePlayers[SEQ_PLAYER_LEVEL];
          seqPlayer->fadeVelocity = 0;
          seqPlayer->fadeTimer = 0;
          seqPlayer->fadeVolume = VOLUME;
          gSequencePlayers[SEQ_PLAYER_LEVEL].volume = VOLUME;*/
    }

    switch (o->oAction) {
        case 0:
            // intro cutscene
            o->oAnimState = 3;
            joinPosition[0][o->oAnimState][0] = o->oPosX;
            joinPosition[0][o->oAnimState][1] = o->oPosY;
            joinPosition[0][o->oAnimState][2] = o->oPosZ;
            cur_obj_init_animation(1);
            o->header.gfx.unk38.animFrame = 0;
            o->oMoveAngleYaw = o->oAngleToMario;
            if (gMarioState->pos[1] == gMarioState->floorHeight) {
                if (!o->oHiddenBlueCoinSwitch) {
                    o->oHiddenBlueCoinSwitch = spawn_object(o, 0, bhvPiranhaCutsceneDummy);
                }
                if (set_mario_npc_dialog(2) == 2
                    && cutscene_object(CUTSCENE_STAR_SPAWN, o->oHiddenBlueCoinSwitch) == 1) {
                    o->oAction = 1;
                    //  play_sound(SOUND_GENERAL_WATER_LEVEL_TRIG, gDefaultSoundArgs);
                    play_sequence(SEQ_PLAYER_LEVEL, SEQ_BOSS_PIRANHA, 0);
                }
            }
            break;
        case 1:
            // intro cutscene
            if (o->oTimer == 20) {
                cur_obj_play_sound_2(SOUND_OBJ_EEL);
            }
            if (o->oTimer == 5) {
                spawn_earth();
                cur_obj_play_sound_2(SOUND_OBJ_PIRANHA_PLANT_APPEAR);
            }
            if (o->oTimer > 110) {
                gObjCutsceneDone = TRUE;
                if (cutscene_object(CUTSCENE_STAR_SPAWN, o->oHiddenBlueCoinSwitch) == -1) {
                    set_mario_npc_dialog(0);
                    o->oAction = 2;
                    o->oOpacity = (random_u16() & 0x7F) + 0x200;
                    o->oHealth = 0;
                    o->oHiddenBlueCoinSwitch = spawn_object(o, 0, bhvHitbox);
                }
            }
            break;
        case 2:
            // wait idly
            /*  if (cur_obj_nearest_object_with_behavior(bhvMouseTalk)) {
                  cur_obj_nearest_object_with_behavior(bhvMouseTalk)->activeFlags = 0;
              }*/
            cur_obj_init_animation(0);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            if (o->oOpacity) {
                o->oOpacity--;
            } else {
                o->oAction = 3;
            }
            if (o->oTimer > 80 - (o->oHealth * 15)) {
                if (o->oOpacity > 30) {
                    o->oTimer = 0;
                    // spawn these with random or predictive offsets if taken damage
                    cur_obj_play_sound_1(SOUND_ENV_BOAT_ROCKING1);
                    spawn = spawn_object(o, 0x41, bhvPiranhaAttack);
                    // also set oanimstate
                    spawn->oPosX += sins(o->oMoveAngleYaw) * 500.f;
                    spawn->oPosZ += coss(o->oMoveAngleYaw) * 500.f;
                    spawn->oAnimState = o->oBobombBlinkTimer;
                    o->oBobombBlinkTimer++;
                    o->oBobombBlinkTimer = o->oBobombBlinkTimer % 3;
                    switch (random_u16() % 3) {
                        case 0:
                            // normal spawn
                            break;
                        case 1:
                            // random offset
                            spawn->oMoveAngleYaw += (o->oAngleToMario - o->oBobombBuddyRole) * 50;
                            break;
                        case 2:
                            // random offset predictive
                            spawn->oMoveAngleYaw += (o->oAngleToMario - o->oBobombBuddyRole) * 50
                                                    + (random_u16() & 0x1fff) - 0x1000;
                            break;
                    }
                    spawn->oFaceAngleYaw = spawn->oMoveAngleYaw;
                }
            }
            o->oBobombBuddyRole = o->oAngleToMario;
            break;
        case 3:
            // lower mouth for dynamite, end this state after a bit
            if (find_nearest_TNT(o->oHiddenBlueCoinSwitch, &TNT) < 300.f) {
                o->oAction = 4;
                TNT->activeFlags = 0;
                cur_obj_play_sound_2(SOUND_OBJ_EEL);
            }
            cur_obj_init_animation(2);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x400);
            if (o->oTimer > (180 - o->oHealth * 20)) {
                if (cur_obj_check_if_at_animation_end()) {
                    o->oAction = 2;
                    o->oOpacity = (random_u16() & 0x7F) + 0x100 + o->oHealth * 0x60;
                }
            } else {
                if (o->header.gfx.unk38.animFrame > 49) {
                    o->header.gfx.unk38.animFrame -= 25;
                }
            }
            break;
        case 4:
#define DAMAGEFRAME 58
            // taken damage, do animation
            cur_obj_init_animation(3);
            if (!o->oTimer) {
                o->oHealth++;
                if (o->oHealth == 3) {
                    o->oAction = 5;
                    return;
                }
            }

            if ((o->oTimer > DAMAGEFRAME) && (o->oTimer < DAMAGEFRAME + 15)) {
                /*if (cur_obj_nearest_object_with_behavior(bhvTNT)) {
                    spawn = spawn_object(cur_obj_nearest_object_with_behavior(bhvTNT), MODEL_EXPLOSION,
                                         bhvExplosion);
                    cur_obj_nearest_object_with_behavior(bhvTNT)->activeFlags = 0;
                }*/
            }
            if (o->oTimer == DAMAGEFRAME) { // change with anim
                spawn = spawn_object(o->oHiddenBlueCoinSwitch, MODEL_EXPLOSION, bhvExplosion);
                spawn->oBehParams2ndByte = 10;
                spawn->oPosY += 200.f;
                cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_DYING);
            }
            if (cur_obj_check_if_at_animation_end()) {
                // wait for a multiple of 25 frames so its back in sync
                o->oAction = 2;
                o->oOpacity = (random_u16() & 0x7F) + 0x180 + o->oHealth * 0x60;
            }
            break;
        case 5:
            cur_obj_init_animation(4);
            if ((o->oTimer > DAMAGEFRAME) && (o->oTimer < DAMAGEFRAME + 15)) {
                if (cur_obj_nearest_object_with_behavior(bhvTNT)) {
                    spawn = spawn_object(cur_obj_nearest_object_with_behavior(bhvTNT), MODEL_EXPLOSION,
                                         bhvExplosion);
                    cur_obj_nearest_object_with_behavior(bhvTNT)->activeFlags = 0;
                }
            }
            if (o->oTimer == DAMAGEFRAME) { // change with anim
                spawn = spawn_object(o->oHiddenBlueCoinSwitch, MODEL_EXPLOSION, bhvExplosion);
                spawn->oBehParams2ndByte = 10;
                spawn->oPosY += 200.f;
            }
#define DISAPPEARTIMER 150
            if (!o->oSubAction) {
                if (gMarioState->pos[1] == gMarioState->floorHeight) {
                    if (set_mario_npc_dialog(2) == 2 && cutscene_object(CUTSCENE_STAR_SPAWN, o) == 1) {
                        o->oSubAction = 1;
                        o->oTimer = 0;
                        play_sound(SOUND_GENERAL_WATER_LEVEL_TRIG, gDefaultSoundArgs);
                    }
                }
            } else {
                if (o->oTimer > DISAPPEARTIMER) {
                    cur_obj_scale(o->header.gfx.scale[1] - 0.02f);
                }
                if (o->oTimer == DISAPPEARTIMER) {
                    cur_obj_play_sound_2(SOUND_OBJ_PIRANHA_PLANT_SHRINK);
                }
                if (o->oTimer > DISAPPEARTIMER + 49) {
                    gObjCutsceneDone = TRUE;
                    if (cutscene_object(CUTSCENE_STAR_SPAWN, o->oHiddenBlueCoinSwitch) == -1) {
                        set_mario_npc_dialog(0);
                        o->oAction = 6;
                    }
                }
            }
            break;
        case 6:
            endvinesanim = TRUE; // CuckyDev: Stop animating vines when dead
            spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f, gMarioState->pos[2]);
            play_sequence(SEQ_PLAYER_LEVEL, 0, 0);
            obj_mark_for_deletion(o);
            obj_mark_for_deletion(o->oHiddenBlueCoinSwitch);
            return;
            break;
    }
    for (i = 0; i < 5; i++) {
        if (dist_to_segment(gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2],
                            joinPosition[i][o->oAnimState][0], joinPosition[i][o->oAnimState][1],
                            joinPosition[i][o->oAnimState][2], joinPosition[i + 1][o->oAnimState][0],
                            joinPosition[i + 1][o->oAnimState][1],
                            joinPosition[i + 1][o->oAnimState][2])
            < 150.f) {
            if (m->action != ACT_LAVA_BOOST) {
                drop_and_set_mario_action(m, ACT_LAVA_BOOST, 0);
                gMarioState->hurtCounter = 12;
            }
        }
    }
    if (o->oAction) {
        vec3f_copy(&o->oHiddenBlueCoinSwitch->oPosX, joinPosition[5][o->oAnimState]);
        o->oBobombBuddyCannonStatus++; // timer for music
    }
}

void piranhavine(void) {
    switch (o->oAction) {
        case 0:
            o->oAction = 1;
            endvinesanim = FALSE;
            break;
        case 1:
            if (!endvinesanim)
                break;
            o->oAction = 2;
            o->oOpacity = o->header.gfx.unk38.animFrame;
            // Fallthrough
        case 2:
            o->header.gfx.unk38.animFrame = o->oOpacity; // CuckyDev: Stop animating vines when dead
            break;
    }
}

s32 cur_obj_wait_then_blink2(s32 timeUntilBlinking, s32 numBlinks) {
    s32 done = FALSE;
    s32 timeBlinking;

    if (o->oOpacity >= timeUntilBlinking) {
        if ((timeBlinking = o->oOpacity - timeUntilBlinking) % 2 != 0) {
            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            if (timeBlinking / 2 > numBlinks) {
                done = TRUE;
            }
        } else {
            o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    }

    return done;
}
void blinkAndDeactivate(void) {
    o->oOpacity++;
    if (o->oInteractStatus || (o == gMarioState->heldObj)) {
        o->oOpacity = 0;
    }
    if (cur_obj_wait_then_blink2(340, 10)) {
        o->activeFlags = 0;
    }
}

void lightsbhv(void) {
    o->oOpacity++;
    switch (o->oAction) {
        case 0:
            o->oAngleVelYaw += 0x040;
            if (o->oAngleVelYaw > 0x200) {
                o->oAction++;
            }
            break;
        case 1:
            if ((o->oTimer > 85) && (!(o->oOpacity % 25))) {
                o->oAction++;
            }
            break;
        case 2:
            o->oAngleVelYaw -= 0x040;
            if (o->oAngleVelYaw < -0x200) {
                o->oAction++;
            }
            break;
        case 3:
            if ((o->oTimer > 85) && (!(o->oOpacity % 25))) {
                o->oAction = 0;
            }
            break;
    }
    o->oFaceAngleYaw += o->oAngleVelYaw;
}

#define MOD 250.f;
void updownplatformcode(void) {
    o->oVelY = sins((o->oOpacity * 0x100 + o->oBehParams2ndByte * 0x200) & 0xffff) * MOD;
    o->oOpacity++;
    o->oPosY = o->oHomeY + o->oVelY;
    load_object_collision_model();
}

#define waittimer 100
#define downwait 50
#define MAXUP 75
void hammercode(void) {
    switch (o->oAction) {
        case 0:
            o->oPosY += 10.f;
            if (o->oTimer >= MAXUP) {
                o->oAction++;
            }
            break;
        case 1:
            if (o->oTimer >= waittimer) {
                o->oAction++;
            }
            break;
        case 2:
            o->oVelY -= 4.f;
            o->oPosY += o->oVelY;
            if (o->oPosY < (o->oHomeY + 5.f)) {
                o->oAction++;
                o->oPosY = o->oHomeY + 5.f;
                cur_obj_play_sound_2(SOUND_GENERAL_BIG_POUND);
                cur_obj_shake_screen(SHAKE_POS_SMALL);
                o->oVelY = 0.f;
            }
            break;
        case 3:
            if (o->oTimer >= (downwait + o->oOpacity)) {
                o->oAction = 0;
                o->oOpacity = random_u16() & 0x1f;
            }
            break;
    }
    o->oCollisionDistance = 5000.f;
    load_object_collision_model();
}

#define podoboowait 20
void podoboo(void) {
    struct Object *spawn;
    switch (o->oAction) {
        case 0:
            if (o->oTimer >= podoboowait) {
                o->oAction++;
                o->oVelY = 80.f;
                o->oFaceAngleYaw = 0;
            }
            break;
        case 1:
            o->oVelY -= 3.f;
            o->oPosY += o->oVelY;
            if (o->oPosY < (o->oHomeY - 130.f)) {
                spawn = spawn_object(o, 0x47, bhvLavaSplash);
                spawn->oPosY += 90.f;
                o->oAction = 0;
                o->oPosY = (o->oHomeY - 130.f);
                cur_obj_play_sound_2(SOUND_OBJ_DIVING_IN_WATER);
                o->oVelY = 0.f;
            }
            break;
    }
    o->oFaceAngleRoll =
        approach_s16_symmetric(o->oFaceAngleRoll, (o->oVelY < 5.f) ? 0x8000 : 0x0000, 0x800);
    o->oFaceAngleYaw = atan2s(o->oPosZ - gLakituState.pos[2], o->oPosX - gLakituState.pos[0]);
}

struct ObjectHitbox nipperhitbox = {
    /* interactType:      */ INTERACT_MR_BLIZZARD,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 2,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 100,
    /* height:            */ 80,
    /* hurtboxRadius:     */ 50,
    /* hurtboxHeight:     */ 60,
};
u8 sNipperWalkAttackHandlers[] = {
    /* ATTACK_PUNCH:                 */ ATTACK_HANDLER_KNOCKBACK,
    /* ATTACK_KICK_OR_TRIP:          */ ATTACK_HANDLER_KNOCKBACK,
    /* ATTACK_FROM_ABOVE:            */ ATTACK_HANDLER_NOP,
    /* ATTACK_GROUND_POUND_OR_TWIRL: */ ATTACK_HANDLER_NOP,
    /* ATTACK_FAST_ATTACK:           */ ATTACK_HANDLER_KNOCKBACK,
    /* ATTACK_FROM_BELOW:            */ ATTACK_HANDLER_KNOCKBACK,
};

void checkNipperJump() {
    if (o->header.gfx.unk38.animFrame == 1) {
        o->oVelY = 40.f;
        o->oForwardVel = 32.f;
        cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_BITE);
    }
}

void nipperenemyx(void) {
    int action = o->oAction;
    if (obj_handle_attacks(&nipperhitbox, 95, sNipperWalkAttackHandlers)) {
        // When attacked by mario, lessen the knockback
        o->oMoveFlags = 0; // weird flex but okay
        // Don't allow mario to punch the spiny two frames in a row?
        o->oInteractType = INTERACT_MR_BLIZZARD;
    } else {
        o->oInteractType = INTERACT_UNKNOWN_08;
    }
    if (o->oAction == 95) {
        o->oAction = action;
    }
    o->oInteractStatus = 0;
    switch (o->oAction) {
        case 0:
            // walk in circles
            cur_obj_init_animation(2);

            if (o->header.gfx.unk38.animFrame > 33) {
                o->header.gfx.unk38.animFrame = 16;
            }
            if (o->header.gfx.unk38.animFrame == 25) {
                o->oVelY = 30.f;
                o->oForwardVel = 14.f;
                cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_WALK);
            }
            if ((o->header.gfx.unk38.animFrame > 24) && (o->header.gfx.unk38.animFrame < 28)) {
                o->oMoveAngleYaw =
                    approach_s16_symmetric(o->oMoveAngleYaw, cur_obj_angle_to_home(), 0x500);
            }

            if (cur_obj_lateral_dist_from_mario_to_home() < 3200.f) {
                o->oAction = 1;
            }
            break;
        case 1:
            // attack mario
            cur_obj_init_animation(2);
            if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) > 0x1000) {
                if ((o->header.gfx.unk38.animFrame > 33) || (o->header.gfx.unk38.animFrame < 16)) {
                    o->header.gfx.unk38.animFrame = 16;
                }
                if (o->header.gfx.unk38.animFrame == 25) {
                    o->oVelY = 30.f;
                    cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_WALK);
                }
                if ((o->header.gfx.unk38.animFrame > 24) && (o->header.gfx.unk38.animFrame < 28)) {
                    o->oMoveAngleYaw =
                        approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x500);
                }
            } else {
                checkNipperJump();
                if (cur_obj_lateral_dist_from_mario_to_home() > 4400.f) {
                    o->oAction = 0;
                }
                if (o->header.gfx.unk38.animFrame < 5) {
                    o->oMoveAngleYaw =
                        approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x400);
                }
            }
            if (lateral_dist_between_objects(o, gMarioState->marioObj) < 400.f) {
                if (gMarioState->pos[1] > o->oPosY + 200.f) {
                    o->oAction = 2;
                }
            }
            break;
        case 2:
            // attack upwards
            cur_obj_init_animation(3);
            if (o->header.gfx.unk38.animFrame == 4) {
                o->oVelY = 48.f;
                o->oForwardVel = 0.f;
                cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_BITE);
            }
            if (o->oTimer == 50) {
                o->oAction = 1;
            }
            break;
        case 3:
        case 100:
        case 101:
            // fkn die
            if (!o->oTimer) {
                spawn_mist_particles_with_sound(SOUND_OBJ_DYING_ENEMY1);
            }
            cur_obj_init_animation(2);
            o->oNumLootCoins = 2;
            o->header.gfx.unk38.animFrame = 1;
            if (o->oTimer == 15) {
                spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
                obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);
                obj_mark_for_deletion(o);
            }
            break;
    }
    o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 0.f, 0.01f);
    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.f);
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
}

void bhvbluespawenrosadhbgiuogdsiuzfghdsaiuzofgo(void) {
    if (gMarioState->action == ACT_GROUND_POUND_LAND) {
        if (o->oDistanceToMario < 200.f) {
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f,
                                   gMarioState->pos[2]);
            obj_mark_for_deletion(o);
        }
    }
}

void pumpking(void) {
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_ground_pounding_platform()) {
                o->oAction = 1;
            }
            break;
        case 1:
            o->header.gfx.scale[1] -= 0.08f;
            o->header.gfx.scale[0] = sqrtf(1.f / o->header.gfx.scale[1]);
            o->header.gfx.scale[2] = o->header.gfx.scale[0];
            if (o->oTimer > 8) {
                spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
                spawn_mist_particles_variable(0, 0, 46.f);
                spawn_triangle_break_particles(30, 138, 3.0f, 4);
                mark_obj_for_deletion(o);
            }
            break;
    }
    load_object_collision_model();
}

void eatingnipper(void) {
    if (cur_obj_nearest_object_with_behavior(bhvPumpking)) {
        cur_obj_init_animation(1);
    } else {
        cur_obj_init_animation(4);
        if (o->header.gfx.unk38.animFrame >= 129) {
            o->header.gfx.unk38.animFrame = 69;
        }
    }
}

void happyholecode(void) {
    struct Object *obj;
    struct ObjectNode *listHead;
    int i, j;
    if (!o->oTimer) {
        for (i = 0; i < mirrorcount; i++) {
            MirrorArray[i] = 0;
        }
    }
    for (i = 0; i < mirrorcount; i++) {
        if (MirrorArray[i]) {
            MirrorArray[i]->activeFlags = 0;
        }
        MirrorArray[i] = 0;
    }
    i = 0;
    for (j = 1; j < 16; j++) {
        listHead = &gObjectLists[j];
        obj = (struct Object *) listHead->next;
        if (obj) {
            while (obj != (struct Object *) listHead) {
                if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                    if (obj->header.gfx.sharedChild) {
                        if (obj->oBehParams != 0xDEADBEEF) {
                            if ((obj->oPosZ > -5000.f) && (obj->oPosZ < 5000.f)
                                && (obj->oPosX > -5000.f) && (obj->oPosX < 5000.f)
                                && (obj->oPosY >= -433.f)) {
                                MirrorArray[i] = spawn_object(obj, 0, bhvMirroredObject);
                                MirrorArray[i]->oPosZ = obj->oPosZ;
                                MirrorArray[i]->oPosY = obj->oPosY  - 12000.f;
                                MirrorArray[i]->oPosX = obj->oPosX;

                                MirrorArray[i]->header.gfx.sharedChild = obj->header.gfx.sharedChild;
                                MirrorArray[i]->header.gfx.scale[0] = obj->header.gfx.scale[0];
                                MirrorArray[i]->header.gfx.scale[1] = obj->header.gfx.scale[1];
                                MirrorArray[i]->header.gfx.scale[2] = obj->header.gfx.scale[2];
                                MirrorArray[i]->header.gfx.node.flags = obj->header.gfx.node.flags;
                                MirrorArray[i]->oFaceAngleYaw = obj->oFaceAngleYaw;
                                MirrorArray[i]->oFaceAngleRoll = obj->oFaceAngleRoll;
                                MirrorArray[i]->oAnimState = obj->oAnimState;
                                MirrorArray[i]->header.gfx.unk38.animFrame =
                                    obj->header.gfx.unk38.animFrame;
                                MirrorArray[i]->header.gfx.unk38.animYTrans =
                                    obj->header.gfx.unk38.animYTrans;
                                MirrorArray[i]->header.gfx.unk38.curAnim =
                                    obj->header.gfx.unk38.curAnim;
                                MirrorArray[i]->header.gfx.unk38.animTimer =
                                    obj->header.gfx.unk38.animTimer;
                                MirrorArray[i]->header.gfx.unk38.animFrameAccelAssist =
                                    obj->header.gfx.unk38.animFrameAccelAssist;

                                MirrorArray[i]->oBehParams = 0xDEADBEEF;
                                i++;
                                if (i == mirrorcount) {
                                    return;
                                }
                            }
                        }
                    }
                }
                obj = (struct Object *) obj->header.next;
            }
        }
    }
}
