#include <ultra64.h>

#include "sm64.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "mario.h"
#include "audio/external.h"
#include "game_init.h"
#include "interaction.h"
#include "mario_step.h"
#include "engine/math_util.h"
#include "engine/surface_load.h"
#include "object_helpers.h"
#include "OPT_FOR_SIZE.h"

static s16 sMovingSandSpeeds[] = { 12, 8, 4, 0 };

struct Surface gWaterSurfacePseudoFloor = {
    TERRAIN(2, 2, 0, 0, SPECFLAG_MEGASLIP, 0x0013),
    0,
    0,
    0,
    0,
    0,
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0.0f, 1.0f, 0.0f },
    0.0f,
    NULL,
};

void transfer_bully_speed(struct BullyCollisionData *obj1, struct BullyCollisionData *obj2) {
    f32 rx = obj2->posX - obj1->posX;
    f32 rz = obj2->posZ - obj1->posZ;

    //! Bully NaN crash
    f32 projectedV1 = (rx * obj1->velX + rz * obj1->velZ) / (rx * rx + rz * rz);
    f32 projectedV2 = (-rx * obj2->velX - rz * obj2->velZ) / (rx * rx + rz * rz);

    // Kill speed along r. Convert one object's speed along r and transfer it to
    // the other object.
    obj2->velX += obj2->conversionRatio * projectedV1 * rx - projectedV2 * -rx;
    obj2->velZ += obj2->conversionRatio * projectedV1 * rz - projectedV2 * -rz;

    obj1->velX += -projectedV1 * rx + obj1->conversionRatio * projectedV2 * -rx;
    obj1->velZ += -projectedV1 * rz + obj1->conversionRatio * projectedV2 * -rz;

    //! Bully battery
}

void init_bully_collision_data(struct BullyCollisionData *data, f32 posX, f32 posZ, f32 forwardVel,
                               s16 yaw, f32 conversionRatio, f32 radius) {
    if (forwardVel < 0.0f) {
        forwardVel *= -1.0f;
        yaw += 0x8000;
    }

    data->radius = radius;
    data->conversionRatio = conversionRatio;
    data->posX = posX;
    data->posZ = posZ;
    data->velX = forwardVel * sins(yaw);
    data->velZ = forwardVel * coss(yaw);
}

void mario_bonk_reflection(struct MarioState *m, u32 negateSpeed) {
    if (m->wall != NULL) {
        s16 wallAngle = atan2s(m->wall->normal.z, m->wall->normal.x);
        m->faceAngle[1] = wallAngle - (s16) (m->faceAngle[1] - wallAngle);

        play_sound(SOUND_ACTION_BONK, m->marioObj->header.gfx.cameraToObject);
    } else {
        play_sound(SOUND_ACTION_HIT, m->marioObj->header.gfx.cameraToObject);
    }

    if (negateSpeed) {
        mario_set_forward_vel(m, -m->forwardVel);
    } else {
        m->faceAngle[1] += 0x8000;
    }
}

u32 mario_update_quicksand(struct MarioState *m, f32 sinkingSpeed) {
    if (m->action & ACT_FLAG_RIDING_SHELL) {
        m->quicksandDepth = 0.0f;
    } else {
        if (m->quicksandDepth < 1.1f) {
            m->quicksandDepth = 1.1f;
        }

        if ((SURFACETYPE(m->floor) == SURFACE_INSTANT_QUICKSAND)
            || (SURFACETYPE(m->floor) == SURFACE_INSTANT_MOVING_QUICKSAND)) {
            return drop_and_set_mario_action(m, ACT_QUICKSAND_DEATH, 0);
        } else if (m->floor->type & (SPECFLAG_SINKSAND << 8)) {
            if ((m->quicksandDepth += sinkingSpeed) >= 25.0f) {
                m->quicksandDepth = 25.0f;
            }
        } else {
            m->quicksandDepth = 0.0f;
        }
    }

    return 0;
}

