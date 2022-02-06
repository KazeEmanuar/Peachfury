// stuff to make programming easier
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
#include "engine/surface_collision.h"
#include "game/interaction.h"
#include "engine/behavior_script.h"
#include "game/object_list_processor.h"
#include "behavior_data.h"
#include "../../enhancements/puppycam.h"
#include "game/rendering_graph_node.h"
#include "game/save_file.h"
#include "audio/data.h"
#include <PR/gbi.h>
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject
#define objAnimID (o->header.gfx.unk38.animID)
#define objAnimFrame (o->header.gfx.unk38.animFrame)
#define marioObject gMarioState->marioObj
#include "levels/hmc/header.h"
#include "audio/external.h"
#include "game/camera.h"

#define m gMarioState
#define o gCurrentObject

#define m gMarioState
#define o gCurrentObject
#include "game/OPT_FOR_SIZE.h"

#define latdist 120.f
#define vertdist 80.f
struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior);
void bounceyflower(void) {
    o->oFaceAngleYaw += 0x800;
    if ((lateral_dist_between_objects(o, gMarioObject) < latdist)
        && (absf((gMarioState->pos[1] - o->oPosY)) < vertdist)) {

        gMarioState->faceAngle[0] = 0;
        play_sound(SOUND_MARIO_TWIRL_BOUNCE, m->marioObj->header.gfx.cameraToObject);
        drop_and_set_mario_action(m, ACT_TWIRLING, 0);
        bounce_off_object(m, o, 100.0f - o->oBehParams2ndByte);
        gMarioState->vel[1] = 100.f - o->oBehParams2ndByte;
    }
};

void screwcode(void) {
    if (cur_obj_is_mario_on_platform()) {
        if (coss(atan2s(gMarioObject->oPosX - o->oPosX, gMarioObject->oPosZ - o->oPosZ)
                 + o->oFaceAngleYaw)
            > 0) {
            o->oAngleVelRoll -= 0x0020;
        } else {
            o->oAngleVelRoll += 0x0020;
        }
        if (o->oAngleVelRoll > 0x400) {
            o->oAngleVelRoll = 0x400;
        }
        if ((s16) o->oAngleVelRoll < -0x400) {
            o->oAngleVelRoll = -0x400;
        }
    }
    o->oAngleVelRoll = approach_s16_symmetric(o->oAngleVelRoll, 0, 0x08);
    o->oFaceAngleRoll += o->oAngleVelRoll;
    o->oForwardVel = o->oAngleVelRoll * 0.007f;
    cur_obj_move_xz_using_fvel_and_yaw();
    resolve_and_return_wall_collisions(&o->oPosX, 0.0f, 200.0f);
    load_object_collision_model();
}

// int scavengerhuntID = 0;

void spawn_default_star(f32 sp20, f32 sp24, f32 sp28);
void scavengersign(void) {
    struct Object *sign;
    if (!o->oAction) {
        if ((gMarioState->action == ACT_GROUND_POUND_LAND) && (o->oDistanceToMario < 1000.f)) {
            if (o->oBehParams & 0x00ff) {
                spawn_default_star(gMarioState->pos[0], gMarioState->pos[1] + 200.f,
                                   gMarioState->pos[2]);
                mark_obj_for_deletion(o);
            } else {
                sign = spawn_object(o, MODEL_WOODEN_SIGNPOST, bhvMessagePanel);
                spawn_object(o, 0, bhvMistCircParticleSpawner);
                sign->oBehParams2ndByte = o->oBehParams2ndByte;
                sign->oMoveAngleYaw =
                    atan2s(o->oPosZ - gMarioObject->oPosZ, o->oPosX - gMarioObject->oPosX) + 0x8000;
                sign->oFaceAngleYaw = sign->oMoveAngleYaw;
                o->oAction = 1;
                sign->oPosY -= 200.f;
                o->oHiddenBlueCoinSwitch = sign;
                play_sound(SOUND_GENERAL2_RIGHT_ANSWER, gDefaultSoundArgs);
            }
        }
    } else {
        if (o->oHiddenBlueCoinSwitch) {
            if (o->oTimer < 10) {
                o->oHiddenBlueCoinSwitch->oPosY += 20.f;
            }
        }
    }
}

void cloudbox(void) {
    if (!o->oTimer) {
        spawn_mist_particles_variable(0, -40.f, 46.0f);
    }
    if (o->oTimer < 15) {
        o->header.gfx.scale[0] += 0.097;
        o->header.gfx.scale[1] = o->header.gfx.scale[0];
        o->header.gfx.scale[2] = o->header.gfx.scale[0];
        o->oFaceAngleYaw += 0x0800;
    } else {
        if (cur_obj_wait_then_blink(360, 20) || cur_obj_is_mario_ground_pounding_platform()) {
            spawn_mist_particles_variable(0, -40.f, 46.0f);
            mark_obj_for_deletion(o);
        } else {
            load_object_collision_model();
        }
    }
}

#define gapclose 0.1f
void cloudfollowing(void) {
    float x, y, z;
    if (gMarioObject->oHiddenBlueCoinSwitch == o) {
        y = gMarioStates->pos[1] + 150.f;
        x = gMarioStates->pos[0] + sins(gMarioStates->faceAngle[1]) * 50.f;
        z = gMarioStates->pos[2] + coss(gMarioStates->faceAngle[1]) * 50.f;
    } else if (gMarioObject->oHiddenBlueCoinSwitch
               && (gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch == o)) {
        y = gMarioObject->oHiddenBlueCoinSwitch->oPosY + 10.f;
        x = gMarioObject->oHiddenBlueCoinSwitch->oPosX
            + sins(gMarioObject->oHiddenBlueCoinSwitch->oMoveAngleYaw) * 50.f;
        z = gMarioObject->oHiddenBlueCoinSwitch->oPosZ
            + coss(gMarioObject->oHiddenBlueCoinSwitch->oMoveAngleYaw) * 50.f;
    } else if (gMarioObject->oHiddenBlueCoinSwitch
               && (gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch)) {
        y = gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch->oPosY + 10.f;
        x = gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch->oPosX
            + sins(gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch->oMoveAngleYaw) * 50.f;
        z = gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch->oPosZ
            + coss(gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch->oMoveAngleYaw) * 50.f;
    }
    o->oPosX = o->oPosX + (x - o->oPosX) * gapclose;
    o->oPosY = o->oPosY + (y - o->oPosY) * gapclose;
    o->oPosZ = o->oPosZ + (z - o->oPosZ) * gapclose;
}
extern u8 iceState;
void icequad_funciton(void) {
    spawn_object(o, MODEL_SMOKE, bhvAzote);
    o->oCollisionDistance = 600.f;
    if (iceState) {
        iceState = 0;
        cur_obj_scale(approach_f32_symmetric(o->header.gfx.scale[0], 1.f, .1f));
    } else {
        cur_obj_scale(approach_f32_symmetric(o->header.gfx.scale[0], 0.f, .1f));
        spawn_object(o, MODEL_SMOKE, bhvAzote);
        if (o->header.gfx.scale[0] < .1f) {
            mark_obj_for_deletion(o);
        }
    }
}

void icehexhoriz(void) {
    if (gMarioState->action == ACT_WALL_SLIDE) {
        o->oPosY = gMarioState->pos[1] + 35.f;
        cur_obj_scale(approach_f32_symmetric(o->header.gfx.scale[0], 1.f, .2f));
    } else {
        cur_obj_scale(approach_f32_symmetric(o->header.gfx.scale[0], 0.f, .1f));
        if (o->header.gfx.scale[0] < .1f) {
            mark_obj_for_deletion(o);
        }
    }
}
extern f32 newcam_pos[3];
void bhv_azote(void) {
    o->oOpacity = 200;
    o->oFaceAngleRoll = 0x4000;
    o->oPosX += (random_f32_around_zero(250.f));
    o->oPosY += (random_float() * 50.0f);
    o->oPosZ += (random_f32_around_zero(250.f));
    cur_obj_scale(1.2f);
    o->oFaceAngleYaw = atan2s(-o->oPosZ + newcam_pos[2], -o->oPosX + newcam_pos[0]);
}

