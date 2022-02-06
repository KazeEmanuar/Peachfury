#include <PR/ultratypes.h>

#include "behavior_data.h"
#include "debug.h"
#include "engine/behavior_script.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "main.h"
#include "object_constants.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "print.h"
#include "sm64.h"
#include "types.h"
#include "engine/math_util.h"
#include "OPT_FOR_SIZE.h"

#define DEBUG_INFO_NOFLAGS (0 << 0)
#define DEBUG_INFO_FLAG_DPRINT (1 << 0)
#define DEBUG_INFO_FLAG_LSELECT (1 << 1)
#define DEBUG_INFO_FLAG_ALL 0xFF

extern struct MarioState *gMarioState;
extern Lights1 backupLights1;
extern Lights1 backupLights2;
extern Lights1 backupLights3;
extern Lights1 mario_body_lights;
extern Lights1 mario_cap_lights;
extern Lights1 mario_blue_lights;
extern Lights1 mario_red_lights;
extern Lights1 mario_shoes_lights;

Lights1 cloudcapLights1 = gdSPDefLights1(0x7F, 0x7F, 0x7F, 0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);
Lights1 cloudcapLights2 = gdSPDefLights1(0x7F, 0x0, 0x0, 0xFE, 0x0, 0x0, 0x28, 0x28, 0x28);

Lights1 cloudcapLights12 = gdSPDefLights1(0x7F, 0x7F, 0x7F, 0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);
Lights1 cloudcapLights22 = gdSPDefLights1(0x7F, 0x0, 0x0, 0xFE, 0x0, 0x0, 0x28, 0x28, 0x28);

Lights1 icemarioLights1 = gdSPDefLights1(0x7F, 0x0, 0x0, 0xFE, 0x0, 0x0, 0x28, 0x28, 0x28);
Lights1 icemarioLights2 = gdSPDefLights1(0x00, 0x5f, 0xbf, 0x00, 0x7f, 0xFE, 0x28, 0x28, 0x28);
extern Gfx cloudcap_cloud_mario_mesh[];
extern Gfx mario_000_displaylist_mesh_layer_1_tri_1[];
extern Lights1 cloudcap_M_Logo_lights;

Gfx cloudcap = gsSPDisplayList(cloudcap_cloud_mario_mesh);
Gfx back = gsSPEndDisplayList();

extern Gfx backupHat1;
extern Gfx backupHat2;

u8 cloudcount;