u32 mario_push_off_steep_floor(struct MarioState *m, u32 action, u32 actionArg) {
    s16 floorDYaw = m->floorAngle - m->faceAngle[1];

    if (floorDYaw > -0x4000 && floorDYaw < 0x4000) {
        m->forwardVel = 16.0f;
        m->faceAngle[1] = m->floorAngle;
    } else {
        m->forwardVel = -16.0f;
        m->faceAngle[1] = m->floorAngle + 0x8000;
    }

    return set_mario_action(m, action, actionArg);
}

u32 mario_update_moving_sand(struct MarioState *m) {
    struct Surface *floor = m->floor;
    s32 floorType = SURFACETYPE(floor);

    if (floorType == SURFACE_DEEP_MOVING_QUICKSAND || floorType == SURFACE_SHALLOW_MOVING_QUICKSAND
        || floorType == SURFACE_MOVING_QUICKSAND || floorType == SURFACE_INSTANT_MOVING_QUICKSAND) {
        s16 pushAngle = floor->force << 8;
        f32 pushSpeed = sMovingSandSpeeds[floor->force >> 8];

        m->vel[0] += pushSpeed * sins(pushAngle);
        m->vel[2] += pushSpeed * coss(pushAngle);

        return 1;
    }

    return 0;
}

void stop_and_set_height_to_floor(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    mario_set_forward_vel(m, 0.0f);
    m->vel[1] = 0.0f;

    //! This is responsible for some downwarps.
    m->pos[1] = m->floorHeight;

    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
}

s32 stationary_ground_step(struct MarioState *m) {
    u32 takeStep;
    struct Object *marioObj = m->marioObj;
    u32 stepResult = GROUND_STEP_NONE;

    mario_set_forward_vel(m, 0.0f);
    m->vel[1] = 0.0f;

    takeStep = mario_update_moving_sand(m);
    if (takeStep) {
        stepResult = perform_ground_step(m);
    } else {
        //! This is responsible for several stationary downwarps.
        m->pos[1] = m->floorHeight;

        vec3f_copy(marioObj->header.gfx.pos, m->pos);
        vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    }

    return stepResult;
}

#define CLIP_LENGTH (48.0f)
#define CLIP_Y (75.0f)

