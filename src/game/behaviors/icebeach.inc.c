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
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject

void icecube(void) {
    obj_scale_xyz(o, 1.f + ((s8) (o->oBehParams >> 24)) / 100.f,
                  1.f + ((s8) ((o->oBehParams >> 16) & 0xFF)) / 100.f,
                  1.f + ((s8) ((o->oBehParams >> 8) & 0xFF)) / 100.f);
    if (cur_obj_is_mario_on_platform()) {
        o->oVelY -= 1.0f;
    }
    if (cur_obj_is_mario_ground_pounding_platform()) {
        o->oVelY -= 4.0f;
    }
    o->oVelY += (o->oHomeY - o->oPosY) / 30.f;
    o->oVelY += sins(o->oTimer * 0x200) * 0.25f;
    o->oVelY *= 0.90f;
    o->oPosY += o->oVelY;
    load_object_collision_model();
    if (!o->oOpacity) {
        o->oOpacity = 1;
        o->oCollisionDistance += 800.f;
    }
}

void whale(void) {
    o->oVelY += (o->oHomeY - o->oPosY) / 30.f;
    o->oVelY += sins(o->oTimer * 0x200) * 0.25f;
    o->oVelY *= 0.90f;
    o->oPosY += o->oVelY;
    if (cur_obj_is_mario_ground_pounding_platform()) {
        cur_obj_play_sound_2(SOUND_OBJ_DORRIEWHALE);
    }
    switch (o->oAction) {
        case 0:
            if (!o->oTimer) {
                o->oOpacity = (random_u16() & 0x3F) + 20;
            }
            if (o->oDistanceToMario < 7000.f) {
                if (o->oTimer > o->oOpacity) {
                    o->oAction++;
                    o->oHiddenBlueCoinSwitch =
                        spawn_object_relative(0, 0, 600.f, 236.f, o, 0x37, bhvWaterSpout);
                    obj_scale_xyz(o->oHiddenBlueCoinSwitch, .5f, 0.05, .5f);
                }
            }
            break;
        case 1:
            if (o->oHiddenBlueCoinSwitch->header.gfx.scale[0] < 0.05f) {
                obj_mark_for_deletion(o->oHiddenBlueCoinSwitch);
                o->oAction = 0;
            }
            break;
    }
    load_object_collision_model();
}

void waterspout(void) {
    if (((s8 *) o)[0xF0]++ % 4 == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_WHALEFALL); // //SOUND_OBJ_DIVING_INTO_WATER
    }
    switch (o->oAction) {
        case 0:
            o->header.gfx.scale[0] = approach_f32_symmetric(o->header.gfx.scale[0], 1.f, 0.03f);
            o->header.gfx.scale[1] = approach_f32_symmetric(o->header.gfx.scale[1], 1.5f, 0.05f);
            o->header.gfx.scale[2] = approach_f32_symmetric(o->header.gfx.scale[2], 1.f, 0.03f);
            if (o->header.gfx.scale[1] > 1.45f) {
                o->oAction++;
                o->oOpacity = (random_u16() & 0x3F) + 60;
            }
            break;
        case 1:
            o->header.gfx.scale[1] += sins(o->oTimer * 0x400) * .02f;
            if (o->oTimer > o->oOpacity) {
                o->oAction++;
            }
            break;
        case 2:
            o->header.gfx.scale[0] = approach_f32_symmetric(o->header.gfx.scale[0], 0.02f, 0.05f);
            o->header.gfx.scale[1] = approach_f32_symmetric(o->header.gfx.scale[1], 0.02f, 0.1f);
            o->header.gfx.scale[2] = approach_f32_symmetric(o->header.gfx.scale[2], 0.02f, 0.05f);
            break;
    }
    o->oCollisionDistance = 3000.f;
    load_object_collision_model();
}