void bhv_azote2(void) {
    o->oPosX += o->oVelX;
    o->oPosY += o->oVelY;
    o->oPosZ += o->oVelZ;
    o->oFaceAngleYaw = atan2s(-o->oPosZ + newcam_pos[2], -o->oPosX + newcam_pos[0]);

    if (o->oTimer > 10) {
        obj_mark_for_deletion(o);
    }
}
extern u8 goombacount;
void goombaSpawnerCode(void) {
    struct Object *obj;
    goombacount = count_objects_with_behavior(bhvGoomba) + count_objects_with_behavior(bhvGoombaBunny);
    switch (o->oAction) {
        case 0:
            if (!o->oTimer & !goombacount) {
                obj = spawn_object(o, MODEL_STAR, bhvStar);
                o->oAction++;
                obj->oBehParams = o->oBehParams;
            } else {
                if (!goombacount) {
                    spawn_default_star(o->oPosX, o->oPosY, o->oPosZ);
                    o->oAction++;
                }
            }
            break;
        case 1:

            break;
    }
}
f32 vec3f_dist(Vec3f a, Vec3f b) {
    f32 x, y, z;
    x = a[0] - b[0];
    y = a[1] - b[1];
    z = a[2] - b[2];
    return (sqrtf(x * x + y * y + z * z));
}

u8 talkToMario(int dialogID, int actionArg) {
    return cur_obj_update_dialog_with_cutscene(actionArg, 1, CUTSCENE_DIALOG, dialogID);
}

u8 talkToMarioNoRotation(int dialogID, int actionArg) {
    return cur_obj_update_dialog_with_cutscene(actionArg, 0, CUTSCENE_DIALOG, dialogID);
}

u8 askMario(int dialogID, u8 rotate, int actionArg) {
    return cur_obj_update_dialog_with_cutscene(actionArg, rotate, CUTSCENE_RACE_DIALOG, dialogID);
}

extern void goombaking_walk(void);
#define king_sleep 0
#define king_wakeuptalk 1
#define king_idle 2
#define king_jump 3
#define king_confused 4
#define king_damaged 5
#define king_tutorial 6 // if you jump of his crown without damaging him give tutorial
void goombakingcode(void) {
    if (o->oHiddenBlueCoinSwitch) {
        vec3f_copy(&o->oHiddenBlueCoinSwitch->oVelX, &o->oHiddenBlueCoinSwitch->oPosX);
        o->oHiddenBlueCoinSwitch->oAngleVelYaw = o->oFaceAngleYaw;
        obj_copy_pos_and_angle(o->oHiddenBlueCoinSwitch, gCurrentObject);
        o->oHiddenBlueCoinSwitch->oPosY += 400.f * o->header.gfx.scale[1] / 7.0f;
        o->oHiddenBlueCoinSwitch->oAngleVelYaw =
            o->oHiddenBlueCoinSwitch->oFaceAngleYaw - o->oHiddenBlueCoinSwitch->oAngleVelYaw;
        o->oHiddenBlueCoinSwitch->oVelX =
            o->oHiddenBlueCoinSwitch->oPosX - o->oHiddenBlueCoinSwitch->oVelX;
        o->oHiddenBlueCoinSwitch->oVelY =
            o->oHiddenBlueCoinSwitch->oPosY - o->oHiddenBlueCoinSwitch->oVelY;
        o->oHiddenBlueCoinSwitch->oVelZ =
            o->oHiddenBlueCoinSwitch->oPosZ - o->oHiddenBlueCoinSwitch->oVelZ;
    }
    if (o->oPosY < -4.f) {
        o->oPosY = find_floor_height(o->oPosX, o->oPosY + 500.0f, o->oPosZ);
    }
    // keep him contained in bounds
    cur_obj_update_floor_and_walls();
    switch (o->oAction) {
        case 0:
            cur_obj_scale(7);
            o->oHealth = 3;
            o->oAnimState = 1;
            cur_obj_init_animation(1);
            if (o->header.gfx.unk38.animFrame >= 80) {
                if (o->oDistanceToMario < 750.f) {
                    o->oAction++;
                    o->oAnimState = 0;
                } else {
                    o->header.gfx.unk38.animFrame = 0;
                    o->oHomeX = -291.f;
                    o->oHomeY = 4.f;
                    o->oHomeZ = -2074.f;
                }
            }
            break;
        case 1:
            if (o->header.gfx.unk38.animFrame == 85 || o->header.gfx.unk38.animFrame == 86) {
                o->oAnimState = 1;
            } else if (o->header.gfx.unk38.animFrame == 94 || o->header.gfx.unk38.animFrame == 95) {
                o->oAnimState = 1;
            } else if (o->header.gfx.unk38.animFrame == 99 || o->header.gfx.unk38.animFrame == 100) {
                o->oAnimState = 1;
            } else {
                o->oAnimState = 0;
            }
            if (cur_obj_check_if_at_animation_end()) {
                cur_obj_init_animation(0);
            }
            if (o->header.gfx.unk38.animID == 0) {
                curr_obj_random_blink(&o->oBobombBlinkTimer);
                if (talkToMario(157, 2)) {
                    o->oAction++;
                    o->oInteractType = INTERACT_BOUNCE_TOP2;
                    o->oHiddenBlueCoinSwitch = spawn_object(o, 0, bhvGoombaKingCrown);
                }
            }
            break;
        case 2:
            o->oAnimState = 0;
            cur_obj_init_animation(0);
            goombaking_walk();
            if (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x200) {
                if (o->oDistanceToMario < 600.0f) {
                    o->oAction = king_jump;
                }
            }
            if (gMarioState->marioObj->platform == o->oHiddenBlueCoinSwitch) {
                o->oAction = king_confused;
            }
            if (gMarioObject->platform == o->oHiddenBlueCoinSwitch) {
                if (gMarioStates[0].action == ACT_GROUND_POUND_LAND) {
                    o->oAction = king_damaged;
                    o->oHealth--;
                    o->oIntangibleTimer = -1;
                }
            }
            break;
        case king_jump:
            if (!o->oTimer) {
                cur_obj_init_animation(2);
                o->oVelY = 60.f;
                o->oForwardVel = 35.f;
                cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB_JUMP);
            } else {
                if (!o->oSubAction) {
                    if (o->oTimer == 30) {
                        cur_obj_play_sound_2(SOUND_OBJ_THWOMP);
                    }
                    if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
                        o->oSubAction = 1;
                        o->oTimer = 1;
                    }
                } else {
                    o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.0f, 2.0f);
                    if (o->oTimer == 40) {
                        o->oAction = 2;
                    }
                }
            }
            if (gMarioObject->platform == o->oHiddenBlueCoinSwitch) {
                if (gMarioStates[0].action == ACT_GROUND_POUND_LAND) {
                    o->oAction = king_damaged;
                    o->oHealth--;
                    o->oIntangibleTimer = -1;
                }
            }
            cur_obj_move_standard(-78);
            break;
        case king_confused:
            cur_obj_init_animation(3);
            o->oForwardVel = approach_f32(o->oForwardVel, 0.0f, 2.0f, 2.0f);
            if (gMarioObject->platform == o->oHiddenBlueCoinSwitch) {
                if (gMarioStates[0].action == ACT_GROUND_POUND_LAND) {
                    o->oAction = king_damaged;
                    o->oHealth--;
                    o->oIntangibleTimer = -1;
                }
            } else {
                if (o->oTimer > 25) {
                    if ((!o->oOpacity) && (o->oHealth == 3)) {
                        o->oAction = 6;
                    } else {
                        o->oAction = 2;
                    }
                }
            }
            if ((gMarioState->marioObj->platform == o->oHiddenBlueCoinSwitch)
                || (gMarioState->pos[1] != gMarioState->floorHeight)) {
                o->oTimer = 0;
            }
            cur_obj_move_standard(-78);
            break;
        case 5:
            o->header.gfx.unk38.animFrame--;
            if (o->oTimer < 10) {
                o->header.gfx.scale[1] -= 0.35f;
            }
            if ((o->oTimer < 20) && (o->oTimer > 9)) {
                o->header.gfx.scale[1] += 0.35f;
            }
            if (o->oTimer == 21) {
                //    if (o->oHealth > 0) {
                gMarioState->action = ACT_TRIPLE_JUMP;
                gMarioState->forwardVel = -40.f;
                gMarioState->vel[1] = 65.f;
                //   }
            }
            if (o->oTimer > 22) {
                if (o->oHealth > 0) {
                    o->oAction = 2;
                    o->oIntangibleTimer = 0;
                } else {
                    o->oAction = 7;
                    if (o->oHiddenBlueCoinSwitch) {
                        mark_obj_for_deletion(o->oHiddenBlueCoinSwitch);
                        o->oHiddenBlueCoinSwitch = 0;
                    }
                    /*
                    spawn_default_star(-1303.f, 689.f, 2078.f);
                    spawn_mist_particles_variable(0, -40.f, 46.0f);
                    mark_obj_for_deletion(o);
                    mark_obj_for_deletion(o->oHiddenBlueCoinSwitch);
                    */
                }
            }
            o->header.gfx.scale[0] = sqrtf(329 / o->header.gfx.scale[1]);
            o->header.gfx.scale[2] = o->header.gfx.scale[0];
            break;
        case 6:
            cur_obj_init_animation(0);
            if (talkToMario(156, 2)) {
                o->oAction = 2;
                o->oOpacity = 1;
            }
            break;
        case 7:
            cur_obj_init_animation(4);
            if (o->header.gfx.unk38.animFrame > 48) {
                o->header.gfx.unk38.animFrame = 10;
            }
            if (talkToMario(155, 2)) {
                o->oAction++;
            }
            break;
        case 8:
            cur_obj_scale(7 - o->oTimer * .6f);
            if (o->oTimer == 10) {
                spawn_default_star(-1303.f, 689.f, 2078.f);
                spawn_mist_particles_variable(0, -40.f, 46.0f);
                mark_obj_for_deletion(o);
            }
            break;
    }
}

