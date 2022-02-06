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

#define m gMarioState
#define o gCurrentObject
#include "game/OPT_FOR_SIZE.h"

extern Gfx mat_wf_dl_sm64_material_025[];
#define SPAWNTIME 20
#define RANGE 150.f
u8 openDoors = 0;

Gfx switchout[] = {
    gsSPDisplayList(mat_wf_dl_sm64_material_025),
};

extern f32 newcam_pos[3]; // Position the camera is in the world
/** FOG macros
 * fm = z multiplier
 * fo = z offset
 * FOG FORMULA:    alpha(fog) = (eyespace z) * fm  + fo  CLAMPED 0 to 255
 *   note: (eyespace z) ranges -1 to 1
 * eye Z is from after dividing by W, so it might not be linear in world space (it should look linear
 * though)
 */
extern struct CutsceneJump *currentScene;
void CpuFog(u8 R, u8 G, u8 B, u32 near, u32 far, Vtx *apply, u16 vtxCount, f32 alphamultiply) {
    u16 i;
    register s16 camX = newcam_pos[0];
    register s16 camY = newcam_pos[1];
    register s16 camZ = newcam_pos[2];
    register s16 distX;
    register s16 distY;
    register s16 distZ;
    register s32 multiplier = 0;
    if (currentScene) {
        // use cutscene cam values instead
        camX = gLakituState.pos[0];
        camY = gLakituState.pos[1];
        camZ = gLakituState.pos[2];
    }
    near = near * near;
    far = far * far;
    if (apply[0].v.flag == 0) {
        for (i = 0; i < vtxCount; i++) {
            apply[i].v.flag = GPACK_RGBA5551(apply[i].v.cn[0], apply[i].v.cn[1], apply[i].v.cn[2], 0);
        }
    }
    for (i = 0; i < vtxCount; i++) {
        distX = camX - apply[i].v.ob[0];
        distY = camY - apply[i].v.ob[1];
        distZ = camZ - apply[i].v.ob[2];
        multiplier = distX * distX + distY * distY + distZ * distZ;
        multiplier -= near;
        if (multiplier < 0) {
            apply[i].v.cn[0] = ((apply[i].v.flag & 0xf800) >> 8);
            apply[i].v.cn[1] = ((apply[i].v.flag & 0x7c0) >> 3);
            apply[i].v.cn[2] = ((apply[i].v.flag & 0x3e) << 2);
            apply[i].v.cn[3] = 256 * alphamultiply;
        } else if (multiplier > far) {
            apply[i].v.cn[0] = R;
            apply[i].v.cn[1] = G;
            apply[i].v.cn[2] = B;
            apply[i].v.cn[3] = 0;
            continue;
        } else {
            multiplier = multiplier / (far >> 8);
            apply[i].v.cn[0] =
                (((apply[i].v.flag & 0xf800) >> 8) * (256 - multiplier) + R * multiplier) >> 8;
            apply[i].v.cn[1] =
                (((apply[i].v.flag & 0x7c0) >> 3) * (256 - multiplier) + G * multiplier) >> 8;
            apply[i].v.cn[2] =
                (((apply[i].v.flag & 0x3e) << 2) * (256 - multiplier) + B * multiplier) >> 8;
            apply[i].v.cn[3] = (256 - multiplier) * (256 - multiplier) / 256 * alphamultiply;
        }
    }
}

#include "levels/wf/header.h"/*
Vtx *CPUFogged[] = { &wf_dl_ZZZZZZmaingeometry_mesh_vtx_0, &wf_dl_aDomes_mesh_vtx_0,
                     &wf_dl_aDomes_mesh_vtx_1, &wf_dl_aDomes_mesh_vtx_2, &wf_dl_aDomes_mesh_vtx_3 };
u16 cpuFogVerts[] = { sizeof(wf_dl_ZZZZZZmaingeometry_mesh_vtx_0) / 0x10,
                      sizeof(wf_dl_aDomes_mesh_vtx_0) / 0x10, sizeof(wf_dl_aDomes_mesh_vtx_1) / 0x10,
                      sizeof(wf_dl_aDomes_mesh_vtx_2) / 0x10, sizeof(wf_dl_aDomes_mesh_vtx_3) / 0x10 };
