#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "envfx_bubbles.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "levels/bob/header.h"
#include "levels/ttm/header.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "mario_misc.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "rendering_graph_node.h"
#include "OPT_FOR_SIZE.h"
/**
 * @file obj_behaviors.c
 * This file contains a portion of the obj behaviors and many helper functions for those
 * specific behaviors. Few functions besides the bhv_ functions are used elsewhere in the repo.
 */

#define o gCurrentObject

#define OBJ_COL_FLAG_GROUNDED (1 << 0)
#define OBJ_COL_FLAG_HIT_WALL (1 << 1)
#define OBJ_COL_FLAG_UNDERWATER (1 << 2)
#define OBJ_COL_FLAG_NO_Y_VEL (1 << 3)
#define OBJ_COL_FLAGS_LANDED (OBJ_COL_FLAG_GROUNDED | OBJ_COL_FLAG_NO_Y_VEL)

extern void *inside_castle_seg7_trajectory_mips;
/**
 * Turns an object away from floors/walls that it runs into.
 */
void turn_obj_away_from_surface(f32 velX, f32 velZ, f32 nX, UNUSED f32 nY, f32 nZ, f32 *objYawX,
                                f32 *objYawZ) {
    *objYawX =
        (nZ * nZ - nX * nX) * velX / (nX * nX + nZ * nZ) - 2 * velZ * (nX * nZ) / (nX * nX + nZ * nZ);

    *objYawZ =
        (nX * nX - nZ * nZ) * velZ / (nX * nX + nZ * nZ) - 2 * velX * (nX * nZ) / (nX * nX + nZ * nZ);
}

/**
 * Finds any wall collisions, applies them, and turns away from the surface.
 */
s32 obj_find_wall(f32 objNewX, f32 objY, f32 objNewZ, f32 objVelX, f32 objVelZ) {
    struct WallCollisionData hitbox;
    f32 wall_nX, wall_nY, wall_nZ, objVelXCopy, objVelZCopy, objYawX, objYawZ;

    hitbox.x = objNewX;
    hitbox.y = objY;
    hitbox.z = objNewZ;
    hitbox.offsetY = o->hitboxHeight / 2;
    hitbox.radius = o->hitboxRadius;

    if (find_wall_collisions(&hitbox)) {
        o->oPosX = hitbox.x;
        o->oPosY = hitbox.y;
        o->oPosZ = hitbox.z;

        wall_nX = hitbox.walls[0]->normal.x;
        wall_nY = hitbox.walls[0]->normal.y;
        wall_nZ = hitbox.walls[0]->normal.z;

        objVelXCopy = objVelX;
        objVelZCopy = objVelZ;

        // Turns away from the first wall only.
        turn_obj_away_from_surface(objVelXCopy, objVelZCopy, wall_nX, wall_nY, wall_nZ, &objYawX,
                                   &objYawZ);

        o->oMoveAngleYaw = atan2s(objYawZ, objYawX);
        return FALSE;
    }

    return TRUE;
}

/**
 * Turns an object away from steep floors, similarly to walls.
 */
s32 turn_obj_away_from_steep_floor(struct Surface *objFloor, f32 floorY, f32 objVelX, f32 objVelZ) {
    f32 floor_nX, floor_nY, floor_nZ, objVelXCopy, objVelZCopy, objYawX, objYawZ;

    if (!objFloor) {
        o->oMoveAngleYaw += 0x8000;
        return FALSE;
    }

    floor_nX = objFloor->normal.x;
    floor_nY = objFloor->normal.y;
    floor_nZ = objFloor->normal.z;

    // If the floor is steep and we are below it (i.e. walking into it), turn away from the floor.
    if (floor_nY < 0.5 && floorY > o->oPosY) {
        objVelXCopy = objVelX;
        objVelZCopy = objVelZ;
        turn_obj_away_from_surface(objVelXCopy, objVelZCopy, floor_nX, floor_nY, floor_nZ, &objYawX,
                                   &objYawZ);
        o->oMoveAngleYaw = atan2s(objYawZ, objYawX);
        return FALSE;
    }

    return TRUE;
}

