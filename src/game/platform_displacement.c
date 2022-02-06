#include <PR/ultratypes.h>

#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "level_update.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "types.h"
#include "OPT_FOR_SIZE.h"

/**
 * Determine if Mario is standing on a platform object, meaning that he is
 * within 4 units of the floor. Set his referenced platform object accordingly.
 */
void update_mario_platform(void) {
    if (gMarioObject) {
        if ((gMarioState->floor->object)
            && (absf(gMarioObject->oPosY - gMarioState->floorHeight) < 4.0f)) {
            gMarioObject->platform = gMarioState->floor->object;
        } else {
            gMarioObject->platform = NULL;
        }
    }
}

u8 firstframe = 0;
/**
 * Apply one frame of platform rotation to Mario or an object using the given
 * platform. If isMario is 0, use gCurrentObject.
 */
void apply_platform_displacement(u32 isMario, struct Object *platform) {
    f32 x;
    f32 y;
    f32 z;
    f32 platformPosX;
    f32 platformPosY;
    f32 platformPosZ;
    Vec3f currentObjectOffset;
    Vec3f relativeOffset;
    Vec3f newObjectOffset;
    Vec3s rotation;
    f32 displaceMatrix[4][4];

    rotation[0] = platform->oAngleVelPitch;
    rotation[1] = platform->oAngleVelYaw;
    rotation[2] = platform->oAngleVelRoll;

    if (isMario) {
        x = gMarioStates[0].pos[0];
        y = gMarioStates[0].pos[1];
        z = gMarioStates[0].pos[2];
    } else {
        x = gCurrentObject->oPosX;
        y = gCurrentObject->oPosY;
        z = gCurrentObject->oPosZ;
    }

    x += platform->oVelX;
    z += platform->oVelZ;

    if (rotation[0] != 0 || rotation[1] != 0 || rotation[2] != 0) {

        if (isMario) {
            gMarioStates[0].faceAngle[1] += rotation[1];
        }

        platformPosX = platform->oPosX;
        platformPosY = platform->oPosY;
        platformPosZ = platform->oPosZ;

        currentObjectOffset[0] = x - platformPosX;
        currentObjectOffset[1] = y - platformPosY;
        currentObjectOffset[2] = z - platformPosZ;

        rotation[0] = platform->oFaceAnglePitch - platform->oAngleVelPitch;
        rotation[1] = platform->oFaceAngleYaw - platform->oAngleVelYaw;
        rotation[2] = platform->oFaceAngleRoll - platform->oAngleVelRoll;

        mtxf_rotate_zxy_and_translate(displaceMatrix, currentObjectOffset, rotation);
        linear_mtxf_transpose_mul_vec3f(displaceMatrix, relativeOffset, currentObjectOffset);

        rotation[0] = platform->oFaceAnglePitch;
        rotation[1] = platform->oFaceAngleYaw;
        rotation[2] = platform->oFaceAngleRoll;

        mtxf_rotate_zxy_and_translate(displaceMatrix, currentObjectOffset, rotation);
        linear_mtxf_mul_vec3f(displaceMatrix, newObjectOffset, relativeOffset);

        x = platformPosX + newObjectOffset[0];
        y = platformPosY + newObjectOffset[1];
        z = platformPosZ + newObjectOffset[2];
    }
    if (isMario) {
        gMarioState->inertia[0] = x - gMarioState->pos[0];
        gMarioState->inertia[1] = y - gMarioState->pos[1];
        gMarioState->inertia[2] = z - gMarioState->pos[2];
        if (gMarioState->inertia[1] < 0) {
            gMarioState->inertia[1] = 0;
        }
        firstframe = 1;
        gMarioStates[0].pos[0] = x;
        gMarioStates[0].pos[1] = y;
        gMarioStates[0].pos[2] = z;
    } else {
        gCurrentObject->oPosX = x;
        gCurrentObject->oPosY = y;
        gCurrentObject->oPosZ = z;
    }
}

/**
 * If Mario's platform is not null, apply platform displacement.
 */
void apply_mario_platform_displacement(void) {
    struct Object *platform;
    if (gMarioObject) {
        platform = gMarioObject->platform;
        if (!platform) {
            if (gMarioState->wall) {
                platform = gMarioState->wall->object;
            }
        }
        if (!(gTimeStopState & TIME_STOP_ACTIVE) && gMarioObject != NULL && platform != NULL) {
            apply_platform_displacement(1, platform);
        }
    }
}