#define CLIP_Y (75.0f)
void initClip(Vec3f prev_pos, struct Object *this) {
    vec3f_copy(prev_pos, &this->oPosX);
}
u32 clipObject(Vec3f prev_pos, struct Object *this) {
    Vec3f unit_dir, dir;
    f32 clip_mag;
    Vec3f hit_pos;
    f32 clip;
    struct Surface *hit_surface = NULL;
#define CLIP_LENGTH 24.f
    unit_dir[0] = this->oPosX - prev_pos[0];
    unit_dir[1] = this->oPosY - prev_pos[1];
    unit_dir[2] = this->oPosZ - prev_pos[2];
    clip_mag = vec3f_length(unit_dir);
    vec3f_normalize(unit_dir);

    if (clip_mag > 0.0f) {
        dir[0] = unit_dir[0] * (clip_mag + CLIP_LENGTH);
        dir[1] = unit_dir[1] * (clip_mag + CLIP_LENGTH);
        dir[2] = unit_dir[2] * (clip_mag + CLIP_LENGTH);
        prev_pos[1] += CLIP_Y;
        find_surface_on_ray(prev_pos, dir, &hit_surface, hit_pos, 3);
        if (hit_surface != NULL) {
            if ((hit_surface)->normal.y < -WALLMAXNORMAL) {
                this->oPosX = hit_pos[0] - unit_dir[0] * CLIP_LENGTH;
                if ((hit_surface)->normal.y
                    < -0.2f) // Dirty hack to prevent Mario from getting stuck on super steep ceilings
                    this->oPosY = hit_pos[1] - unit_dir[1] * CLIP_LENGTH - CLIP_Y;
                this->oPosZ = hit_pos[2] - unit_dir[2] * CLIP_LENGTH;
            } else {
                hit_pos[0] = this->oPosX - (hit_surface)->vertex1[0];
                hit_pos[1] = this->oPosY - (hit_surface)->vertex1[1] + CLIP_Y;
                hit_pos[2] = this->oPosZ - (hit_surface)->vertex1[2];
                clip = (hit_pos[0] * (hit_surface)->normal.x + hit_pos[1] * (hit_surface)->normal.y
                        + hit_pos[2] * (hit_surface)->normal.z)
                       - CLIP_LENGTH;
                this->oPosX -= (hit_surface)->normal.x * clip;
                this->oPosY -= (hit_surface)->normal.y * clip;
                this->oPosZ -= (hit_surface)->normal.z * clip;
            }
        }
    }

    vec3f_copy(prev_pos, &this->oPosX);
    return hit_surface;
}