/**
 * Determines an object's forward speed multiplier.
 */
void calc_obj_friction(f32 *objFriction, f32 floor_nY) {
    if (floor_nY < 0.2 && o->oFriction < 0.9999) {
        *objFriction = 0;
    } else {
        *objFriction = o->oFriction;
    }
}

/**
 * Updates an objects speed for gravity and updates Y position.
 */
void calc_new_obj_vel_and_pos_y(struct Surface *objFloor, f32 objFloorY, f32 objVelX, f32 objVelZ) {
    f32 floor_nX = objFloor->normal.x;
    f32 floor_nY = objFloor->normal.y;
    f32 floor_nZ = objFloor->normal.z;
    f32 objFriction;

    // Caps vertical speed with a "terminal velocity".
    o->oVelY -= o->oGravity;
    if (o->oVelY < -75.0) {
        o->oVelY = -75.0;
    }

    o->oPosY += o->oVelY;

    // Snap the object up to the floor.
    if (o->oPosY < objFloorY) {
        o->oPosY = objFloorY;

        // Bounces an object if the ground is hit fast enough.
        if (o->oVelY < -17.5) {
            o->oVelY = -(o->oVelY / 2);
        } else {
            o->oVelY = 0;
        }
    }

    //! (Obj Position Crash) If you got an object with height past 2^31, the game would crash.
    if ((s32) o->oPosY >= (s32) objFloorY && (s32) o->oPosY < (s32) objFloorY + 37) {

        // Adds horizontal component of gravity for horizontal speed.
        objVelX += floor_nX * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * o->oGravity
                   * 2;
        objVelZ += floor_nZ * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * o->oGravity
                   * 2;

        if (objVelX < 0.000001 && objVelX > -0.000001) {
            objVelX = 0;
        }
        if (objVelZ < 0.000001 && objVelZ > -0.000001) {
            objVelZ = 0;
        }

        if (objVelX != 0 || objVelZ != 0) {
            o->oMoveAngleYaw = atan2s(objVelZ, objVelX);
        }

        calc_obj_friction(&objFriction, floor_nY);
        o->oForwardVel = sqrtf(objVelX * objVelX + objVelZ * objVelZ) * objFriction;
    }
}

void calc_new_obj_vel_and_pos_y_underwater(struct Surface *objFloor, f32 floorY, f32 objVelX,
                                           f32 objVelZ, f32 waterY) {
    f32 floor_nX = objFloor->normal.x;
    f32 floor_nY = objFloor->normal.y;
    f32 floor_nZ = objFloor->normal.z;

    f32 netYAccel = (1.0f - o->oBuoyancy) * (-1.0f * o->oGravity);
    o->oVelY -= netYAccel;

    if (o->oVelY < -75.0) {
        o->oVelY = -75.0;
    }

    o->oPosY += o->oVelY;

    // Snap the object up to the floor.
    if (o->oPosY < floorY) {
        o->oPosY = floorY;

        // Bounces an object if the ground is hit fast enough.
        if (o->oVelY < -17.5) {
            o->oVelY = -(o->oVelY / 2);
        } else {
            o->oVelY = 0;
        }
    }

    // If moving fast near the surface of the water, flip vertical speed? To emulate skipping?
    if (o->oForwardVel > 12.5 && (waterY + 30.0f) > o->oPosY && (waterY - 30.0f) < o->oPosY) {
        o->oVelY = -o->oVelY;
    }

    if ((s32) o->oPosY >= (s32) floorY && (s32) o->oPosY < (s32) floorY + 37) {

        // Adds horizontal component of gravity for horizontal speed.
        objVelX += floor_nX * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * netYAccel * 2;
        objVelZ += floor_nZ * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * netYAccel * 2;
    }

    if (objVelX < 0.000001 && objVelX > -0.000001) {
        objVelX = 0;
    }
    if (objVelZ < 0.000001 && objVelZ > -0.000001) {
        objVelZ = 0;
    }

    if (o->oVelY < 0.000001 && o->oVelY > -0.000001) {
        o->oVelY = 0;
    }

    if (objVelX != 0 || objVelZ != 0) {
        o->oMoveAngleYaw = atan2s(objVelZ, objVelX);
    }

    // Decreases both vertical velocity and forward velocity. Likely so that skips above
    // don't loop infinitely.
    o->oForwardVel = sqrtf(objVelX * objVelX + objVelZ * objVelZ) * 0.8;
    o->oVelY *= 0.8;
}