void submarinecode(void) {
    switch (o->oAction) {
        case 0:
            o->oPosY += sins(o->oTimer * 0x400) * 1.0f;

            if (save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 0x18) >= 1) {
                o->oAnimState = 0;
                if (cur_obj_is_mario_on_platform()) {
                    if (SURFACETYPE(gMarioState->floor) == 0xff) {
                        o->oAction++;
                        m->action = ACT_WAITING_FOR_DIALOG;
                    }
                }
            } else {
                o->oAnimState = 1;
            }
            break;
        case 1:
            if (o->oTimer > 20) {
                o->oVelY -= 0.5f;
                o->oPosY += o->oVelY;
                gMarioState->pos[1] -= o->oVelY;
                // gMarioState->numKeys = ( - 200)/2;
                m->action = ACT_DISAPPEARED;
                m->actionTimer = 3; // CuckyDev: Disappear instead of warp
            } else {
                o->oHomeX = o->oPosX + sins((o->oFaceAngleYaw + 0xC000) & 0xffff) * 80.f;
                o->oHomeZ = o->oPosZ + coss((o->oFaceAngleYaw + 0xc000) & 0xffff) * 80.f;
                gMarioState->pos[0] = approach_f32_symmetric(gMarioState->pos[0], o->oHomeX, 3.0f);
                gMarioState->pos[2] = approach_f32_symmetric(gMarioState->pos[2], o->oHomeZ, 3.0f);
                gMarioState->numKeys = (-(o->oTimer * o->oTimer / 2)) / 2;
            }
            o->header.gfx.unk38.animYTrans = m->unkB0;
            if (o->oTimer == 40) {
                m->usedObj = o;
                level_trigger_warp(m, 0x20);
            }
            break;
    }
    load_object_collision_model();
}

struct Object *findOtherDoor(int ID) {
    struct ObjectNode *listHead;
    struct Object *obj;
    listHead = &gObjectLists[OBJ_LIST_SURFACE];
    obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == o->behavior) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && obj != o) {
                if ((obj->oBehParams >> 0x18) == ID) {
                    return obj;
                }
            }
        }
        obj = (struct Object *) obj->header.next;
    }
    return 0x80000000;
}

extern u8 openDoors;

extern Gfx wf_dl_domeinside1_mesh[];
extern Gfx wf_dl_domeinside1_Alpha_mesh[];
extern Gfx wf_dl_domeinside1_SOLID_mesh[];
extern Gfx wf_dl_domeinside1_SOLIDLIGHTS_mesh[];

extern Gfx wf_dl_domeinside2_mesh[];
extern Gfx wf_dl_domeinside2_Alpha_mesh[];
extern Gfx wf_dl_domeinside2_SOLID_mesh[];
extern Gfx wf_dl_domeinside2_SOLIDLIGHTS_mesh[];

extern Gfx wf_dl_domeinside3_mesh[];
extern Gfx wf_dl_domeinside3_SOLID_mesh[];

extern Gfx wf_dl_STAR4domeinside_mesh[];
extern Gfx wf_dl_STAR4domeinside_solid_mesh[];

