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

extern const Trajectory vcutm_area_1_spline_ship_path_001[];
void airCruisers(void) {
    f32 x, z, y;
    f32 dist;
    x = ((s16 *) segmented_to_virtual(
        vcutm_area_1_spline_ship_path_001))[1 + (o->oBehParams2ndByte * 4)];
    z = ((s16 *) segmented_to_virtual(
        vcutm_area_1_spline_ship_path_001))[3 + (o->oBehParams2ndByte * 4)];
    y = ((s16 *) segmented_to_virtual(
        vcutm_area_1_spline_ship_path_001))[2 + (o->oBehParams2ndByte * 4)];
    dist = sqrtf((x - o->oPosX) * (x - o->oPosX) + (z - o->oPosZ) * (z - o->oPosZ));
    o->oAngleVelYaw = approach_s16_symmetric(
        o->oAngleVelYaw,
        -192.f * (abs_angle_diff(atan2s(z - o->oPosZ, x - o->oPosX), o->oMoveAngleYaw) / 16384.f),
        0x000C);
    o->oMoveAngleYaw += o->oAngleVelYaw;

    if (dist < 700.f) {
        o->oBehParams2ndByte++;
        if (o->oBehParams2ndByte == 10) {
            o->oBehParams2ndByte = 0;
        }
    }
    o->oVelY = approach_f32_symmetric(o->oVelY, (y - o->oPosY) / (dist / o->oForwardVel), 1.0f);
    o->oVelY *= 0.99f;
    o->oForwardVel = 18.0f;
        obj_move_xyz_using_fvel_and_yaw();
    load_object_collision_model();
}

void safetyCloud(void) {
    f32 framesTillThere = 30.f;
    switch (o->oAction) {
        case 0:
            cur_obj_hide();
            o->oVelY = 0.f;
            o->oForwardVel = 0.f;
            o->oPosX = 0.f;
            o->oPosZ = 0.f;
            o->oPosY = -5000.f;
            if (gMarioState->floor && (SURFACETYPE(gMarioState->floor) == SURFACE_DEATH_PLANE)) {
                if (m->pos[1] < m->floorHeight + 3072.0f) {
                    o->oAction = 1;
                    o->oPosX = gMarioState->pos[0];
                    o->oPosZ = gMarioState->pos[2];
                    o->oPosY = gMarioState->pos[1] - 1000.f;
                    spawn_mist_particles_variable(0, -20, 200.0f);
                }
            }
            break;
        case 1:
            cur_obj_unhide();
            o->oPosX = gMarioState->pos[0];
            o->oPosZ = gMarioState->pos[2];
            o->oVelY += 2.f;
            if (cur_obj_is_mario_on_platform()) {
                o->oAction++;
                drop_and_set_mario_action(m, ACT_FEET_STUCK_IN_GROUND, 0);
            }
            break;
        case 2:
#define FRAMECOUNT 100
#define PHASE 25
            framesTillThere = FRAMECOUNT - o->oTimer;
            o->oMoveAngleYaw = atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX);
            if (o->oTimer < PHASE) {
                o->oVelY += 1.f;
            }
            if (o->oTimer > (FRAMECOUNT - PHASE)) {
                o->oVelY -= 1.f;
            }
            o->oForwardVel = approach_f32_symmetric(
                o->oForwardVel, cur_obj_lateral_dist_to_home() / framesTillThere, 4.5f);
            if (o->oTimer == FRAMECOUNT) {
                o->oAction++;
                gMarioState->forwardVel = 0;
                gMarioState->vel[1] = 0;
                gMarioState->vel[0] = 0;
                gMarioState->vel[2] = 0;
                gMarioState->inertia[1] = 0;
                gMarioState->inertia[0] = 0;
                gMarioState->inertia[2] = 0;
                gMarioState->action = ACT_FREEFALL;
            }
        obj_move_xyz_using_fvel_and_yaw();
            gMarioState->pos[0] = o->oPosX;
            gMarioState->pos[2] = o->oPosZ;
            gMarioState->pos[1] = o->oPosY;
            vec3f_copy(gMarioState->marioObj->header.gfx.pos, m->pos);
            break;
        case 3:
            gMarioState->inertia[1] = 0;
            gMarioState->inertia[0] = 0;
            gMarioState->inertia[2] = 0;
            spawn_mist_particles_variable(0, -20, 200.0f);
            o->oAction = 0;
            break;
    }
    if ((o->oAction != 3) && (o->oAction)) {
        load_object_collision_model();
    }
}