/**
 * Generates splashes if at surface of water, entering water, or bubbles
 * if underwater.
 */
void obj_splash(s32 waterY, s32 objY) {
    u32 globalTimer = gGlobalTimer;

    // Spawns waves if near surface of water and plays a noise if entering.
    if ((f32) (waterY + 30) > o->oPosY && o->oPosY > (f32) (waterY - 30)) {
        spawn_object(o, MODEL_IDLE_WATER_WAVE, bhvObjectWaterWave);

        if (o->oVelY < -20.0f) {
            cur_obj_play_sound_2(SOUND_OBJ_DIVING_INTO_WATER);
        }
    }

    // Spawns bubbles if underwater.
    if ((objY + 50) < waterY && (globalTimer & 0x1F) == 0) {
        spawn_object(o, MODEL_WHITE_PARTICLE_SMALL, bhvObjectBubble);
    }
}

/**
 * Generic object move function. Handles walls, water, floors, and gravity.
 * Returns flags for certain interactions.
 */
s16 object_step(void) {
    f32 objX = o->oPosX;
    f32 objY = o->oPosY;
    f32 objZ = o->oPosZ;

    f32 floorY;
    f32 waterY = -10000.0;

    f32 objVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    f32 objVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);

    s16 collisionFlags = 0;

    // Find any wall collisions, receive the push, and set the flag.
    if (obj_find_wall(objX + objVelX, objY, objZ + objVelZ, objVelX, objVelZ) == 0) {
        collisionFlags += OBJ_COL_FLAG_HIT_WALL;
    }

    floorY = find_floor(objX + objVelX, objY, objZ + objVelZ, &o->oFloorPointer);
    if (turn_obj_away_from_steep_floor(o->oFloorPointer, floorY, objVelX, objVelZ)) {
        waterY = find_water_level(objX + objVelX, objZ + objVelZ);
        if (o->oFloorPointer) {
            switch (SURFACETYPE(o->oFloorPointer)) {
                case SURFACE_PAINTING_WARP_F9:
                    waterY = floorY + 694.f;
                    break;
                case SURFACE_PAINTING_WARP_F8:
                    waterY = floorY + 253.f;
                    break;
            }
        }
        if (waterY > objY) {
            calc_new_obj_vel_and_pos_y_underwater(o->oFloorPointer, floorY, objVelX, objVelZ, waterY);
            collisionFlags += OBJ_COL_FLAG_UNDERWATER;
        } else {
            calc_new_obj_vel_and_pos_y(o->oFloorPointer, floorY, objVelX, objVelZ);
        }
    } else {
        // Treat any awkward floors similar to a wall.
        collisionFlags += ((collisionFlags & OBJ_COL_FLAG_HIT_WALL) ^ OBJ_COL_FLAG_HIT_WALL);
    }

    o->oPosX += o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oPosZ += o->oForwardVel * coss(o->oMoveAngleYaw);
    if ((s32) o->oPosY == (s32) floorY) {
        collisionFlags += OBJ_COL_FLAG_GROUNDED;
    }

    if ((s32) o->oVelY == 0) {
        collisionFlags += OBJ_COL_FLAG_NO_Y_VEL;
    }

    // Generate a splash if in water.
    obj_splash((s32) waterY, (s32) o->oPosY);
    return collisionFlags;
}