extern Gfx wf_dl_REDCOINDOMEinside_mesh[];
extern Gfx wf_dl_REDCOINDOMEinside_ALPHA_mesh[];
extern Gfx wf_dl_REDCOINDOMEinside_solid2_mesh[];
// bparam1= 0,1 for dome1
// bparam1= 2,3 for dome2
// bparam1= 5,6 for dome3
// bparam1= 4 for star4dome
// bparam1= 7 for redcoindome
Gfx DLData[] = {
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

Gfx DLBackups[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

u8 renderedDome = 255;
void renderDome(int ID) {
    Gfx *a = segmented_to_virtual(wf_dl_domeinside1cpucull_mesh);
    Gfx *b = segmented_to_virtual(wf_dl_domeinside1_Alphacpucull_mesh);
    Gfx *c = segmented_to_virtual(wf_dl_domeinside1_SOLIDcpucull_mesh);
    Gfx *e = segmented_to_virtual(wf_dl_domeinside1_SOLIDLIGHTScpucull_mesh);
    Gfx *f = segmented_to_virtual(wf_dl_domeinside2cpucull_mesh);
    Gfx *g = segmented_to_virtual(wf_dl_domeinside2_Alphacpucull_mesh);
    Gfx *h = segmented_to_virtual(wf_dl_domeinside2_SOLIDcpucull_mesh);
    Gfx *i = segmented_to_virtual(wf_dl_domeinside2_SOLIDLIGHTScpucull_mesh);
    Gfx *j = segmented_to_virtual(wf_dl_domeinside3cpucull_mesh);
    Gfx *k = segmented_to_virtual(wf_dl_domeinside3_SOLIDcpucull_mesh);
    Gfx *l = segmented_to_virtual(wf_dl_STAR4domeinsidecpucull_mesh);
    Gfx *x = segmented_to_virtual(wf_dl_STAR4domeinside_solidcpucull_mesh);
    Gfx *n = segmented_to_virtual(wf_dl_REDCOINDOMEinsidecpucull_mesh);
    Gfx *y = segmented_to_virtual(wf_dl_REDCOINDOMEinside_ALPHAcpucull_mesh);
    Gfx *p = segmented_to_virtual(wf_dl_REDCOINDOMEinside_solid2cpucull_mesh);
    if (!DLBackups[0].words.w0) {
        DLBackups[0] = *a;
        DLBackups[1] = *b;
        DLBackups[2] = *c;
        DLBackups[3] = *e;
        DLBackups[4] = *f;
        DLBackups[5] = *g;
        DLBackups[6] = *h;
        DLBackups[7] = *i;
        DLBackups[8] = *j;
        DLBackups[9] = *k;
        DLBackups[10] = *l;
        DLBackups[11] = *x;
        DLBackups[12] = *n;
        DLBackups[13] = *y;
        DLBackups[14] = *p;
    }
    renderedDome = ID;
    a[0] = DLData[1];
    b[0] = DLData[1];
    c[0] = DLData[1];
    e[0] = DLData[1];
    f[0] = DLData[1];
    g[0] = DLData[1];
    h[0] = DLData[1];
    i[0] = DLData[1];
    j[0] = DLData[1];
    l[0] = DLData[1];
    x[0] = DLData[1];
    n[0] = DLData[1];
    y[0] = DLData[1];
    p[0] = DLData[1];
    switch (ID) {
        case 0:
        case 1:
            a[0] = DLBackups[0];
            b[0] = DLBackups[1];
            c[0] = DLBackups[2];
            e[0] = DLBackups[3];

            break;
        case 2:
        case 3:
            f[0] = DLBackups[4];
            g[0] = DLBackups[5];
            h[0] = DLBackups[6];
            i[0] = DLBackups[7];
            break;
        case 4:
            l[0] = DLBackups[10];
            x[0] = DLBackups[11];
            break;
        case 5:
        case 6:
            j[0] = DLBackups[8];
            k[0] = DLBackups[9];
            break;
        case 7:
            n[0] = DLBackups[12];
            y[0] = DLBackups[13];
            p[0] = DLBackups[14];
            break;
        default:
            break;
    }
}
void waterplane(void) {
    if (absf(gMarioState->pos[1] - o->oPosY) < 100.f) {
        if (lateral_dist_between_objects(o, gMarioState->marioObj) < 800.f) {
            renderDome(7);
        }
    }
}

void underwaterdoorcode(void) {
    if (!o->oOpacity) {
        renderDome(-1);
        o->oOpacity = 1;
    }
    o->oAnimState = o->oBehParams2ndByte ^ 1;
    o->oDrawingDistance = 65000.f;
    if ((openDoors & (o->oBehParams & 0xff)) || !(o->oBehParams & 0xff)) {
        switch (o->oAction) {
            case 0:
                if (o->oDistanceToMario < 1000.f) {
                    if (o->oDistanceToMario < findOtherDoor(o->oBehParams >> 0x18)->oDistanceToMario) {
                        if (findOtherDoor(o->oBehParams >> 0x18)->oAction != 1) {
                            o->oAction++;
                            // renderDome
                            cur_obj_play_sound_2(SOUND_GENERAL_STAR_DOOR_OPEN);
                            renderDome(o->oBehParams >> 24);
                        }
                    }
                }
                break;
            case 1:
#define HEIGHT 30
#define SPEED 25.f
#define goDownTimer 90
                if (o->oTimer < HEIGHT) {
                    o->oPosY += SPEED;
                } else {
                    if (*gEnvironmentLevels > 0) {
                        play_sound(SOUND_ENV_WATER_DRAIN, gDefaultSoundArgs);
                    }
                }
                if (o->oTimer > goDownTimer) {
                    o->oPosY -= SPEED;
                }
                if (o->oTimer == goDownTimer) {
                    cur_obj_play_sound_2(SOUND_GENERAL_STAR_DOOR_CLOSE);
                }
                if (o->oTimer >= goDownTimer + HEIGHT) {
                    o->oAction++;
                }
                break;
            case 2:
                if (o->oDistanceToMario > 1000.f) { // also check if dist to other door is bigger
                    o->oAction = 0;
                } else if (((o->oDistanceToMario
                             < findOtherDoor(o->oBehParams >> 0x18)->oDistanceToMario))
                           && (o->oTimer > 100)) {
                    o->oAction = 0;
                }
                break;
        }
    }
    load_object_collision_model();
}

extern Gfx selectDisplay_dsplay_mesh[];
void cardboardboxIntroCode(void) {
    if (!o->oTimer) {
        gMarioState->prevAction = gMarioState->action;
        gMarioState->statusForCamera->cameraEvent = 0xc8;
        //  gMarioState->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        advance_cutscene_step(gMarioState);
    }
    if (o->oTimer < 30) {
        o->header.gfx.unk38.animFrame = 0;
    }
    if (o->header.gfx.unk38.animFrame >78){
        cur_obj_set_model(0x3A);
    }
    gMarioState->controller->buttonDown |= START_BUTTON;
}

int fileindices[4][3] = { { 43, 40, 37 }, { 34, 31, 28 }, { 25, 22, 19 }, { 13, 10, 7 } };
extern Gfx mat_selectDisplay__0[];
extern Gfx mat_selectDisplay__1[];
extern Gfx mat_selectDisplay__2[];
extern Gfx mat_selectDisplay__3[];
extern Gfx mat_selectDisplay__4[];
extern Gfx mat_selectDisplay__5[];
extern Gfx mat_selectDisplay__6[];
extern Gfx mat_selectDisplay__7[];
extern Gfx mat_selectDisplay__8[];
extern Gfx mat_selectDisplay__9[];

Gfx number[] = {
    gsSPDisplayList(mat_selectDisplay__0), gsSPDisplayList(mat_selectDisplay__1),
    gsSPDisplayList(mat_selectDisplay__2), gsSPDisplayList(mat_selectDisplay__3),
    gsSPDisplayList(mat_selectDisplay__4), gsSPDisplayList(mat_selectDisplay__5),
    gsSPDisplayList(mat_selectDisplay__6), gsSPDisplayList(mat_selectDisplay__7),
    gsSPDisplayList(mat_selectDisplay__8), gsSPDisplayList(mat_selectDisplay__9),
};

s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse);
void filedisplaycode(void) {
    Gfx *a = segmented_to_virtual(selectDisplay_dsplay_mesh);
    int i, j;
    int n, k;
    for (i = 0; i < 4; i++) {
        n = save_file_get_total_star_count(i, 0, 0x18);
        for (j = 0; j < 3; j++) {
            switch (j) {
                case 2:
                    k = n % (10);
                    break;
                case 1:
                    k = (n / 10) % (10);
                    break;
                case 0:
                    k = (n / 100) % (10);
                    break;
            }
            a[fileindices[i][j]] = number[k];
        }
    }
}
void offsetRandom(struct Object *a, f32 diam) {
    a->oPosX += random_f32_around_zero(diam);
    a->oPosY += random_f32_around_zero(diam);
    a->oPosZ += random_f32_around_zero(diam);
}

extern Vtx bowsernet_Cube_mesh_vtx_0[39];
extern void obj_set_pos_relative(struct Object *obj, struct Object *other, f32 dleft, f32 dy,
                                 f32 dforward);
extern u16 renderBowserText;
void bowsercutsceneCode(void) {
    Vtx *a = segmented_to_virtual(bowsernet_Cube_mesh_vtx_0);
    u16 oldangle = o->oMoveAngleYaw;
    f32 prevScale;
    f32 x, y, z;
    struct Object *spawn;
    int i;
    x = o->oPosX;
    y = o->oPosY;
    z = o->oPosZ;
    for (i = 0; i < 39; i++) {
        if (a[i].n.flag) {
            a[i].n.ob[1] = -(o->oObjPointer1->oGraphYOffset / o->oObjPointer1->header.gfx.scale[1])
                           + 166; // usually 163 but we moved it up 3 coordinates for zbuffer
        }
    }
    switch (o->oAction) {
        case 0:
            // fly menacingly
            o->oObjPointer1->oGraphYOffset = 1000.f;
            gMarioState->action = ACT_INTRO_CUTSCENE;
            gMarioState->actionArg = 0;
            o->oObjPointer1->header.gfx.scale[1] = 1.0f;
            if (o->oTimer > 200) {
                o->oForwardVel = 4.f;
                o->oVelY = -3.0f;
            }
            if (o->oTimer == 300) {
                play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_BOSS_KOOPA), 0);
            }
            if (o->oTimer >= 400) {
                o->oAction = 1;
            }
            o->oFaceAngleYaw = o->oMoveAngleYaw;
            break;
        case 1:
            o->oVelY = 0;
            o->oForwardVel = 0.f;
            // talk to camera about abducting yoshis, do the dance
            if (o->oTimer == 20) {
                cur_obj_init_animation(0x0A);
                renderBowserText = 200;
            }
            if (o->oTimer > 0x5A) {
                cur_obj_init_animation(0x0C);
            }
            if (o->oTimer >= 240) {
                o->oAction = 2;
                o->oPosX = -342.f;
                o->oPosY = 2511.f;
                o->oPosZ = -1783.f;
                o->oMoveAngleYaw = 0x9000;
                o->oFaceAngleYaw = o->oMoveAngleYaw - 0x4000;
            }
            break;
        case 2:
            // throw food
            if ((o->oTimer == 100) || (o->oTimer == 130)) {

                play_sound(SOUND_GENERAL_SOFT_LANDING, gDefaultSoundArgs);
            }
            if (o->oTimer < 67) {
                o->oObjPointer1->header.gfx.scale[1] = 1.0f;
            } else if (o->oTimer < 100) {
                cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
                o->oObjPointer1->header.gfx.scale[1] = 1.0f;
                o->oObjPointer1->oVelY -= 1.0425f;
            } else if (o->oTimer < 130) {
                cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
#define finalScale 0.01575f
                o->oObjPointer1->oVelY = 0.f;
                prevScale = o->oObjPointer1->header.gfx.scale[1];
                o->oObjPointer1->header.gfx.scale[1] =
                    1.0f - ((1.0f - finalScale) / 30) * (o->oTimer - 100);
                o->oObjPointer1->header.gfx.scale[0] = 1 + (o->oTimer - 100) * 0.0025f;
                o->oObjPointer1->header.gfx.scale[2] = o->oObjPointer1->header.gfx.scale[0];

                o->oObjPointer1->oGraphYOffset -=
                    (prevScale - o->oObjPointer1->header.gfx.scale[1]) * 2100.f;
            } else if (o->oTimer == 145) {
                spawn = spawn_object(o->oObjPointer1, 0x38, bhvCutsceneObjectFruit);
                spawn->oObjPointer1 = o->oObjPointer1;
                offsetRandom(spawn, 500.f);

                spawn = spawn_object(o->oObjPointer1, 0x39, bhvCutsceneObjectFruit);
                spawn->oObjPointer1 = o->oObjPointer1;
                offsetRandom(spawn, 500.f);

                spawn = spawn_object(o->oObjPointer1, 0x3A, bhvCutsceneObjectFruit);
                spawn->oObjPointer1 = o->oObjPointer1;
                offsetRandom(spawn, 500.f);

                spawn = spawn_object(o->oObjPointer1, 0x38, bhvCutsceneObjectFruit);
                spawn->oObjPointer1 = o->oObjPointer1;
                offsetRandom(spawn, 500.f);

                spawn = spawn_object(o->oObjPointer1, 0x39, bhvCutsceneObjectFruit);
                spawn->oObjPointer1 = o->oObjPointer1;
                offsetRandom(spawn, 500.f);

                spawn = spawn_object(o->oObjPointer1, 0x3A, bhvCutsceneObjectFruit);
                spawn->oObjPointer1 = o->oObjPointer1;
                offsetRandom(spawn, 500.f);
#define WAITTIME 120
            } else if ((o->oTimer < (210 + WAITTIME)) && (o->oTimer > (180 + WAITTIME))) {
                cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
                o->oObjPointer1->oVelY = 0.f;
                prevScale = o->oObjPointer1->header.gfx.scale[1];
                o->oObjPointer1->header.gfx.scale[1] =
                    finalScale + ((1.0f - finalScale) / 30) * (o->oTimer - 180 - WAITTIME);
                o->oObjPointer1->header.gfx.scale[0] =
                    1 - (o->oTimer - 180 - WAITTIME) * 0.0025f + 0.0025f * 30.f;
                o->oObjPointer1->header.gfx.scale[2] = o->oObjPointer1->header.gfx.scale[0];

                o->oObjPointer1->oGraphYOffset -=
                    (prevScale - o->oObjPointer1->header.gfx.scale[1]) * 2100.f;
            } else if ((o->oTimer < (239 + WAITTIME)) && (o->oTimer > (180 + WAITTIME))) {
                cur_obj_play_sound_1(SOUND_AIR_HEAVEHO_MOVE);
                // o->oObjPointer1->header.gfx.scale[1] = 1.0f;
                o->oObjPointer1->oVelY = 10.425f;
            } else if (o->oTimer > (180 + WAITTIME)) {
                //  o->oObjPointer1->header.gfx.scale[1] = 1.0f;
                o->oObjPointer1->oVelY = 0.f;
            }
            if (o->oTimer > (415)) {
                o->oAction = 4;
            }
            break;
        case 3:
            // pull net up
            break;
        case 4:
            // text about enslaving them
            if (o->oTimer == 20) {
                cur_obj_init_animation(0x0A);
                renderBowserText = 235;
            }
            if (o->oTimer > 0x5A) {
                cur_obj_init_animation(0x0C);
            }
            if (o->oTimer > 270) {

                cur_obj_init_animation(0x0B);
            }
            if (o->oTimer > 280) {
                o->oAction = 5;
                play_sound(SOUND_OBJ2_BOWSER_ROAR, gDefaultSoundArgs);
            }
            break;
        case 5:
#define targetangle 0xF00
            // fly off, drop 9 star gate
            if (o->oTimer == 75) {
                play_sound(SOUND_GENERAL_YOSHI_AWOWOWO, gDefaultSoundArgs);
            }
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetangle, 0x0100);
            o->oForwardVel = approach_f32_symmetric(
                o->oForwardVel, (coss(o->oMoveAngleYaw - targetangle) + 1.0f) * 12.5f, 1.0f);
            o->oVelY = approach_f32_symmetric(o->oVelY, 21.f, 1.5f);
            o->oFaceAngleYaw += (o->oMoveAngleYaw - oldangle);
            if (o->oTimer >= 140) {
                play_sound(SOUND_OBJ_SOMETHING_LANDING, gDefaultSoundArgs);
                spawn = spawn_object(o, 0x3b, bhvStargate);
                spawn->oPosX = -726.f;
                spawn->oPosY = 5440.f;
                spawn->oPosZ = 56.f;
                spawn->oFaceAngleYaw = 0;
                spawn->oBehParams2ndByte = 1;
                o->oAction = 6;
            }
            break;
        case 6:
            // fly off and despawn
            if (o->oTimer == 120) {
                // play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_HAPPYVILLAGE), 0);
                level_trigger_warp(m, WARP_OP_WARP_FLOOR);
                play_transition(WARP_TRANSITION_FADE_INTO_BOWSER, 0x30, 0x00, 0x00, 0x00);
                play_sound(SOUND_MENU_BOWSER_LAUGH, gDefaultSoundArgs);
                sDelayedWarpTimer = 48;
                sSourceWarpNodeId = 0x20;
            }
            break;
    }
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oPosY += o->oVelY;
    obj_set_pos_relative(o->oObjPointer1, o, 0, -975.f, -730.f);
    o->oObjPointer1->oGraphYOffset += o->oObjPointer1->oVelY;
    obj_set_pos_relative(o->oObjPointer2, o, 0, -975.f, -730.f);
}