void clip_mario(Vec3f prev_pos, Vec3f new_pos, struct Surface **hit_surface, struct MarioState *m) {
    // CuckyDev: Rewrite clip_mario
    Vec3f unit_dir, dir;
    f32 clip_mag;
    Vec3f hit_pos;
    f32 clip;

    *hit_surface = NULL;

    // Get direction
    unit_dir[0] = new_pos[0] - prev_pos[0];
    unit_dir[1] = new_pos[1] - prev_pos[1];
    unit_dir[2] = new_pos[2] - prev_pos[2];
    clip_mag = vec3f_length(unit_dir);
    vec3f_normalize(unit_dir);

    if (clip_mag > 0.0f) {
        // Perform raycast
        dir[0] = unit_dir[0] * (clip_mag + CLIP_LENGTH);
        dir[1] = unit_dir[1] * (clip_mag + CLIP_LENGTH);
        dir[2] = unit_dir[2] * (clip_mag + CLIP_LENGTH);

        prev_pos[1] += CLIP_Y;
        find_surface_on_ray(prev_pos, dir, hit_surface, hit_pos, 7);

        // Clip if collision was found
        if (*hit_surface != NULL) {
            /*m->forwardVel = 0;
            m->vel[0] = 0;
            m->vel[1] = 0;
            m->vel[2] = 0;*/
            if ((*hit_surface)->normal.y < -WALLMAXNORMAL) {
                new_pos[0] = hit_pos[0] - unit_dir[0] * CLIP_LENGTH;
                if ((*hit_surface)->normal.y < -0.2f) // Dirty hack to prevent Mario from getting
                                                      // stuck on super steep ceilings
                    new_pos[1] = hit_pos[1] - unit_dir[1] * CLIP_LENGTH - CLIP_Y;
                new_pos[2] = hit_pos[2] - unit_dir[2] * CLIP_LENGTH;
            } else {
                hit_pos[0] = new_pos[0] - (*hit_surface)->vertex1[0];
                hit_pos[1] = new_pos[1] - (*hit_surface)->vertex1[1] + CLIP_Y;
                hit_pos[2] = new_pos[2] - (*hit_surface)->vertex1[2];
                clip = (hit_pos[0] * (*hit_surface)->normal.x + hit_pos[1] * (*hit_surface)->normal.y
                        + hit_pos[2] * (*hit_surface)->normal.z)
                       - CLIP_LENGTH;
                new_pos[0] -= (*hit_surface)->normal.x * clip;
                new_pos[1] -= (*hit_surface)->normal.y * clip;
                new_pos[2] -= (*hit_surface)->normal.z * clip;
            }
        }
    }
}
void doMarioWaterColl(f32 *waterLevel, struct MarioState *m) {
    struct Surface floor;
    f32 floorHeight;
    if (m->floor == &gWaterSurfacePseudoFloor) {
        *waterLevel = m->floorHeight;
        floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &floor);
        switch (floor.type) {
            case SURFACE_PAINTING_WARP_F9:
                if (*waterLevel < (floorHeight + 694.f + ICEFLOWERWALKOFFSTE)) {
                    *waterLevel = floorHeight + 694.f + ICEFLOWERWALKOFFSTE;
                }
                break;
            case SURFACE_PAINTING_WARP_F8:
                if (*waterLevel < (floorHeight + 253.f + ICEFLOWERWALKOFFSTE)) {
                    *waterLevel = floorHeight + 253.f + ICEFLOWERWALKOFFSTE;
                }
                break;
        }
        return;
    }
    if (m->floor) {
        switch (SURFACETYPE(m->floor)) {
            case SURFACE_PAINTING_WARP_F9:
                if (*waterLevel < (m->floorHeight + 694.f)) {
                    *waterLevel = m->floorHeight + 694.f;
                }
                break;
            case SURFACE_PAINTING_WARP_F8:
                if (*waterLevel < (m->floorHeight + 253.f)) {
                    *waterLevel = m->floorHeight + 253.f;
                }
                break;
        }
    }
}
__attribute__((always_inline))  inline static s32 perform_ground_quarter_step(struct MarioState *m, Vec3f nextPos) {
    // struct Surface *lowerWall;
    struct Surface *upperWall;
    struct Surface *ceil;
    struct Surface *floor;
    f32 ceilHeight;
    f32 floorHeight;
    f32 marioHeight = 160.f; // crawlspaces

    // lowerWall = resolve_and_return_wall_collisions(nextPos, 30.0f, 24.0f);
    upperWall = resolve_and_return_wall_collisions(nextPos, 60.0f, 50.0f);

    floorHeight = find_floor(nextPos[0], nextPos[1], nextPos[2], &floor);
    ceilHeight = vec3f_find_ceil(nextPos, nextPos[1], &ceil);

    m->wall = upperWall;

    if (floor == NULL) {
        return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
    }

    if (((m->action & ACT_FLAG_RIDING_SHELL) || (m->flags & MARIO_VANISH_CAP))
        && floorHeight < m->waterLevel) {
        floorHeight = m->waterLevel + ICEFLOWERWALKOFFSTE;
        floor = &gWaterSurfacePseudoFloor;
        floor->originOffset = m->waterLevel + ICEFLOWERWALKOFFSTE;
    }
    if (m->action & ACT_FLAG_SHORT_HITBOX) { // crawlspaces
        marioHeight = 80.f;
    }
    if (nextPos[1] > floorHeight + 100.0f) {
        if (nextPos[1] + marioHeight >= ceilHeight) { // crawlspaces
            return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
        }

        vec3f_copy(m->pos, nextPos);
        m->floor = floor;
        m->floorHeight = floorHeight;
        return GROUND_STEP_LEFT_GROUND;
    }

    if (floorHeight + marioHeight >= ceilHeight) { // crawlspaces
        return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
    }

    vec3f_set(m->pos, nextPos[0], floorHeight, nextPos[2]);
    m->floor = floor;
    m->floorHeight = floorHeight;
    if (SURFACETYPE(m->floor) == 0x00A7) {
        return GROUND_STEP_ON_RAIL;
    }
    if (upperWall != NULL) {
        s16 wallDYaw = atan2s(upperWall->normal.z, upperWall->normal.x) - m->faceAngle[1];

        if (wallDYaw >= 0x2AAA && wallDYaw <= 0x5555) {
            return GROUND_STEP_NONE;
        }
        if (wallDYaw <= -0x2AAA && wallDYaw >= -0x5555) {
            return GROUND_STEP_NONE;
        }

        return GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS;
    }

    return GROUND_STEP_NONE;
}
u32 TimeSinceLeftFloor;
extern f32 absf(f32 x);
s32 perform_ground_step(struct MarioState *m) {
    s32 i;
    u32 stepResult;
    Vec3f intendedPos;
    Vec3f priorPos;
    struct Surface *hit_surface;
    TimeSinceLeftFloor = 0;
    m->inertia[0] = 0;
    m->inertia[1] = 0;
    m->inertia[2] = 0;
    priorPos[0] = m->pos[0];
    priorPos[1] = m->pos[1];
    priorPos[2] = m->pos[2];

    // CuckyDev: Rewrite slope factor
    if ((absf(m->vel[0]) + absf(m->vel[2])) > 0.001f) {
        f32 mag = sqrtf(sqr(m->vel[2]) + sqr(m->vel[0])); // Get XZ magnitude (for division)
        f32 dot = m->vel[0] * m->floor->normal.x + m->vel[2] * m->floor->normal.z; // Get Y factor
        f32 dotd = dot / mag;
        f32 slopeFactor = m->floor->normal.y
                          / sqrtf(sqr(m->floor->normal.y) + sqr(dotd)); // Convert Y factor to XZ factor
        intendedPos[0] = m->pos[0] + (m->vel[0]) * slopeFactor;
        intendedPos[2] = m->pos[2] + (m->vel[2]) * slopeFactor;
        intendedPos[1] = m->pos[1] - dot; // CuckyDev: Move Mario up/down slopes as he runs on them
    } else {
        intendedPos[0] = m->pos[0];
        intendedPos[2] = m->pos[2];
        intendedPos[1] = m->pos[1];
    }

    stepResult = perform_ground_quarter_step(m, intendedPos);

    clip_mario(priorPos, m->pos, &hit_surface, m);

    // CuckyDev: Handle clip_mario walls
    if (stepResult == GROUND_STEP_NONE && hit_surface != NULL) {
        if (hit_surface->normal.y >= -WALLMAXNORMAL && hit_surface->normal.y <= WALLMAXNORMAL) {
            s16 wallDYaw = atan2s(hit_surface->normal.z, hit_surface->normal.x) - m->faceAngle[1];
            m->wall = hit_surface;

            if (wallDYaw >= 0x2AAA && wallDYaw <= 0x5555) {
                stepResult = GROUND_STEP_NONE;
            } else if (wallDYaw <= -0x2AAA && wallDYaw >= -0x5555) {
                stepResult = GROUND_STEP_NONE;
            } else {
                stepResult = GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS;
            }
        }
    }

    m->terrainSoundAddend = mario_get_terrain_sound_addend(m);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    if (stepResult == GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS) {
        stepResult = GROUND_STEP_HIT_WALL;
    }
    return stepResult;
}