/**
 * Uses an object's forward velocity and yaw to move its X, Y, and Z positions.
 * This does accept an object as an argument, though it is always called with `o`.
 * If it wasn't called with `o`, it would modify `o`'s X and Z velocities based on
 * `obj`'s forward velocity and yaw instead of `o`'s, and wouldn't update `o`'s
 * position.
 */
void obj_move_xyz_using_fvel_and_yaw() {
    o->oVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);

    o->oPosX += o->oVelX;
    o->oPosY += o->oVelY;
    o->oPosZ += o->oVelZ;
}

s32 is_point_close_to_object(struct Object *obj, f32 x, f32 y, f32 z, s32 dist) {
    f32 objX = obj->oPosX;
    f32 objY = obj->oPosY;
    f32 objZ = obj->oPosZ;

    if ((x - objX) * (x - objX) + (y - objY) * (y - objY) + (z - objZ) * (z - objZ)
        < (f32) (dist * dist)) {
        return TRUE;
    }

    return FALSE;
}
s32 is_point_within_radius_of_mario(f32 x, f32 y, f32 z, s32 dist) {
    return (is_point_close_to_object(gMarioState->marioObj, x, y, z, dist));
}

/**
 * Sets an object as visible if within a certain distance of Mario's graphical position.
 */
void set_object_visibility(struct Object *obj, s32 dist) {

    if (is_point_within_radius_of_mario(obj->oPosX, obj->oPosY, obj->oPosZ, dist)) {
        obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    } else {
        obj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
    }
    // obj->oDrawingDistance = dist;
}

/**
 * Turns an object towards home if Mario is not near to it.
 */
s32 obj_return_home_if_safe(struct Object *obj, f32 homeX, f32 y, f32 homeZ, s32 dist) {
    f32 homeDistX = homeX - obj->oPosX;
    f32 homeDistZ = homeZ - obj->oPosZ;
    s16 angleTowardsHome = atan2s(homeDistZ, homeDistX);

    if (is_point_within_radius_of_mario(homeX, y, homeZ, dist) ) {
        return TRUE;
    } else {
        obj->oMoveAngleYaw = approach_s16_symmetric(obj->oMoveAngleYaw, angleTowardsHome, 320);
    }

    return FALSE;
}


/**
 * Spawns a number of coins at the location of an object
 * with a random forward velocity, y velocity, and direction.
 */