void fruitCode(void) {
    f32 minPos;
    f32 x, z;
#define downMinOffset -2000.f
    if (!o->oTimer) {
        o->oHomeX = o->oObjPointer1->oPosX;
        o->oHomeZ = o->oObjPointer1->oPosZ;
    }
    o->oPosX += o->oObjPointer1->oPosX - o->oHomeX;
    o->oPosZ += o->oObjPointer1->oPosZ - o->oHomeZ;
    o->oHomeX = o->oObjPointer1->oPosX;
    o->oHomeZ = o->oObjPointer1->oPosZ;

    cur_obj_update_floor_and_walls();
    minPos = downMinOffset + o->oObjPointer1->oGraphYOffset + o->oObjPointer1->oPosY;
    // 845radius
    // 720 max pos
    x = o->oPosX - o->oObjPointer1->oPosX;
    z = o->oPosZ - o->oObjPointer1->oPosZ;
    minPos += (sqrtf(x * x + z * z) / 845.f) * (sqrtf(x * x + z * z) / 845.f) * 720.f
              * o->oObjPointer1->header.gfx.scale[1];
    if (minPos > o->oFloorHeight) {
        o->oFloorHeight = minPos;
    }
    cur_obj_move_standard(-78);
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        play_sound(SOUND_GENERAL_SOFT_LANDING, gDefaultSoundArgs);
    }
}

void cur_obj_spawn_particlesOffset(struct SpawnParticlesInfo *info, f32 dLeft, f32 dY, f32 dForward) {
    struct Object *particle;
    s32 i;
    f32 scale;
    s32 numParticles = info->count;

    // If there are a lot of objects already, limit the number of particles
   /* if (gPrevFrameObjectCount > 150 && numParticles > 10) {
        numParticles = 10;
    }*/

    // We're close to running out of object slots, so don't spawn particles at
    // all
    /*if (gPrevFrameObjectCount > 210) {
        numParticles = 0;
    }*/

    for (i = 0; i < numParticles; i++) {
        scale = random_float() * (info->sizeRange * 0.1f) + info->sizeBase * 0.1f;

        particle = spawn_object(o, info->model, bhvWhitePuffExplosion);

        obj_set_pos_relative(particle, o, dLeft, dY, dForward);
        particle->oBehParams2ndByte = info->behParam;
        particle->oMoveAngleYaw = random_u16();
        particle->oGravity = info->gravity;
        particle->oDragStrength = info->dragStrength;

        particle->oPosY += info->offsetY;
        particle->oForwardVel = random_float() * info->forwardVelRange + info->forwardVelBase;
        particle->oVelY = random_float() * info->velYRange + info->velYBase;
        obj_scale_xyz(particle, scale, scale, scale);
    }
}

struct SpawnParticlesInfo dusty = {
    0, 5, MODEL_WHITE_PARTICLE_DL, 0, 0, 20, 20, 0, 252, 30, 2.0f, 2.0f
};

void hideGate(void) {
    o->oAction = 1;
    cur_obj_hide();
}
extern struct CutsceneJump *currentScene;
void stargatecode(void) {
    if (!o->oBehParams2ndByte) {
        if (currentScene) {
            hideGate();
        }
    }
    if (save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 0x18) >= 8) {
        hideGate();
    }
    if (o->oTimer > 10) {
        if (o->oMoveFlags & OBJ_MOVE_LANDED) {
            cur_obj_play_sound_2(SOUND_GENERAL_BIG_POUND);
            o->oOpacity = 6;
        }
        if (o->oOpacity) {
            o->oOpacity--;
            set_camera_pitch_shake(0x60, 0xC, 0x8000);
        }
    }
    if (!o->oAction) {

        load_object_collision_model();
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
}

u16 textIDs[] = { 35, 36, 39, 40, 42, 43, 46, 48, 49, 50, 51, 52, 53, 54 };
u8 songcount = sizeof(textIDs) / 2;