u32 check_ledge_grab(struct MarioState *m, struct Surface *wall, Vec3f intendedPos, Vec3f nextPos) {
    struct Surface *ledgeFloor;
    Vec3f ledgePos;
    f32 displacementX;
    f32 displacementZ;

    if (m->vel[1] > 0) {
        return 0;
    }

    displacementX = nextPos[0] - intendedPos[0];
    displacementZ = nextPos[2] - intendedPos[2];

    // Only ledge grab if the wall displaced Mario in the opposite direction of
    // his velocity.
    if (displacementX * m->vel[0] + displacementZ * m->vel[2] > 0.0f) {
        return 0;
    }

    //! Since the search for floors starts at y + 160, we will sometimes grab
    // a higher ledge than expected (glitchy ledge grab)
    ledgePos[0] = nextPos[0] - wall->normal.x * 60.0f;
    ledgePos[2] = nextPos[2] - wall->normal.z * 60.0f;
    ledgePos[1] = find_floor(ledgePos[0], nextPos[1] + 120.0f, ledgePos[2], &ledgeFloor);

    if (ledgePos[1] - nextPos[1] <= 80.0f - m->vel[1]) { // ledgegrab fix
        return 0;
    }

    vec3f_copy(m->pos, ledgePos);
    m->floor = ledgeFloor;
    m->floorHeight = ledgePos[1];

    if (m->floor->normal.y < 0.9063078f) // CuckyDev: Fix fake ledge grabs
        return 0;

    m->floorAngle = atan2s(ledgeFloor->normal.z, ledgeFloor->normal.x);

    m->faceAngle[0] = 0;
    m->faceAngle[1] = atan2s(wall->normal.z, wall->normal.x) + 0x8000;
    return 1;
}