u8 cpuFogCount = sizeof(CPUFogged) / 4;*/
extern u8 renderedDome;
#define MAXDIST 10000.f
extern Gfx mat_wf_dl_SCROLLWATER_layer1_area1[];
extern void renderDome(int ID);
void bhv_air_rocks(void) {
    Gfx *lights1 = segmented_to_virtual(wf_dl_domeinside1_SOLIDLIGHTScpucull_mesh);
    Gfx *lights2 = segmented_to_virtual(wf_dl_domeinside2_SOLIDLIGHTScpucull_mesh);
    Gfx *scrollMaterial = segmented_to_virtual(mat_wf_dl_SCROLLWATER_layer1_area1);
    Gfx *switchoutp = switchout;
    Vtx *b = segmented_to_virtual(wf_dl_ZZZZZZmaingeometry_mesh_vtx_0);
    int cpufog;
    int old = 0;
    struct Object *a;
    if (o->oTimer >= (SPAWNTIME + (random_u16() & 7))) {
        if (!(o->oBehParams >> 24) || (renderedDome == (o->oBehParams >> 24))) {
            if (o->oDistanceToMario < MAXDIST) {
                a = spawn_object_relative(0, random_f32_around_zero(RANGE), -80,
                                          random_f32_around_zero(RANGE), o, MODEL_BUBBLE,
                                          bhvWaterAirBubble);
                a->oBehParams = o->oBehParams;
            }
        }
        o->oTimer = 0;
    }
    // control C2 stuff
    if (o->oBehParams2ndByte) {
        openDoors &= 3;
        if (openDoors & 1) {
            lights1[4] = switchoutp[0];
        }
        if (openDoors & 2) {
            lights2[4] = switchoutp[0];
        }
// reads and edits gsDPSetTileSize
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLCMDINDEX 23
        /* old = (scrollMaterial[SCROLLCMDINDEX].words.w0 & SCROLLBITFLAG) >> 12;
         scrollMaterial[SCROLLCMDINDEX].words.w0 =
             ((scrollMaterial[SCROLLCMDINDEX].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old += 2, 12,
         12));*/

        /*for (cpufog = 0; cpufog < cpuFogCount; cpufog++) {
            if (cpufog) {
                CpuFog(0x10, 0x18, 0x45, 50 * 25, 750 * 25, segmented_to_virtual(CPUFogged[cpufog]),
                       cpuFogVerts[cpufog], 0.6f);
            } else {
                CpuFog(0x10, 0x18, 0x45, 50 * 25, 750 * 25, segmented_to_virtual(CPUFogged[cpufog]),
                       cpuFogVerts[cpufog], 0.6f);
            }
        }*/
        if (!o->oOpacity) {
            o->oOpacity = 1;
            if (sSourceWarpNodeId == 0x0B) {
                renderDome(4);
            } else {
                renderDome(8);
            }
        }
    }
}

void bhv_doorswitch(void) {
}

extern u8 sObjectCutscene;
extern u8 gRecentCutscene;
extern f32 obj_find_floor_height(struct Object *obj);
#define MINANGLE 0x0000
#define RANGE 0xA000/*
void bhv_rock_in_water(void) {
    switch (o->oAction) {
        case 0:
            // waitmariokick
            o->oAngleVelPitch += 0x0020;
            if (o->oFaceAngleRoll > 0) {
                o->oAngleVelPitch -= o->oFaceAngleRoll / 8;
            }
            if ((((u16) (o->oAngleToMario)) > MINANGLE)
                && (((u16) (o->oAngleToMario)) < (MINANGLE + RANGE))) {
                if (o->oDistanceToMario < 500.f) {
                    if ((m->forwardVel > 10.0f) || (m->action == ACT_WATER_SHELL_SWIMMING)) {
                        // cutscene
                        cutscene_object(CUTSCENE_STAR_SPAWN, o);
                        set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
                        o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                        o->oAction++;
                        o->oMoveAngleYaw += 0x4000;
                        o->oAngleVelPitch = 0;
                        cur_obj_nearest_object_with_behavior(bhvDomeDoor)->oFaceAnglePitch = 0x3000;
                    }
                }
            }
            break;
        case 1:
            if (o->oTimer < 12) {
                o->oAngleVelPitch -= 0x0020;
                o->oForwardVel = 5.0f;
            } else {
                o->oGraphYOffset += 5.f;
            }
            // o->oGraphYOffset = (f32)(((u16)(o->oFaceAngleRoll))/0x200);
            o->oForwardVel *= 0.98f;
            cur_obj_move_standard(-78);
            o->oPosY = obj_find_floor_height(o);
            if (o->oTimer > 80) {
                o->oAngleVelPitch = 0;
                o->oAction++;
                cur_obj_nearest_object_with_behavior(bhvAirrocks)->activeFlags = 0;
                cur_obj_nearest_object_with_behavior(bhvDomeDoor)->oAction = 1;
                cur_obj_nearest_object_with_behavior(bhvDomeDoor)->oTimer = 0;
                cur_obj_nearest_object_with_behavior(bhvDomeDoor)->oFaceAnglePitch = 0x3000;
            }
            // fall
            break;
        case 2:
            gObjCutsceneDone = TRUE;
            clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
            o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
            sObjectCutscene = 0;
            gRecentCutscene = 0;
            o->oAction = 3;
            break;
        case 3:
            break;
    }
    o->oFaceAngleRoll += o->oAngleVelPitch;
}*/

#define WRONG 0x4800;
void bhv_dome_door(void) {
    switch (o->oAction) {
        case 0:
            o->oFaceAnglePitch -= WRONG;
            // waitmariokick
            o->oAngleVelPitch += 0x0020 /*+ (random_u16() & 0x001f)*/;
            if (o->oFaceAnglePitch > 0) {
                o->oAngleVelPitch -= o->oFaceAnglePitch / 8;
            }
            o->oFaceAnglePitch += o->oAngleVelPitch;
            o->oFaceAnglePitch += WRONG;
            break;
        case 1:
#define MOMENT 7
            if (o->oTimer == MOMENT) {
                // cutscene_object(CUTSCENE_STAR_SPAWN, o);
                start_object_cutscene(CUTSCENE_STAR_SPAWN, o);
                set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
                o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
                play_puzzle_jingle();
            }
            if (o->oTimer == MOMENT + 20) {
                cur_obj_play_sound_2(SOUND_GENERAL_CLOSE_IRON_DOOR);
            }
            if (o->oTimer > MOMENT) {
                o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0x100);
                o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 0, 0x100);
            }
            if (o->oTimer > (MOMENT + 50)) {
                o->oAction++;
            }
            break;
        case 2:
            gObjCutsceneDone = TRUE;
            clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
            o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
            o->oAction = 3;
            break;
        case 3:
            o->oFaceAnglePitch = 0;
            o->oAngleVelPitch = 0;
            break;
    }
}

void breakinblock(void){
    load_object_collision_model();
}