extern int stated;
void stateyosh(void) {
    o->oDrawingDistance = 32000.f;
    switch (o->oAction) {
        case 0:
            // cutscen, come from above
            if (!o->oTimer) {
                o->oHomeY = gMarioState->pos[1];
            } else {
                o->oPosY += gMarioState->pos[1] - o->oHomeY;
                o->oHomeY = gMarioState->pos[1];
            }
            if (o->oTimer < 40) {
                o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
                return;
            }
            if (!o->oSubAction) {
                if (gMarioState->action & ACT_FLAG_INTANGIBLE) {
                    return;
                } else {
                    o->oSubAction = 1;
                    sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
                    play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_EVENT_YOSHIBEAT), 0);
                }
            }
            cutscene_object(CUTSCENE_STAR_SPAWN, o);
            set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
            o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
            o->oAction++;
            o->oVelY = -80.f;
            o->oForwardVel = 50.f;
            break;
        case 1:
            if (o->oTimer > 50) {
                o->oVelY *= 0.98f;
            }
            if (o->oTimer > 200) {
                gObjCutsceneDone = TRUE;
                clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
                o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
                o->oAction = 2;
                o->oBehParams2ndByte = random_u16() % songcount;
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 0.5f);
            // cutscene wait
            break;
        case 2:
            // talk
            if (talkToMario(textIDs[o->oBehParams2ndByte], 2)) {
                o->oAction = 3;
            }
            o->oVelY = 0.f;
            o->oForwardVel = 0.f;
            break;
        case 3:
            // fly away
            o->oVelY += 2.f;
            o->oForwardVel -= 2.f;
            if (o->oTimer > 100) {
                mark_obj_for_deletion(o);
                stop_background_music(SEQUENCE_ARGS(4, SEQ_EVENT_YOSHIBEAT));
            }
            break;
    }
    o->oVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);
    cur_obj_move_using_vel();
}

static struct ObjectHitbox bunnyBox = {
    /* interactType:      */ INTERACT_BOUNCE_TOP,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 2,
    /* health:            */ 0,
    /* numLootCoins:      */ 2,
    /* radius:            */ 160,
    /* height:            */ 120,
    /* hurtboxRadius:     */ 100,
    /* hurtboxHeight:     */ 94,
};

u8 bunnyHandle[6] = {
    // regular and tiny
    /* ATTACK_PUNCH:                 */ ATTACK_HANDLER_KNOCKBACK,
    /* ATTACK_KICK_OR_TRIP:          */ ATTACK_HANDLER_KNOCKBACK,
    /* ATTACK_FROM_ABOVE:            */ ATTACK_HANDLER_SQUISHED,
    /* ATTACK_GROUND_POUND_OR_TWIRL: */ ATTACK_HANDLER_SQUISHED,
    /* ATTACK_FAST_ATTACK:           */ ATTACK_HANDLER_KNOCKBACK,
    /* ATTACK_FROM_BELOW:            */ ATTACK_HANDLER_KNOCKBACK,
};

extern s32 approach_f32_ptr(f32 *px, f32 target, f32 delta);
extern f32 cur_obj_lateral_dist_to_home(void);
extern s32 approach_s16_symmetric(s16 value, s16 target, s32 increment);

void despawnonBparam1Stars(void) {
    if (save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 0x18) >= (o->oBehParams >> 24)) {
        mark_obj_for_deletion(o);
    }
}

void deletebutton(void) {
    int i = 0;
    struct Object *spawmn = 0;
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_on_platform()) {
                o->oBobombBuddyPosXCopy = -15.f;
            } else {

                o->oBobombBuddyPosXCopy = 0.f;
            }
            if (cur_obj_is_mario_ground_pounding_platform()) {
                o->oVelY = -10.f;
                cur_obj_play_sound_2(SOUND_GENERAL_BUTTON_PRESS);
                o->oBobombBuddyPosXCopy = -35.f;
                o->oAction = 1;
                spawmn = spawn_object(o, 0x39, bhvDeletePiranha);
                spawmn->oBehParams = o->oBehParams;
                spawmn->oPosY -= 200.f;
                spawmn->oPosZ = -885.f;
            }
            break;
        case 1:
            o->oBobombBuddyPosXCopy = -35.f;
            if (o->oTimer > 80) {
                o->oAction++;
            }
            break;
        case 2:
            o->oAction = 0;
            break;
    }
    o->oVelY += (o->oHomeY - o->oPosY + o->oBobombBuddyPosXCopy) / 5.f;
    o->oVelY *= 0.5f;
    o->oPosY += o->oVelY;
    load_object_collision_model();
};
extern struct Object *spawn_object_relative(s16 behaviorParam, s16 relativePosX, s16 relativePosY,
                                            s16 relativePosZ, struct Object *parent, s32 model,
                                            const BehaviorScript *behavior);
void deleteplant(void) {
    struct Object *flame;
    f32 flameVel;
    s32 sp34;
    s32 model;
    int i;
    switch (o->oAction) {
        case 0:
            if (!o->oTimer) {
                cur_obj_play_sound_2(SOUND_MENU_EXIT_PIPE);
            }
            o->oPosY += 9.f;
            if (o->header.gfx.unk38.animFrame > 40) {
                o->oAction++;
            }
            break;
        case 1:
            if (cur_obj_check_if_at_animation_end()) {
                o->header.gfx.unk38.animFrame = 40;
            }
            i = askMario(62, 1, 4);
            switch (i) {
                case 0:
                    break;
                case 1:
                    // no
                    o->oAction = 3;
                    break;
                case 2:
                    // yes
                    o->oAction = 2;
                case 3:

                    break;
            }
            break;
        case 2:
#define BLOWSTART 102
#define BLOWEND 144
#define INHALEFRAME 55
#define EXHALEFRAME 100
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, 0x0000, 0x0100);
            cur_obj_init_animation(2);
            if (o->header.gfx.unk38.animFrame == INHALEFRAME) {
                cur_obj_play_sound_2(SOUND_OBJ_BOWSER_INHALING);
            }
            if (o->header.gfx.unk38.animFrame == EXHALEFRAME) {
                cur_obj_play_sound_2(SOUND_AIR_BOWSER_SPIT_FIRE);
            }
            if ((o->header.gfx.unk38.animFrame > BLOWSTART)
                && (o->header.gfx.unk38.animFrame < BLOWEND)) {

                model = MODEL_RED_FLAME;
                flameVel = -10.0f;
                sp34 = 1;
                if (o->oTimer < (60 + BLOWSTART))
                    sp34 = 15;
                else if (o->oTimer < (74 + BLOWSTART))
                    sp34 = 75 - o->oTimer; // Range: [15..2]
                else
                    o->oAction++;
                o->oFlameThowerUnk110 = sp34;
                flame = spawn_object_relative(o->oBehParams2ndByte, 0, 200.f, -50.f, o, model,
                                              bhvFlamethrowerFlame);
                flame->oForwardVel = flameVel;
                flame->oVelY = 7.f;
                cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
            }
            if (o->oTimer == 120) {
                play_sound(SOUND_MARIO_WAAAOOOW, gDefaultSoundArgs);
                save_file_erase((o->oBehParams >> 24) - 1);
            }
            if (cur_obj_check_if_at_animation_end()) {
                o->oAction++;
            }
            break;
        case 3:
            if (!o->oTimer) {
                cur_obj_play_sound_2(SOUND_MENU_ENTER_PIPE);
            }
            cur_obj_init_animation(1);
            o->oPosY -= 9.f;
            if (o->oTimer > 40) {
                mark_obj_for_deletion(o);
            }
            break;
    }
}

void checkRun() {
    u8 b = 1;
    struct Object *p;
    if (o->oDistanceToMario < 500.f) {
        o->oBehParams2ndByte = obj_angle_to_object(gMarioObject, o);
        o->oAction = 3;
        cur_obj_play_sound_1(SOUND_GENERAL_BIRDS_FLY_AWAY);
    }
}