__attribute__((always_inline))  inline s32 perform_air_quarter_step(struct MarioState *m, Vec3f intendedPos, u32 stepArg) {
    s16 wallDYaw;
    Vec3f nextPos;
    struct Surface *lowerWall;
    struct Surface *ceil;
    struct Surface *floor;
    f32 ceilHeight;
    f32 floorHeight;
    f32 floorObjects;

    vec3f_copy(nextPos, intendedPos);

    lowerWall = resolve_and_return_wall_collisions(nextPos, 75.0f, 50.0f);

    floorHeight = find_floor_marioair(nextPos[0], nextPos[1], nextPos[2], &floor);
    ceilHeight = vec3f_find_ceil(nextPos, nextPos[1], &ceil);

    m->wall = NULL;

    if (((m->action & ACT_FLAG_RIDING_SHELL) || (m->flags & MARIO_VANISH_CAP))
        && floorHeight < m->waterLevel) {
        floorHeight = m->waterLevel + ICEFLOWERWALKOFFSTE;
        floor = &gWaterSurfacePseudoFloor;
        floor->originOffset = m->waterLevel + ICEFLOWERWALKOFFSTE;
    }
    //! The water pseudo floor is not referenced when your intended qstep is
    // out of bounds, so it won't detect you as landing.

    if (floor == NULL) {
        if (nextPos[1] <= m->floorHeight) {
            m->pos[1] = m->floorHeight;
            return AIR_STEP_LANDED;
        }

        m->pos[1] = nextPos[1];
        return AIR_STEP_HIT_WALL;
    }

    //! This check uses f32, but findFloor uses short (overflow jumps)
    if (nextPos[1] <= floorHeight) {
        if (ceilHeight - floorHeight > 160.0f) {
            m->pos[0] = nextPos[0];
            m->pos[2] = nextPos[2];
            m->floor = floor;
            m->floorHeight = floorHeight;
        }

        //! When ceilHeight - floorHeight <= 160, the step result says that
        // Mario landed, but his movement is cancelled and his referenced floor
        // isn't updated (pedro spots)
        m->pos[1] = floorHeight;
        return AIR_STEP_LANDED;
    }

    if (nextPos[1] + 160.0f > ceilHeight) {
        if (m->vel[1] >= 0.0f) {
            m->vel[1] = 0.0f;

            //! Uses referenced ceiling instead of ceil (ceiling hang upwarp)
            if ((stepArg & AIR_STEP_CHECK_HANG) && m->ceil != NULL
                && ((m->ceil->type & (SPECFLAG_HANGABLE << 8)))) {
                return AIR_STEP_GRABBED_CEILING;
            }

            return AIR_STEP_NONE;
        }

        //! Potential subframe downwarp->upwarp?
        if (nextPos[1] <= m->floorHeight) {
            m->pos[1] = m->floorHeight;
            return AIR_STEP_LANDED;
        }

        m->pos[1] = nextPos[1];
        return AIR_STEP_HIT_WALL;
    }
    //! When the wall is not completely vertical or there is a slight wall
    // misalignment, you can activate these conditions in unexpected situations
    if ((stepArg & AIR_STEP_CHECK_LEDGE_GRAB) && lowerWall != NULL) { // wallcuck
        if (check_ledge_grab(m, lowerWall, intendedPos, nextPos)) {
            return AIR_STEP_GRABBED_LEDGE;
        }
    }

    vec3f_copy(m->pos, nextPos);
    m->floor = floor;
    m->floorHeight = floorHeight;

    if (lowerWall) {
        m->wall = lowerWall;
        wallDYaw = atan2s(m->wall->normal.z, m->wall->normal.x) - m->faceAngle[1];

        if (m->wall->type & (SPECFLAG_BURNING << 8)) {
            return AIR_STEP_HIT_LAVA_WALL;
        }

        if (wallDYaw < -0x6000 || wallDYaw > 0x6000) {
            m->flags |= MARIO_UNKNOWN_30;
            return AIR_STEP_HIT_WALL;
        }
    }

    return AIR_STEP_NONE;
}