void checkCloudSpawn() {
    struct Object *a;
    if (gMarioState->action & ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION) {
        if (!(gMarioState->action & ACT_FLAG_INVULNERABLE)) {
            if (gMarioState->controller->buttonPressed & A_BUTTON) {
                if ((find_floor_height(gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2])
                     - gMarioState->pos[1])
                    < -72.f) {
                    if ((gMarioState->action != ACT_WALL_KICK_AIR) || (gMarioState->actionTimer > 1)) {
                        if ((gMarioState->action != ACT_FORWARD_ROLLOUT)
                            && (gMarioState->action != ACT_FLYING_TRIPLE_JUMP)) {
                            if (cloudcount > 0) {
                                a = spawn_object(gMarioObject, 0xf3, bhvCloudSpawn);
                                gMarioState->vel[1] = 30.f;
                                gMarioState->forwardVel = 0;
                                gMarioState->inertia[0] = 0;
                                gMarioState->inertia[1] = 0;
                                gMarioState->inertia[2] = 0;
                                set_mario_action(gMarioState, ACT_FORWARD_ROLLOUT, 0);
                                a->oFaceAnglePitch = 0;
                                a->oFaceAngleRoll = 0;
                                a->oPosX += sins(gMarioState->faceAngle[1]) * 175.f;
                                a->oPosZ += coss(gMarioState->faceAngle[1]) * 175.f;
                                cloudcount--;
                                switch (cloudcount) {
                                    case 0:
                                        gMarioState->flags &= ~MARIO_METAL_CAP;
                                        obj_mark_for_deletion(gMarioObject->oHiddenBlueCoinSwitch);
                                        break;
                                    case 1:
                                        obj_mark_for_deletion(
                                            gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch);
                                        break;
                                    case 2:
                                        obj_mark_for_deletion(
                                            gMarioObject->oHiddenBlueCoinSwitch->oHiddenBlueCoinSwitch
                                                ->oHiddenBlueCoinSwitch);
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void checkCloudCount(void) {
    struct Object *o;
    int count = 0;
    uintptr_t *behaviorAddr = segmented_to_virtual(bhvCloudFollow);
    struct Object *closestObj = NULL;
    struct Object *obj;
    struct ObjectNode *listHead;
    listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
    obj = (struct Object *) listHead->next;
    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                count++;
            }
        }
        obj = (struct Object *) obj->header.next;
    }
    if (cloudcount > count) {
        o = spawn_object(gMarioObject, 0xf4, bhvCloudFollow);
        o->oHiddenBlueCoinSwitch = gMarioObject->oHiddenBlueCoinSwitch;
        gMarioObject->oHiddenBlueCoinSwitch = o;
    }
}

f32 find_water_height(f32 x, f32 y, f32 z) {
    f32 waterLevel = find_water_level(x, z);
    struct Surface *floor;
    f32 floorHeight = find_floor(x, y, z, &floor);
    if (floor) {
        switch (SURFACETYPE(floor)) {
            case SURFACE_PAINTING_WARP_F9:
                if (waterLevel < (floorHeight + 694.f)) {
                    waterLevel = floorHeight + 694.f;
                }
                break;
            case SURFACE_PAINTING_WARP_F8:
                if (waterLevel < (floorHeight + 253.f)) {
                    waterLevel = floorHeight + 253.f;
                }
                break;
        }
    }
    return waterLevel;
}
// round to 120 degrees
// offset 866 coordinates
// 1000 on Z axis
// 866 on X axis
void spawnQuad(f32 x, f32 y, f32 z) {
    struct Object *t = spawn_object(gMarioObject, 0xF5, bhvIceQuad);
    t->oPosY = y;
    t->oPosX = x;
    t->oPosZ = z;
    t->oFaceAngleYaw = 0;
    t->oFaceAnglePitch = 0;
    t->oFaceAngleRoll = 0;
    t->header.gfx.pos[0] = x;
    t->header.gfx.pos[1] = t->oPosY;
    t->header.gfx.pos[2] = z;
    t->header.gfx.angle[0] = 0;
    t->header.gfx.angle[1] = 0;
    t->header.gfx.angle[2] = 0;
    obj_scale(t, 0.0f);
}

void spawnQuadHoriz() {
    struct Object *t;
    s16 angle;
    if (!cur_obj_nearest_object_with_behavior(bhvIceQuadHoriz)) {
        angle = atan2s(gMarioState->wall->normal.z, gMarioState->wall->normal.x) - 0x8000;
        t = spawn_object(gMarioObject, 0xF5, bhvIceQuadHoriz);
        t->oPosY = gMarioState->pos[1] + 35.f;
        t->oPosX = gMarioState->pos[0] + sins(gMarioState->faceAngle[1]) * 50.f;
        t->oPosZ = gMarioState->pos[2] + coss(gMarioState->faceAngle[1]) * 50.f;
        t->oFaceAngleYaw = gMarioState->faceAngle[1];
        t->oFaceAnglePitch = 0xC000;
        t->oFaceAngleRoll = 0;
        t->header.gfx.pos[0] = t->oPosX;
        t->header.gfx.pos[1] = t->oPosY;
        t->header.gfx.pos[2] = t->oPosZ;
        t->header.gfx.angle[0] = 0xC000;
        t->header.gfx.angle[1] = gMarioState->faceAngle[1];
        t->header.gfx.angle[2] = 0;
        obj_scale(t, 0.0f);
    }
}

#define WATERDOWNOFFSET 155.f
extern Vtx icequad_Circle_mesh_layer_7_vtx_0[7];
u8 iceState = 1;
void checkIcePlatforms() {
    f32 x, y, z;
    struct Object *quad;
    f32 waterLevel;
    f32 multiplied = 1.f;
    f32 waterDown = 0.f;
f32 LFX, LFZ;
    f32 xDiff, zDiff;
    f32 zadd, xadd;
    u8 i;
    u8 spawnAnyway;
    x = gMarioState->pos[0];
    z = gMarioState->pos[2];
    waterLevel = find_water_height(
        gMarioState->pos[0] + sins(gMarioState->faceAngle[1]) * gMarioState->forwardVel * multiplied,
        gMarioState->pos[1],
        gMarioState->pos[2] + coss(gMarioState->faceAngle[1]) * gMarioState->forwardVel * multiplied);
    spawnAnyway = ((gMarioState->pos[1] == gMarioState->floorHeight)
                   && SURFACETYPE(gMarioState->floor) == 0x00AD);
    if (spawnAnyway
        || ((waterLevel + WATERDOWNOFFSET + waterDown - gMarioState->vel[1]) > gMarioState->pos[1])) {
        x = gMarioState->pos[0]
            + sins(gMarioState->faceAngle[1]) * gMarioState->forwardVel * multiplied;
        z = gMarioState->pos[2]
            + coss(gMarioState->faceAngle[1]) * gMarioState->forwardVel * multiplied;
        if ((gMarioState->pos[1] + gMarioState->vel[1]) < waterLevel) {
            gMarioState->pos[1] = waterLevel;
            gMarioState->vel[1] = 0;
        }
        if (!(quad = cur_obj_nearest_object_with_behavior(bhvIceQuad))) {
            spawnQuad(x, waterLevel, z);
            LFX = gMarioState->pos[0];
            LFZ = gMarioState->pos[2];
            iceState = 1;
        } else {
            quad->oPosX = gMarioState->pos[0];
            quad->oPosZ = gMarioState->pos[2];
            if (spawnAnyway) {
                quad->oPosY = gMarioState->floorHeight;
            }
            xDiff = (gMarioState->pos[0] - LFX) / (500.f / 2048.f);
            zDiff = (gMarioState->pos[2] - LFZ) / (500.f / 2048.f);
            // 500 ingame coordinates = 0x800 texture coords
            zadd = 0;
            xadd = 0;
            if (((Vtx *) segmented_to_virtual(icequad_Circle_mesh_layer_7_vtx_0))[i].v.tc[0] > 2048) {
                xadd = -2048.f;
            } else if (((Vtx *) segmented_to_virtual(icequad_Circle_mesh_layer_7_vtx_0))[i].v.tc[0]
                       < -2048) {
                xadd = 2048.f;
            }
            if (((Vtx *) segmented_to_virtual(icequad_Circle_mesh_layer_7_vtx_0))[i].v.tc[1] > 2048) {
                zadd = -2048.f;
            } else if (((Vtx *) segmented_to_virtual(icequad_Circle_mesh_layer_7_vtx_0))[i].v.tc[1]
                       < -2048) {
                zadd = 2048.f;
            }

            for (i = 0; i < 7; i++) {
                ((Vtx *) segmented_to_virtual(icequad_Circle_mesh_layer_7_vtx_0))[i].v.tc[0] +=
                    xDiff + xadd;
                ((Vtx *) segmented_to_virtual(icequad_Circle_mesh_layer_7_vtx_0))[i].v.tc[1] +=
                    zDiff + zadd;
            }
            LFX = gMarioState->pos[0];
            LFZ = gMarioState->pos[2];
            iceState = 1;
        }
    }
    if (gMarioState->wall && (SURFACETYPE(gMarioState->wall) == 0x00AD)) {
        spawnQuadHoriz();
    }
}

u16 currRotation;
void bombspritecode(void) {
    gCurrentObject->oPosX =
        gMarioState->pos[0]
        + sins(currRotation + gCurrentObject->oBehParams2ndByte * (0x10000 / 5)) * 120.f;
    gCurrentObject->oPosZ =
        gMarioState->pos[2]
        + coss(currRotation + gCurrentObject->oBehParams2ndByte * (0x10000 / 5)) * 120.f;
    gCurrentObject->oPosY = gMarioState->pos[1] + 50.f;
}

extern Gfx bombcap_HammerBros_mesh[];
Lights1 bombcapLights1 = gdSPDefLights1(0, 0, 0x10, 0, 0, 0x20, 0x28, 0x28, 0x28);
Lights1 bombcapLights2 = gdSPDefLights1(0, 0, 0x7f, 0, 0, 0xFE, 0x28, 0x28, 0x28);
Lights1 bombcapLights3 = gdSPDefLights1(0x7f, 0x6a, 0x00, 0xFE, 0xF0, 0, 0x28, 0x28, 0x28);
Gfx bombcap = gsSPDisplayList(bombcap_HammerBros_mesh);

extern struct Object *cur_obj_find_nearest_object_with_bParam2andBehav(const BehaviorScript *behavior,
                                                                       u32 Bparam2);
s8 powerupSystem2 = 0;
u8 bombsleft = 5;
extern f32 gDefaultSoundArgs[3];
inline void LIGHTS(void) {
    Gfx *k = (Gfx *) segmented_to_virtual(&mario_000_displaylist_mesh_layer_1_tri_1[0]);
    Lights1 *i = (Lights1 *) segmented_to_virtual(&mario_body_lights);
    Lights1 *j = (Lights1 *) segmented_to_virtual(&mario_cap_lights);
    Lights1 *i2 = (Lights1 *) segmented_to_virtual(&mario_blue_lights);
    Lights1 *j2 = (Lights1 *) segmented_to_virtual(&mario_red_lights);
    Lights1 *l = (Lights1 *) segmented_to_virtual(&mario_shoes_lights);
    struct MarioBodyState *bodyState = gMarioState->marioBodyState;
    struct Object *bomb;
    struct Object *sprite;

    if (powerupSystem2 & 2) {
        if (count_objects_with_behavior(bhvBombSprite) < bombsleft) {
            sprite = spawn_object(gCurrentObject, 0xF8, bhvBombSprite);
            sprite->oBehParams2ndByte = count_objects_with_behavior(bhvBombSprite);
        }
        currRotation += 0x200;
        *i = bombcapLights1;
        *j = bombcapLights2;
        *i2 = bombcapLights1;
        *j2 = bombcapLights2;
        *l = bombcapLights3;
        *k = bombcap;
        *(k + 1) = back;
        if ((gMarioState->action == ACT_MOVE_PUNCHING) || (gMarioState->action == ACT_PUNCHING)) {
            bomb = spawn_object(gMarioState->marioObj, MODEL_BLACK_BOBOMB, bhvBobomb);
            gMarioState->action = ACT_THROWING;
            bomb->oBehParams2ndByte = 0x77;
            bomb->oAction = 1;
            bomb->oPosX = gMarioState->pos[0] + sins(gMarioState->faceAngle[1]) * 40.f;
            bomb->oPosY = gMarioState->pos[1] + 80.f;
            bomb->oPosZ = gMarioState->pos[2] + coss(gMarioState->faceAngle[1]) * 40.f;
            bomb->oForwardVel = 25.f;
            bomb->oVelY = 15.f;
            bomb->oBehParams |= 1 << 8;
            if (cur_obj_find_nearest_object_with_bParam2andBehav(bhvBombSprite, bombsleft)) {
                cur_obj_find_nearest_object_with_bParam2andBehav(bhvBombSprite, bombsleft)
                    ->activeFlags = 0;
                bombsleft--;
            } else {
                cur_obj_nearest_object_with_behavior(bhvBombSprite)->activeFlags = 0;
                bombsleft--;
            }
        }
        if (!bombsleft) {
            powerupSystem2 &= ~2;
        }
    }

    if ((gMarioState->flags & (MARIO_METAL_CAP | MARIO_VANISH_CAP))
        == (MARIO_METAL_CAP | MARIO_VANISH_CAP)) {
        gMarioState->capTimer = 600;
        *i = cloudcapLights1;
        *j = cloudcapLights2;
        *i2 = cloudcapLights1;
        *j2 = cloudcapLights2;
        *k = cloudcap;
        *(k + 1) = back;
        checkCloudSpawn();
        checkCloudCount();

        bodyState->modelState |= MODEL_STATE_METAL;
        checkIcePlatforms();
    } else if (gMarioState->flags & MARIO_METAL_CAP) {
        *i = cloudcapLights1;
        *j = cloudcapLights2;
        *i2 = cloudcapLights1;
        *j2 = cloudcapLights2;
        *k = cloudcap;
        *(k + 1) = back;
        checkCloudSpawn();
        checkCloudCount();
    } else if (gMarioState->flags & MARIO_VANISH_CAP) {
        bodyState->modelState |= MODEL_STATE_METAL;
        checkIcePlatforms();
    } else if (!powerupSystem2) {
        *i = backupLights1;
        *j = backupLights2;
        *i2 = backupLights1;
        *j2 = backupLights2;
        *l = backupLights3;
        *k = backupHat1;
        *(k + 1) = backupHat2;
    }
}