extern f32 obj_find_floor_height(struct Object *obj);
void freebird(void) {
    struct Object *coin;
    f32 x, y, z;
    x = o->oPosX;
    y = o->oPosY;
    z = o->oPosZ;
    //o->oGraphYOffset = 18.f * o->header.gfx.scale[1];
    switch (o->oAction) {
        case 0:
            // init values
            if (o->header.gfx.sharedChild == gLoadedGraphNodes[MODEL_BIRDS]) {
                cur_obj_scale(1.f + random_f32_around_zero(0.5f));
            } else {
                cur_obj_scale(2.f);
                o->oDrawingDistance = 16000.f;
            }
            o->oOpacity = o->oMoveAngleYaw;
            o->oMoveAngleYaw = random_u16();
            o->oAction++;
            cur_obj_init_animation(2);
            o->header.gfx.unk38.animFrame = random_u16() % 50;
            break;
        case 1:
            // stand around
            if (o->header.gfx.unk38.animFrame == 53) {
                if (!(random_u16() & 1)) {
                    o->oAction = 2;
                } else {
                    o->header.gfx.unk38.animFrame = 0;
                }
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0.f, 1.f);
            checkRun();
            break;
        case 2:
            // smol hop
            if (o->oTimer == 5) {
                o->oVelY = 20.f;
                o->oForwardVel = 8.f;
                if (cur_obj_lateral_dist_to_home() < 400.f) {
                    o->oMoveAngleYaw = random_u16();
                } else {
                    o->oMoveAngleYaw = cur_obj_angle_to_home();
                }
            }
            if (o->oTimer == 25) {
                o->oAction = 1;
            }
            checkRun();
            break;
        case 3:
            cur_obj_init_animation(0);
            // fly away, drop coin
            if (!o->oTimer) {
                if (o->header.gfx.sharedChild == gLoadedGraphNodes[MODEL_BIRDS]) {
                    coin = spawn_object(o, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
                    coin->oForwardVel = random_float() * 10;
                    coin->oVelY = random_float() * 40 + 20;
                    coin->oMoveAngleYaw = random_u16();
                } else {
                    coin = spawn_object(o, MODEL_RED_COIN, bhvRedCoin);
                    coin->oFloorHeight = o->oFloorHeight;
                    coin->oFloorPointer = o->oFloorPointer;
                    cur_obj_scale(1.5f);
                }
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 50.f, 5.f);
            o->oVelY = approach_f32_symmetric(o->oVelY, 30.f, 3.f);
            if (o->oTimer > 8) {
                o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oOpacity, 0x800);
            } else {
                o->oMoveAngleYaw =
                    approach_s16_symmetric(o->oMoveAngleYaw, o->oBehParams2ndByte, 0x200);
            }
            if (o->oTimer > 210) {
                obj_mark_for_deletion(o);
            }
            break;
    }
    if ((o->oAction == 3) && (o->oTimer > 40)) {
        cur_obj_move_using_vel();
    } else if (o->oAction !=1){
        cur_obj_update_floor_and_walls();
        cur_obj_move_standard(-78);
        if (o->oAction != 3) {
            if ((obj_find_floor_height(o)) < o->oHomeY) {
                o->oPosX = x;
                o->oPosZ = z;
            }
        }
    }
}

static struct SpawnParticlesInfo grounDirst = {
    /* behParam:        */ 0,
    /* count:           */ 2,
    /* model:           */ MODEL_SAND_DUST,
    /* offsetY:         */ 0,
    /* forwardVelBase:  */ 4,
    /* forwardVelRange: */ 4,
    /* velYBase:        */ 20,
    /* velYRange:       */ 15,
    /* gravity:         */ -4,
    /* dragStrength:    */ 0,
    /* sizeBase:        */ 10.0f,
    /* sizeRange:       */ 7.0f,
};

void stickToWall(f32 radius) {
    Vec3f dir;
    struct Surface *hit_surface;
    Vec3f hit_pos;
    f32 x, y, z;
    dir[0] = sins(o->oMoveAngleYaw + 0x8000) * 100.f;
    dir[1] = 0.f;
    dir[2] = coss(o->oMoveAngleYaw + 0x8000) * 100.f;
    find_surface_on_ray(&o->oPosX, dir, &hit_surface, hit_pos, 2);
    if (hit_surface) {
        x = hit_pos[0] - o->oPosX;
        y = hit_pos[1] - o->oPosY;
        z = hit_pos[2] - o->oPosZ;
        if (sqrtf(x * x + z * z + y * y) < 100.f) {
            o->oPosX = hit_pos[0] + hit_surface->normal.x * 35.f;
            o->oPosY = hit_pos[1];
            o->oPosZ = hit_pos[2] + hit_surface->normal.z * 35.f;
            o->oMoveAngleYaw = atan2s(hit_surface->normal.z, hit_surface->normal.x);
        }
    }
}