void obj_spawn_yellow_coins(struct Object *obj, s8 nCoins) {
    struct Object *coin;
    s8 count;
    for (; nCoins>0; nCoins--) {
        coin = spawn_object(obj, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
        coin->oForwardVel = random_float() * 20;
        coin->oVelY = random_float() * 40 + 20;
        coin->oMoveAngleYaw = random_u16();
    }
}

/**
 * Controls whether certain objects should flicker/when to despawn.
 */
s32 obj_flicker_and_disappear(struct Object *obj, s16 lifeSpan) {
    if (obj->oTimer < lifeSpan) {
        return FALSE;
    }

    if (obj->oTimer < lifeSpan + 40) {
        if (obj->oTimer % 2 != 0) {
            obj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        } else {
            obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    } else {
        obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return TRUE;
    }

    return FALSE;
}

/**
 *Checks if a floor is one that should cause an object to "die".
 */
void obj_check_floor_death(s16 collisionFlags, struct Surface *floor) {
    if (floor == NULL) {
        return;
    }

    if ((collisionFlags & OBJ_COL_FLAG_GROUNDED) == 1) {
        if (floor->type & (SPECFLAG_BURNING << 8)) {
            o->oAction = OBJ_ACT_LAVA_DEATH;
        } else if (SURFACETYPE(floor) == 0x000A) {
            o->oAction = OBJ_ACT_DEATH_PLANE_DEATH;
        }
    }
}

/**
 * Controls an object dying in lava by creating smoke, sinking the object, playing
 * audio, and eventually despawning it. Returns TRUE when the obj is dead.
 */
s32 obj_lava_death(void) {
    struct Object *deathSmoke;

    if (o->oTimer >= 31) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return TRUE;
    } else {
        // Sinking effect
        o->oPosY -= 10.0f;
    }

    if ((o->oTimer % 8) == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_BULLY_EXPLODE_2);
        deathSmoke = spawn_object(o, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
        deathSmoke->oPosX += random_float() * 20.0f;
        deathSmoke->oPosY += random_float() * 20.0f;
        deathSmoke->oPosZ += random_float() * 20.0f;
        deathSmoke->oForwardVel = random_float() * 10.0f;
    }

    return FALSE;
}

/**
 * Spawns an orange number object relatively, such as those that count up for secrets.
 */
void spawn_orange_number(s8 behParam, s16 relX, s16 relY, s16 relZ) {
    struct Object *orangeNumber;

    if (behParam >= 10) {
        return;
    }

    orangeNumber = spawn_object_relative(behParam, relX, relY, relZ, o, MODEL_NUMBER, bhvOrangeNumber);
    orangeNumber->oPosY += 25.0f;
}

#include "behaviors/moving_coin.inc.c"
#include "behaviors/seaweed.inc.c"
#include "behaviors/bobomb.inc.c"
#include "behaviors/cannon_door.inc.c"
#include "behaviors/whirlpool.inc.c"
#include "behaviors/amp.inc.c"
#include "behaviors/butterfly.inc.c"
#include "behaviors/hoot.inc.c"
#include "behaviors/beta_holdable_object.inc.c"
#include "behaviors/bubble.inc.c"
#include "behaviors/water_wave.inc.c"
#include "behaviors/explosion.inc.c"
#include "behaviors/corkbox.inc.c"
#include "behaviors/bully.inc.c"
#include "behaviors/water_ring.inc.c"
#include "behaviors/bowser_bomb.inc.c"
#include "behaviors/celebration_star.inc.c"
#include "behaviors/drawbridge.inc.c"
#include "behaviors/bomp.inc.c"
#include "behaviors/sliding_platform.inc.c"
#include "behaviors/moneybag.inc.c"
#include "behaviors/bowling_ball.inc.c"
#include "behaviors/cruiser.inc.c"
#include "behaviors/spindel.inc.c"
#include "behaviors/pyramid_wall.inc.c"
#include "behaviors/pyramid_elevator.inc.c"
#include "behaviors/pyramid_top.inc.c"
#include "behaviors/sound_waterfall.inc.c"
#include "behaviors/sound_volcano.inc.c"
#include "behaviors/castle_flag.inc.c"
#include "behaviors/sound_birds.inc.c"
#include "behaviors/sound_ambient.inc.c"
#include "behaviors/sound_sand.inc.c"
#include "behaviors/castle_cannon_grate.inc.c"
#include "behaviors/snowman.inc.c"
#include "behaviors/boulder.inc.c"
#include "behaviors/cap.inc.c"
#include "behaviors/spawn_star.inc.c"
#include "behaviors/red_coin.inc.c"
#include "behaviors/hidden_star.inc.c"
#include "behaviors/rolling_log.inc.c"
#include "behaviors/mushroom_1up.inc.c"
#include "behaviors/controllable_platform.inc.c"
#include "behaviors/breakable_box_small.inc.c"
#include "behaviors/snow_mound.inc.c"
#include "behaviors/floating_platform.inc.c"
#include "behaviors/arrow_lift.inc.c"
#include "behaviors/orange_number.inc.c"
#include "behaviors/manta_ray.inc.c"
#include "behaviors/falling_pillar.inc.c"
#include "behaviors/floating_box.inc.c"
#include "behaviors/decorative_pendulum.inc.c"
#include "behaviors/treasure_chest.inc.c"
#include "behaviors/mips.inc.c"
#include "behaviors/yoshi.inc.c"