extern f32 calcLandTime(f32 curY, f32 goalY, f32 gravity, f32 startspeed);
void distrustfish(void) {
    f32 latDist = lateral_dist_between_objects(o, gMarioState->marioObj);
    f32 ydist = gMarioState->pos[1] - o->oPosY;
    f32 frameCount;
    f32 waterL;
    f32 dist;
    s16 targetAngle;
    struct Object *sp38;
    switch (o->oAction) {
        case 0:
            cur_obj_init_animation(0);
            o->oIntangibleTimer = 0;
            if (o->oPosY < (waterL = find_water_level(o->oPosX, o->oPosZ))) {
                o->oPosY = approach_f32_symmetric(o->oPosY, waterL, 4.f);
                o->oVelY = approach_s16_symmetric(o->oVelY, 0, 5.f);
            }
            o->oForwardVel = approach_f32_symmetric(o->oForwardVel, 0, 1.f);
            o->oPosX = approach_s16_symmetric(o->oPosX, o->oHomeX, 2.f);
            o->oPosZ = approach_s16_symmetric(o->oPosZ, o->oHomeZ, 2.f);
            // wait and stare at mario, intensely, approach home position symmetric
            o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, 0, 0x200);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);
            if (o->oTimer > 40) {
                if (absf(gMarioState->pos[1] - gMarioState->floorHeight) < 50.f) {
#define LUNGERANGE 3700.f
                    o->oOpacity++;
                    if (o->oOpacity > 60) {
                        if (latDist < LUNGERANGE - ydist * ydist / 1000.f) {
                            o->oAction = 1;
                            o->oVelY = -10.f;
                            o->oOpacity = 0;
                            initClip(&o->oBobombBuddyPosXCopy, o);
                        }
                    }
                }
            }
            break;
        case 1:
            // charge jump
            cur_obj_init_animation(1);
            if (o->oTimer > 2) {
                o->oAction = 2;
                if (ydist > 0) {
                    o->oVelY = -.5f + sqrtf(.25f + (ydist + 350.f) * 4);
                } else {
                    o->oAction = 0;
                }
                o->oForwardVel = latDist / calcLandTime(o->oPosY, gMarioState->pos[1], -2.f, o->oVelY)
                                 + 2.f * ydist / 1000.f;
                cur_obj_set_model(0x39);
                cur_obj_play_sound_2(SOUND_OBJ_DIVING_INTO_WATER);
                sp38 = spawn_object(o, MODEL_WATER_SPLASH, bhvWaterSplash);
                obj_scale(sp38, 3.0f);
            }
            break;
        case 2:
            // in jump
            if (o->oTimer == 1) {

                cur_obj_play_sound_2(SOUND_OBJ2_EYEROK_SOUND_SHORT);
            }
            o->oFaceAnglePitch = atan2s(o->oVelY, o->oForwardVel);
            if (o->oSubAction) {
                o->oAction++;
                o->oFaceAnglePitch = 0x4000;
            }
            clipObject(&o->oBobombBuddyPosXCopy, o);
            break;
        case 3:
            // flop around helplessly, move towards home pos, reset to 0 when in water
            clipObject(&o->oBobombBuddyPosXCopy, o);
            cur_obj_init_animation(2);
            cur_obj_set_model(0x38);
            o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch,
                                                        atan2s(o->oVelY, o->oForwardVel + 40.f), 0x400);
            o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 3.f, 0.1f);
            targetAngle = atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX);
            o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetAngle, 0x240);
            o->oForwardVel = approach_f32(
                o->oForwardVel, 3.f * (coss(targetAngle - o->oMoveAngleYaw) + 1.0f), 0.2f, 1.f);
            if (o->oPosY < (waterL = find_water_level(o->oPosX, o->oPosZ))) {
                o->oPosY = waterL;
                o->oAction = 0;
                cur_obj_play_sound_2(SOUND_OBJ_DIVING_INTO_WATER);
                sp38 = spawn_object(o, MODEL_WATER_SPLASH, bhvWaterSplash);
                obj_scale(sp38, 3.0f);
            }
            break;
        case 5:
            // die by GPd on
            if (o->header.gfx.unk38.animFrame) {
                o->header.gfx.unk38.animFrame--;
            }
            o->header.gfx.scale[2] = approach_f32_symmetric(o->header.gfx.scale[2], 0.1f, 0.15f);
            o->header.gfx.scale[0] = approach_f32_symmetric(o->header.gfx.scale[0], 2.f, 0.15f);
            o->header.gfx.scale[1] = approach_f32_symmetric(o->header.gfx.scale[1], 2.f, 0.15f);
            if (o->oTimer > 10) {
                spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
                create_sound_spawner(SOUND_OBJ_DYING_ENEMY2);
                obj_mark_for_deletion(o);
                spawn_mist_particles_variable(0, 0, 46.f);
            }
            o->oIntangibleTimer = -1;
            o->oInteractStatus = 0;
            break;
    }
    if (o->oInteractStatus & (INT_STATUS_INTERACTED) && (gMarioState->action == ACT_GROUND_POUND)) {
        o->oAction = 5;
        cur_obj_play_sound_2(SOUND_OBJ_STOMPED);
    } else {
        o->oInteractStatus = 0;
    }
    cur_obj_update_floor_height();
    cur_obj_resolve_wall_coll_simple(0.f, 200.f);
        obj_move_xyz_using_fvel_and_yaw();
    if (o->oPosY < o->oFloorHeight) {
        o->oPosY = o->oFloorHeight;
        o->oVelY = 0.f;
        o->oSubAction = 1;
        if (o->oAction == 3) {
            o->oVelY = 20.f;
            cur_obj_play_sound_2(SOUND_OBJ_WALKING_WATER);
        }
    }
    o->oVelY += o->oGravity;
}