void spooder(void) {
    struct Object *coin;
    f32 x, y, z;
    switch (o->oAction) {
        case 0:
            // IDLE AROUND
            if (!o->oOpacity) {
                o->oBobombBuddyPosXCopy = o->oHomeX + random_f32_around_zero(100.f);
                o->oBobombBuddyPosYCopy = o->oHomeY + random_f32_around_zero(100.f);
                o->oBobombBuddyPosZCopy = o->oHomeZ + random_f32_around_zero(100.f);
                o->oOpacity = (random_u16() % 120) + 110;
            }
            o->oOpacity--;
            x = o->oPosX;
            y = o->oPosY;
            z = o->oPosZ;
            o->oPosX = approach_f32_symmetric(o->oPosX, o->oBobombBuddyPosXCopy, 1.f);
            o->oPosY = approach_f32_symmetric(o->oPosY, o->oBobombBuddyPosYCopy, 1.f);
            o->oPosZ = approach_f32_symmetric(o->oPosZ, o->oBobombBuddyPosZCopy, 1.f);
            x = o->oPosX - x;
            y = o->oPosY - y;
            z = o->oPosZ - z;

            if (sqrtf(x * x + y * y + z * z) > 1.5f) {
                x = o->oPosX - o->oBobombBuddyPosXCopy;
                y = o->oPosY - o->oBobombBuddyPosYCopy;
                z = o->oPosZ - o->oBobombBuddyPosZCopy;
                o->oFaceAngleRoll = approach_s16_symmetric(
                    o->oFaceAngleRoll, atan2s(y, sqrtf(x * x + z * z)) - 0x4000, 0x0400);
                if (!(o->oTimer & 7)) {
                    o->oAnimState++;
                }
            } else {
                if (!(o->oTimer & 0x3f)) {
                    o->oAnimState++;
                }
            }
            if (o->oDistanceToMario < 500.f) {
                o->oAction++;
            }
            break;
        case 1:
            o->oFaceAngleRoll = approach_s16_symmetric(o->oFaceAngleRoll, 0x8000, 0x800);
            if (o->oTimer & 1) {
                o->oAnimState++;
            }
            if (abs_angle_diff(o->oFaceAngleRoll, 0x8000) < 0x0800) {
                o->oAction++;
                o->oVelY -= 2.f;
                cur_obj_play_sound_1(SOUND_GENERAL_SHAKE_COFFIN);
            }
            break;
        case 2:
            o->oFaceAngleRoll = approach_s16_symmetric(o->oFaceAngleRoll, 0x8000, 0x800);
            o->oAnimState++;
            o->oVelY -= 2.f;
            if (o->oPosY < o->oFloorHeight) {
                o->oAction++;
            }
            break;
        case 3:
            cur_obj_hide();
            o->oPosY = o->oFloorHeight;
            if (o->oTimer > 25) {
                coin = spawn_object(o, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
                coin->oForwardVel = random_float() * 10;
                coin->oVelY = random_float() * 40 + 20;
                coin->oMoveAngleYaw = random_u16();
                mark_obj_for_deletion(o);
            } else {
                cur_obj_spawn_particles(&grounDirst);
            }
    }
    cur_obj_update_floor_height();
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oPosY += o->oVelY;
    stickToWall(50.f);
}

void grasshopper(void) {
    struct Object *coin;
    f32 x, y, z;
    switch (o->oAction) {
        case 0:
            o->oAnimState = 0;
            if ((o->oTimer > o->oOpacity) || ((o->oTimer > 10) && (o->oDistanceToMario < 250.f))) {
                o->oAction = 1;
                o->oVelY = 50.f;
                o->oForwardVel = 40.f;
                if (cur_obj_lateral_dist_to_home() < 1500.f) {
                    o->oMoveAngleYaw = random_u16();
                } else {
                    o->oMoveAngleYaw = cur_obj_angle_to_home();
                }
            }
            break;
        case 1:
            o->oAnimState = 1;
            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                o->oAction = 0;
                o->oOpacity = (random_u16() % 150) + 100;
                o->oForwardVel = 0;
            }
            break;
    }
    if (o->oDistanceToMario < 100.f) {
        coin = spawn_object(o, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
        coin->oForwardVel = random_float() * 10;
        coin->oVelY = random_float() * 40 + 20;
        coin->oMoveAngleYaw = random_u16();
        mark_obj_for_deletion(o);
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
}

void bhv_signpost(void) {
    if (cur_obj_is_mario_ground_pounding_platform()) {
        o->oPosY -= 6.f;
        o->oOpacity += 6;
    }
    if (o->oOpacity >= 124) {
        mark_obj_for_deletion(o);
        spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
    }
}

void followPath(u32 *progress, s16 *trajectory, s16 rotSpeed, f32 range, f32 fVel) {
    f32 x, z, y;
    x = ((s16 *) segmented_to_virtual(trajectory))[1 + *progress];
    z = ((s16 *) segmented_to_virtual(trajectory))[3 + *progress];
    y = ((s16 *) segmented_to_virtual(trajectory))[2 + *progress];
    if (sqrtf((x - o->oPosX) * (x - o->oPosX) + (z - o->oPosZ) * (z - o->oPosZ)) < range) {
        *progress += 4;
        if ((((s16 *) segmented_to_virtual(trajectory))[*progress]) == -1) {
            *progress = 0;
        }
    }
    if (absf(o->oPosY - y) > 10.f) {
        o->oVelY += (y - o->oPosY) / absf(o->oPosY - y) * 2.f;
    } else {
        o->oVelY *= 0.97f;
    }
    if (o->oVelY > 30.f) {
        o->oVelY = 30.f;
    }
    if (o->oVelY < -30.f) {
        o->oVelY = -30.f;
    }
    o->oMoveAngleYaw =
        approach_s16_symmetric(o->oMoveAngleYaw, atan2s(z - o->oPosZ, x - o->oPosX), rotSpeed);
    o->oForwardVel = fVel;
        obj_move_xyz_using_fvel_and_yaw();
}

u32 deactivateOnDrawDist(f32 multiplier) {
    if (o->oDistanceToMario > o->oDrawingDistance * multiplier) {
        return 1;
    } else {
        return 0;
    }
}

void killbparam(void) {
}

extern void spawn_triangle_break_particlesOffset(s16 numTris, s16 triModel, f32 triSize,
                                                 s16 triAnimState, f32 Offset);
void bowsercartOW(void) {
    switch (o->oAction) {
        case 0:
            if (/*save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 0x18) < 18*/0) {
                // mark_obj_for_deletion(o);
                cur_obj_set_model(0);
            } else {
                cur_obj_set_model(0x45);
                o->oAction = 1;
            }
            break;
        case 1:
            if (cur_obj_is_mario_on_platform()) {
                gMarioState->action = ACT_RIDE_MINECART;
                cur_obj_play_sound_2(SOUND_GENERAL_WATER_LEVEL_TRIG);
                o->oAction = 2;
            }
            load_object_collision_model();
            break;
        case 2:
            gMarioState->action = ACT_RIDE_MINECART;
            o->oForwardVel = approach_f32(o->oForwardVel, 25.f, 1.0f, 1.0f);
            if (o->oTimer == 25) {
                cur_obj_nearest_object_with_behavior(bhvSolidUnitlB2Stars)->activeFlags = 0;
                spawn_mist_particles_variable(0, 375.f, 92.f);
                spawn_triangle_break_particlesOffset(30, 138, 4.0f, 4, 100.f);
                cur_obj_play_sound_2(SOUND_GENERAL_EXPLOSION7);
                set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY, o->oPosZ);
            } else if (o->oTimer == 35) {
                m->usedObj = o;
                level_trigger_warp(m, WARP_OP_WARP_DOOR);
            }
            cur_obj_update_floor_and_walls();
            cur_obj_move_standard(-78);
            load_object_collision_model();
            break;
    }
}

void upcarpet(void) {
    f32 x, z;
    struct Object *dust;
    s32 i;
    switch (o->oAction) {
        case 0:
            if (gMarioState->controller->buttonDown & L_TRIG) {
                o->oAction = 1;
                cur_obj_play_sound_2(SOUND_GENERAL_CANNON_UP);
            }
            break;
        case 1:
            if (o->oTimer < 125) {
                o->oVelY = 20.f;
                for (i = 0; i < 5; i++) {
                    switch (random_u16() & 3) {
                        case 0:
                            x = o->oPosX - 240.f;
                            z = o->oPosZ - 540.f * random_f32_around_zero(2.f);
                            break;
                        case 1:
                            x = o->oPosX + 240.f;
                            z = o->oPosZ + 540.f * random_f32_around_zero(2.f);
                            break;
                        case 2:
                            z = o->oPosZ - 540.f;
                            x = o->oPosX + 240.f * random_f32_around_zero(2.f);
                            break;
                        case 3:
                            z = o->oPosZ + 540.f;
                            x = o->oPosX + 240.f * random_f32_around_zero(2.f);
                            break;
                    }
                    dust = spawn_object(o, MODEL_SMOKE, bhvDustBackwards);
                    dust->oPosX = x;
                    dust->oPosZ = z;
                    dust->oPosY = 10.f;
                    dust->oMoveAngleYaw = obj_angle_to_object(dust, o);
                }
            } else {
                o->oVelY = 0;
            }
            o->oPosY += o->oVelY;
            break;
    }
    load_object_collision_model();
}
//3 pointers to store stuff. stiffness should be around 500. requires floorheight to be set in behavior. assumes 1 to be default scale.
void cur_obj_animate_Yscale(f32 *prevVel, f32 *squishspeed, f32 *squishyness, f32 stiffness) {
    f32 y;
    f32 diff = o->oVelY - *prevVel;
    if (o->oPosY == o->oFloorHeight) {
        diff = -diff;
    }
    if ((o->oVelY == *prevVel) & (o->oVelY == -75.f)) {
        diff = 4.f;
    }
    y = o->header.gfx.scale[1];
    *squishspeed += diff;
    *squishspeed -= (*squishspeed * 0.05f);
    *squishspeed = approach_f32_symmetric(*squishspeed, 0.f, 1.f);
    *squishspeed *= .99f;
    *squishspeed -= *squishyness * 0.1f;
    *squishyness += *squishspeed;
    // y = 1.f + *squishyness/75.f;
    y = approach_f32_asymptotic(y, *squishyness / stiffness + 1.f, 0.05f);

    *prevVel = o->oVelY;
    vec3f_set(o->header.gfx.scale, sqrtf(1.f / y), y, sqrtf(1.f / y));
}

void bunnygoomba(void) {
    u16 randomyaw;
    f32 randomDist;
    f32 preveVel = o->oVelY;
    if (!o->oOpacity) {
        obj_set_hitbox(o, &bunnyBox);
    }

    switch (o->oAction) {
        case 0:
            if (obj_handle_attacks(&bunnyBox, GOOMBA_ACT_ATTACKED_MARIO, bunnyHandle)) {
            }
            // walk around home to random locations
            if (o->header.gfx.unk38.animFrame == 1) {
                randomyaw = random_u16();
                randomDist = random_f32_around_zero(1024.f);
                o->oGoalX = o->oHomeX + sins(randomyaw) * randomDist;
                o->oGoalZ = o->oHomeZ + coss(randomyaw) * randomDist;
                o->oVelY = 40.f;
                o->oForwardVel = 40.f;
                cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_ALERT);
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 1.f, 1.f);
            if (o->header.gfx.unk38.animFrame > 38) {
                o->oMoveAngleYaw = approach_s16_symmetric(
                    o->oMoveAngleYaw, atan2s(-o->oPosZ + o->oGoalZ, -o->oPosX + o->oGoalX), 0x400);
                o->oForwardVel = 0.f;
            }
            if (cur_obj_lateral_dist_to_home() < 2000.f) {
                if (o->oDistanceToMario < 1000.f) {
                    o->oAction = 1;
                }
            }
            break;
        case 1:
            if (obj_handle_attacks(&bunnyBox, GOOMBA_ACT_ATTACKED_MARIO, bunnyHandle)) {
            }
            // attack mario
            if (o->header.gfx.unk38.animFrame == 1) {
                o->oVelY = 40.f;
                o->oForwardVel = 40.f;
                cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_ALERT);
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 1.f, 1.f);
            if (o->header.gfx.unk38.animFrame > 38) {
                o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x800);
                o->oForwardVel = 0.f;
            }
            /*if (o->header.gfx.unk38.animFrame > 40) {
                o->header.gfx.unk38.animFrame += 2;
            }*/
            if (cur_obj_lateral_dist_to_home() > 2000.f) {
                o->oAction = 0;
            }
            break;
        case OBJ_ACT_SQUISHED:
            // die from jump
            o->oForwardVel = 0.0f;
            if (approach_f32_ptr(&o->header.gfx.scale[1], 0.2f, 0.14f)) {
                o->header.gfx.scale[0] = o->header.gfx.scale[2] = 2.0f - o->header.gfx.scale[1];
            }
            if (o->header.gfx.unk38.animFrame) {
                o->header.gfx.unk38.animFrame--;
            }
            if (o->oTimer == 12) {
                obj_spawn_loot_yellow_coins(o, 2, 20.0f);
                obj_mark_for_deletion(o);
                spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
            }
            break;
        case OBJ_ACT_HORIZONTAL_KNOCKBACK:
        case OBJ_ACT_VERTICAL_KNOCKBACK:
            if (!o->oTimer) {
                o->oMoveAngleYaw = o->oAngleToMario;
                o->oForwardVel = -48.f;
                o->oVelY = 40.f;
            }
            if (o->header.gfx.unk38.animFrame) {
                o->header.gfx.unk38.animFrame--;
            }
            if (o->oTimer == 12) {
                obj_spawn_loot_yellow_coins(o, 2, 20.0f);
                obj_mark_for_deletion(o);
                spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
            }
            // die from punch
            break;
    }
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        cur_obj_play_sound_2(SOUND_GENERAL_BOX_LANDING);
    }
    cur_obj_animate_Yscale(&preveVel, &o->oActivatedBackAndForthPlatformMaxOffset, &o->oActivatedBackAndForthPlatformOffset, 200.f);
}