void apply_twirl_gravity(struct MarioState *m) {
    f32 terminalVelocity;
    f32 heaviness;

    if (m->angleVel[1] > 1024) {
        heaviness = 1024.0f / m->angleVel[1];
    } else {
        heaviness = 1.0f;
    }
    if (m->input & INPUT_Z_DOWN) {
        heaviness = 1.0f;
        m->particleFlags |= PARTICLE_DUST;
    }
    m->vel[1] -= 4.0f * heaviness;
    terminalVelocity = -75.0f * heaviness;
    if (m->vel[1] < terminalVelocity) {
        m->vel[1] = terminalVelocity;
    }
}

u32 should_strengthen_gravity_for_jump_ascent(struct MarioState *m) {
    if (!(m->flags & MARIO_UNKNOWN_08)) {
        return FALSE;
    }

    if (m->action & (ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)) {
        return FALSE;
    }

    if (!(m->input & INPUT_A_DOWN) && m->vel[1] > 20.0f) {
        return (m->action & ACT_FLAG_CONTROL_JUMP_HEIGHT) != 0;
    }

    return FALSE;
}

void apply_gravity(struct MarioState *m) {
    f32 terminalVel = -75.f;
    f32 gravity = -4.f;
    if (m->action == ACT_TWIRLING && m->vel[1] < 0.0f) {
        apply_twirl_gravity(m);
        return;
    } else if (m->action == ACT_SHOT_FROM_CANNON) {
        gravity =- 1.0f;
    } else if (m->action == ACT_LONG_JUMP || m->action == ACT_SLIDE_KICK
               || m->action == ACT_BBH_ENTER_SPIN) {
        gravity =- 2.0f;
    } else if (m->action == ACT_LAVA_BOOST || m->action == ACT_FALL_AFTER_STAR_GRAB) {
        gravity =- 3.2f;
        terminalVel = -65.0f;
    } else if (m->action == ACT_GETTING_BLOWN) {
        gravity =- m->unkC4;
    } else if (should_strengthen_gravity_for_jump_ascent(m)) {
        m->vel[1] /= 4.0f;
        return;
    } else if (m->action & ACT_FLAG_METAL_WATER) {
        gravity =- 1.6f;
        terminalVel = -16.0f;
    } else if ((m->flags & MARIO_WING_CAP) && m->vel[1] < 0.0f && (m->input & INPUT_A_DOWN)) {
        m->marioBodyState->wingFlutter = TRUE;
        gravity =- 2.0f;
        terminalVel = -37.5f;
    } else if (m->action == ACT_WALL_SLIDE) {
        if (m->wall) {
            switch (mario_get_floor_class(m->wall)) {
                case SURFACE_CLASS_NOT_SLIPPERY:
                    gravity = -0.5f;
                    terminalVel = -10.0f;
                    break;
                case SURFACE_CLASS_SLIPPERY:
                    gravity =- 2.0f;
                    terminalVel = -40.0f;
                    break;
                case SURFACE_CLASS_VERY_SLIPPERY:
                    gravity =- 3.0f;
                    break;
                default:
                    gravity =- 1.0f;
                    terminalVel = -20.0f;
                    break;
            }
        }
    }
    m->vel[1] += gravity;
    if (m->vel[1] < terminalVel) {
        m->vel[1] = terminalVel;
    }
}

void apply_vertical_wind(struct MarioState *m) {
    f32 maxVelY;
    f32 offsetY;

    if (m->action != ACT_GROUND_POUND) {
        offsetY = m->pos[1] - -1500.0f;

        if (SURFACETYPE(m->floor) == SURFACE_VERTICAL_WIND && -3000.0f < offsetY && offsetY < 2000.0f) {
            if (offsetY >= 0.0f) {
                maxVelY = 10000.0f / (offsetY + 200.0f);
            } else {
                maxVelY = 50.0f;
            }

            if (m->vel[1] < maxVelY) {
                if ((m->vel[1] += maxVelY / 8.0f) > maxVelY) {
                    m->vel[1] = maxVelY;
                }
            }
        }
    }
}
extern u32 TimeSinceLeftFloor;
extern u8 firstframe;
s32 perform_air_step(struct MarioState *m, u32 stepArg) {
    Vec3f intendedPos;
    s32 j;
    s32 stepResult;
    Vec3f priorPos;
    struct Surface *hit_surface;
    TimeSinceLeftFloor++;
    priorPos[0] = m->pos[0];
    priorPos[1] = m->pos[1];
    priorPos[2] = m->pos[2];

    m->wall = NULL;

    for (j = 0; j < 3; j++) {
        intendedPos[j] = m->pos[j] + (m->vel[j]);
        if (!firstframe) {
            intendedPos[j] += m->inertia[j];
        }
    }
    firstframe = 0;

    stepResult = perform_air_quarter_step(m, intendedPos, stepArg);



    clip_mario(priorPos, m->pos, &hit_surface, m);

    // CuckyDev: Handle clip_mario walls
    if (stepResult == AIR_STEP_NONE && hit_surface != NULL) {
        if (hit_surface->normal.y >= -WALLMAXNORMAL && hit_surface->normal.y <= WALLMAXNORMAL) {
            s16 wallDYaw = atan2s(hit_surface->normal.z, hit_surface->normal.x) - m->faceAngle[1];
            m->wall = hit_surface;

            if (m->wall->type & (SPECFLAG_BURNING << 8)) {
                stepResult = AIR_STEP_HIT_LAVA_WALL;
            } else if (wallDYaw < -0x6000 || wallDYaw > 0x6000) {
                m->flags |= MARIO_UNKNOWN_30;
                stepResult = AIR_STEP_HIT_WALL;
            }
        } else if (hit_surface->normal.y
                   > 0.f) { // mario hit a wall, annihilate his speed components to fix bug
            m->vel[0] -= hit_surface->normal.x * m->vel[0];
            m->vel[1] -= hit_surface->normal.y * m->vel[1];
            m->vel[2] -= hit_surface->normal.z * m->vel[2];
        }
    }

    if (m->vel[1] >= 0.0f) {
        m->peakHeight = m->pos[1];
    }

    m->terrainSoundAddend = mario_get_terrain_sound_addend(m);

    if (!(stepArg & AIR_STEP_NO_GRAVITY)) // CuckyDev: For warping
    {
        if (m->action != ACT_FLYING) {
            apply_gravity(m);
        }
        apply_vertical_wind(m);
    }

    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    return stepResult;
}

// They had these functions the whole time and never used them? Lol

void set_vel_from_pitch_and_yaw(struct MarioState *m) {
    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[1] = m->forwardVel * sins(m->faceAngle[0]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);
}

void set_vel_from_yaw(struct MarioState *m) {
    m->vel[0] = m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);
}
