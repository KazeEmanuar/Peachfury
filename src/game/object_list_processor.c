#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "behavior_data.h"
#include "camera.h"
#include "debug.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "interaction.h"
#include "level_update.h"
#include "mario.h"
#include "memory.h"
#include "object_collision.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "profiler.h"
#include "spawn_object.h"
#include "rendering_graph_node.h"
#include "OPT_FOR_SIZE.h"
#include "seq_ids.h"
/**
 * An array of debug controls that could be used to tweak in-game parameters.
 * The only used rows are [4] and [5] (effectinfo and enemyinfo).
 */
s16 gDebugInfo[16][8];

/**
 * A set of flags to control which objects are updated on a given frame.
 * This is used during dialog and cutscenes to freeze most objects in place.
 */
u32 gTimeStopState;

/**
 * The pool that objects are allocated from.
 */
#ifdef OLDOBJPOOLADD
struct Object gObjectPool[OBJECT_POOL_CAPACITY];
#endif
/**
 * A special object whose purpose is to act as a parent for macro objects.
 */
struct Object gMacroObjectDefaultParent;

/**
 * A pointer to gObjectListArray.
 * Given an object list index idx, gObjectLists[idx] is the head of a doubly
 * linked list of all currently spawned objects in the list.
 */
struct ObjectNode *gObjectLists;

/**
 * A singly linked list of available slots in the object pool.
 */
struct ObjectNode gFreeObjectList;

/**
 * The object representing Mario.
 */
struct Object *gMarioObject;

/**
 * An object variable that may have been used to represent the planned
 * second player. This is speculation, based on its position and its usage in
 * shadow.c.
 */
struct Object *gLuigiObject;

/**
 * The object whose behavior script is currently being updated.
 * This object is used frequently in object behavior code, and so is often
 * aliased as "o".
 */
struct Object *gCurrentObject;

/**
 * The next object behavior command to be executed.
 */
const BehaviorScript *gCurBhvCommand;

/**
 * The total number of surface nodes allocated (a node is allocated for each
 * spatial partition cell that a surface intersects).
 */
s32 gSurfaceNodesAllocated;

/**
 * The total number of surfaces allocated.
 */
s32 gSurfacesAllocated;

/**
 * The number of nodes that have been created for surfaces.
 */
s32 gNumStaticSurfaceNodes;

/**
 * The number of surfaces in the pool.
 */
s32 gNumStaticSurfaces;

/**
 * A pool used by chain chomp and wiggler to allocate their body parts.
 */
struct MemoryPool *gObjectMemoryPool;

s16 *gEnvironmentRegions;
s32 gEnvironmentLevels[20];

/**
 * Nodes used to represent the doubly linked object lists.
 */
struct ObjectNode gObjectListArray[16];

/**
 * The order that object lists are processed in a frame.
 */
s8 sObjectListUpdateOrder[] = { OBJ_LIST_SPAWNER,
                                OBJ_LIST_SURFACE,
                                OBJ_LIST_POLELIKE,
                                OBJ_LIST_PLAYER,
                                OBJ_LIST_PUSHABLE,
                                OBJ_LIST_GENACTOR,
                                OBJ_LIST_DESTRUCTIVE,
                                OBJ_LIST_LEVEL,
                                OBJ_LIST_DEFAULT,
                                OBJ_LIST_UNIMPORTANT,
                                -1 };

/**
 * Info needed to spawn particles and keep track of which have been spawned for
 * an object.
 */
struct ParticleProperties {
    u32 particleFlag;
    u32 activeParticleFlag;
    u8 model;
    const BehaviorScript *behavior;
};

/**
 * A table mapping particle flags to various properties use when spawning a particle.
 */
struct ParticleProperties sParticleTypes[] = {
    { PARTICLE_DUST, ACTIVE_PARTICLE_DUST, MODEL_MIST, bhvMistParticleSpawner },
    { PARTICLE_VERTICAL_STAR, ACTIVE_PARTICLE_V_STAR, MODEL_NONE, bhvVertStarParticleSpawner },
    { PARTICLE_HORIZONTAL_STAR, ACTIVE_PARTICLE_H_STAR, MODEL_NONE, bhvHorStarParticleSpawner },
    { PARTICLE_SPARKLES, ACTIVE_PARTICLE_SPARKLES, MODEL_SPARKLES, bhvSparkleParticleSpawner },
    { PARTICLE_BUBBLE, ACTIVE_PARTICLE_BUBBLE, MODEL_BUBBLE, bhvBubbleParticleSpawner },
    { PARTICLE_WATER_SPLASH, ACTIVE_PARTICLE_WATER_SPLASH, MODEL_WATER_SPLASH, bhvWaterSplash },
    { PARTICLE_IDLE_WATER_WAVE, ACTIVE_PARTICLE_IDLE_WATER_WAVE, MODEL_IDLE_WATER_WAVE,
      bhvIdleWaterWave },
    { PARTICLE_PLUNGE_BUBBLE, ACTIVE_PARTICLE_PLUNGE_BUBBLE, MODEL_WHITE_PARTICLE_SMALL,
      bhvPlungeBubble },
    { PARTICLE_WAVE_TRAIL, ACTIVE_PARTICLE_WAVE_TRAIL, MODEL_WAVE_TRAIL, bhvWaveTrail },
    { PARTICLE_FIRE, ACTIVE_PARTICLE_FIRE, MODEL_RED_FLAME, bhvFireParticleSpawner },
    { PARTICLE_SHALLOW_WATER_WAVE, ACTIVE_PARTICLE_SHALLOW_WATER_WAVE, MODEL_NONE,
      bhvShallowWaterWave },
    { PARTICLE_SHALLOW_WATER_SPLASH, ACTIVE_PARTICLE_SHALLOW_WATER_SPLASH, MODEL_NONE,
      bhvShallowWaterSplash },
    { PARTICLE_LEAF, ACTIVE_PARTICLE_LEAF, MODEL_NONE, bhvLeafParticleSpawner },
    { PARTICLE_SNOW, ACTIVE_PARTICLE_SNOW, MODEL_NONE, bhvSnowParticleSpawner },
    { PARTICLE_BREATH, ACTIVE_PARTICLE_BREATH, MODEL_NONE, bhvBreathParticleSpawner },
    { PARTICLE_DIRT, ACTIVE_PARTICLE_DIRT, MODEL_NONE, bhvDirtParticleSpawner },
    { PARTICLE_MIST_CIRCLE, ACTIVE_PARTICLE_MIST_CIRCLE, MODEL_NONE, bhvMistCircParticleSpawner },
    { PARTICLE_TRIANGLE, ACTIVE_PARTICLE_TRIANGLE, MODEL_NONE, bhvTriangleParticleSpawner },
    { 0, 0, MODEL_NONE, NULL },
};

/**
 * Copy position, velocity, and angle variables from MarioState to the Mario
 * object.
 */
void copy_mario_state_to_object(void) {
    s32 i = 0;
    s32 j;
    // L is real
    /*if (gCurrentObject != gMarioObject) {
        i += 1;
    }*/

    gCurrentObject->oFloorHeight = gMarioStates[i].floorHeight;
    gCurrentObject->oFloorPointer = gMarioStates[i].floor;
    for (j = 0; j < 3; j++) {
        gCurrentObject->OBJECT_FIELD_F32(0x09 + j) = gMarioStates[i].vel[j];
        gCurrentObject->OBJECT_FIELD_F32(0x06 + j) = gMarioStates[i].pos[j];
        gCurrentObject->OBJECT_FIELD_S32(0x0F + j) = gCurrentObject->header.gfx.angle[j];
        gCurrentObject->OBJECT_FIELD_S32(0x12 + j) = gCurrentObject->header.gfx.angle[j];
        gCurrentObject->OBJECT_FIELD_S32(0x23 + j) = gMarioStates[i].angleVel[j];
    }
}

/**
 * Spawn a particle at gCurrentObject's location.
 */
void spawn_particle(u32 activeParticleFlag, s16 model, const BehaviorScript *behavior) {
    if (!(gCurrentObject->oActiveParticleFlags & activeParticleFlag)) {
        struct Object *particle;
        gCurrentObject->oActiveParticleFlags |= activeParticleFlag;
        particle = spawn_object_at_origin(gCurrentObject, 0, model, behavior);
        obj_copy_pos_and_angle(particle, gCurrentObject);
    }
}

int stated = 0;
int savingtimer = 0;
extern u8 rendermariofromCam;
extern struct CutsceneJump *currentScene;
/**
 * Mario's primary behavior update function.
 */
extern u32 gAudioRandom;
u8 isEmu = 0;
extern u8 finalBuild;
u16 curSong;
u8 transTimer;
u16 songGoal;
extern u16 sCurrentMusic;
extern f32 backupPos[3];
#include "audio/load.h"
extern u8 sceneTimer;
extern f32 gDefaultSoundArgs[3];
extern struct Controller *gPlayer1Controller;
void bhv_mario_update(void) {
    u32 particleFlags = 0;
    s32 i;
    gCurrentObject->oRoom = 0;
    if (gMarioState->action == ACT_HANG_LANTERN) {
        gMarioState->unkB0 = -222;
    } else {
        gMarioState->unkB0 = 0xBD;
    }
    if (!LevelTimer) {
        if (gCurrLevelNum != LEVEL_HMC) {
            return;
        }
        stop_background_music(0);
        play_music(0, SEQ_EVENT_YOSHIBEAT, 0);
        sCurrentMusic = SEQ_EVENT_YOSHIBEAT;
        curSong = SEQ_EVENT_YOSHIBEAT;
        transTimer = 0;
    }
    if (!currentScene) {
        if (gCurrAreaIndex == 1) {
            if (!AreaTimer) {
                backupPos[1] = gMarioState->pos[1];
                curSong = 0;
            }
            if (backupPos[1] > -2500.f) {
                songGoal = SEQ_LEVEL_DINODINO;
            } else if (backupPos[1] < -4000.f) {
                songGoal = SEQ_LEVEL_BUBBLEDOME;
            }
            if (transTimer) {
                transTimer++;
                gSequencePlayers[0].volume =
                    approach_f32_asymptotic(gSequencePlayers[0].volume, 0, 0.25f);
                gSequencePlayers[0].fadeVolume =
                    approach_f32_asymptotic(gSequencePlayers[0].fadeVolume, 0, 0.25f);
                if (transTimer == 15) {
                    stop_background_music(curSong);
                    play_music(0, songGoal, 0);
                    sCurrentMusic = songGoal;
                    curSong = songGoal;
                    transTimer = 0;
                }
            } else {
                if (songGoal != curSong) {
                    transTimer = 1;
                }
            }
        }
        cur_obj_set_model(rendermariofromCam);
    } else {
        cur_obj_set_model(0);
        read_controller_inputs();
        if ((gPlayer1Controller->controllerData->button & (0xF000)) && !sceneTimer) {
            sceneTimer = 1;
            play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 20, 255, 255, 255);
            play_sound(SOUND_MENU_STAR_SOUND, gDefaultSoundArgs);
        }
        if (!sceneTimer) {
            return;
        }
    }
    if (sceneTimer) {
        sceneTimer++;
        if (sceneTimer == 22) {
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 14, 255, 255, 255);
            currentScene = 0;
            reset_scene(gMarioState->area->camera);
            sceneTimer = 0;
        }
        return;
    }

    if (gMarioState->floor && SURFACETYPE(gMarioState->floor) == 0xE1) {
        if (((gMarioState->pos[1] + gMarioState->vel[1]) - gMarioState->floorHeight) < 20.f) {
            if (!(gMarioState->action & ACT_FLAG_INTANGIBLE)) {
                gMarioState->vel[1] = -.25f * gMarioState->vel[1] + 75.f;
                gMarioState->action = ACT_TRIPLE_JUMP;
                gMarioState->marioObj->header.gfx.unk38.animFrame = 0;
                play_sound(SOUND_MARIO_YAHOO_WAHA_YIPPEE + ((gAudioRandom % 5) << 16),
                           gMarioState->marioObj->header.gfx.cameraToObject);
                spawn_object(gCurrentObject, 0, bhvMistCircParticleSpawner);
            }
        }
    }

    if (!currentScene) {
        gMarioState->animSwappedThisFrame = 0;
        particleFlags = execute_mario_action(gCurrentObject);
    }
    gCurrentObject->oMarioParticleFlags = particleFlags;
#ifdef ANIMSWAP
    if (gMarioState->animationSwapID) {
        if (gMarioState->animationSwap->targetAnim->length
            == gMarioState->animationSwap->animDmaTable->anim[gCurrentObject->header.gfx.unk38.animID]
                   .size) {
            gCurrentObject->header.gfx.unk38.curAnim = gMarioState->animationSwap->targetAnim;
        } else {
            gCurrentObject->header.gfx.unk38.curAnim = gMarioState->animation->targetAnim;
        }
    } else {
        if (gMarioState->animation->targetAnim->length
            == gMarioState->animation->animDmaTable->anim[gCurrentObject->header.gfx.unk38.animID]
                   .size) {
            gCurrentObject->header.gfx.unk38.curAnim = gMarioState->animation->targetAnim;
        } else {
            gCurrentObject->header.gfx.unk38.curAnim = gMarioState->animationSwap->targetAnim;
        }
    }
#else
    gCurrentObject->header.gfx.unk38.curAnim = gMarioState->animation->targetAnim;
#endif
    if (gCurrentObject->header.gfx.unk38.curAnim->values < 0x4000) {
        gCurrentObject->header.gfx.unk38.curAnim->values = (void *) VIRTUAL_TO_PHYSICAL(
            (u8 *) gCurrentObject->header.gfx.unk38.curAnim
            + (uintptr_t) gCurrentObject->header.gfx.unk38.curAnim->values);
        gCurrentObject->header.gfx.unk38.curAnim->index =
            (void *) VIRTUAL_TO_PHYSICAL((u8 *) gCurrentObject->header.gfx.unk38.curAnim
                                         + (uintptr_t) gCurrentObject->header.gfx.unk38.curAnim->index);
        gCurrentObject->header.gfx.unk38.prevAnim = 0;
    }
    copy_mario_state_to_object();

    i = 0;
    while (sParticleTypes[i].particleFlag != 0) {
        if (particleFlags & sParticleTypes[i].particleFlag) {
            if ((gCurrAreaIndex != 4) && (gCurrLevelNum == LEVEL_BBH)
                && (sParticleTypes[i].model == MODEL_RED_FLAME)) {
                spawn_particle(sParticleTypes[i].activeParticleFlag, MODEL_BLUE_FLAME,
                               sParticleTypes[i].behavior);
            } else {
                spawn_particle(sParticleTypes[i].activeParticleFlag, sParticleTypes[i].model,
                               sParticleTypes[i].behavior);
            }
        }

        i++;
    }
}

/**
 * Update every object that occurs after firstObj in the given object list,
 * including firstObj itself. Return the number of objects that were updated.
 */
s32 update_objects_starting_at(struct ObjectNode *objList, struct ObjectNode *firstObj) {
    while (objList != firstObj) {
        gCurrentObject = (struct Object *) firstObj;

        gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
        cur_obj_update();

        firstObj = firstObj->next;
    }

    return 0;
}

/**
 * Update objects in objList starting with firstObj while time stop is active.
 * This means that only certain select objects will be updated, such as Mario,
 * doors, unimportant objects, and the object that initiated time stop.
 * The exact set of objects that are updated depends on which flags are set
 * in gTimeStopState.
 * Return the total number of objects in the list (including those that weren't
 * updated)
 */
s32 update_objects_during_time_stop(struct ObjectNode *objList, struct ObjectNode *firstObj) {
    s32 unfrozen;

    while (objList != firstObj) {
        gCurrentObject = (struct Object *) firstObj;

        unfrozen = FALSE;

        // Selectively unfreeze certain objects
        if (!(gTimeStopState & TIME_STOP_ALL_OBJECTS)) {
            if (gCurrentObject == gMarioObject && !(gTimeStopState & TIME_STOP_MARIO_AND_DOORS)) {
                unfrozen = TRUE;
            }

            if ((gCurrentObject->oInteractType & (INTERACT_DOOR | INTERACT_WARP_DOOR))
                && !(gTimeStopState & TIME_STOP_MARIO_AND_DOORS)) {
                unfrozen = TRUE;
            }

            if (gCurrentObject->activeFlags
                & (ACTIVE_FLAG_UNIMPORTANT | ACTIVE_FLAG_INITIATED_TIME_STOP)) {
                unfrozen = TRUE;
            }
        }

        // Only update if unfrozen
        if (unfrozen) {
            gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
            cur_obj_update();
        } else {
            gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        }

        firstObj = firstObj->next;
    }

    return 0;
}

/**
 * Update every object in the given list. Return the total number of objects in
 * the list.
 */
s32 update_objects_in_list(struct ObjectNode *objList) {
    struct ObjectNode *firstObj = objList->next;

    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        update_objects_starting_at(objList, firstObj);
    } else {
        update_objects_during_time_stop(objList, firstObj);
    }

    return 0;
}

/**
 * Unload any objects in the list that have been deactivated.
 */
s32 unload_deactivated_objects_in_list(struct ObjectNode *objList) {
    struct ObjectNode *obj = objList->next;

    while (objList != obj) {
        gCurrentObject = (struct Object *) obj;

        obj = obj->next;

        if ((gCurrentObject->activeFlags & ACTIVE_FLAG_ACTIVE) != ACTIVE_FLAG_ACTIVE) {
            // Prevent object from respawning after exiting and re-entering the
            // area
            if (!(gCurrentObject->oFlags & OBJ_FLAG_PERSISTENT_RESPAWN)) {
                set_object_respawn_info_bits(gCurrentObject, RESPAWN_INFO_DONT_RESPAWN);
            }

            unload_object(gCurrentObject);
        }
    }

    return 0;
}

/**
 * OR the object's respawn info with bits << 8. If bits = 0xFF, this prevents
 * the object from respawning after leaving and re-entering the area.
 * For macro objects, respawnInfo points to the 16 bit entry in the macro object
 * list. For other objects, it points to the 32 bit behaviorArg in the
 * SpawnInfo.
 */
void set_object_respawn_info_bits(struct Object *obj, u8 bits) {
    u32 *info32;
    u16 *info16;

    switch (obj->respawnInfoType) {
        case RESPAWN_INFO_TYPE_32:
            info32 = (u32 *) obj->respawnInfo;
            *info32 |= bits << 8;
            break;

        case RESPAWN_INFO_TYPE_16:
            info16 = (u16 *) obj->respawnInfo;
            *info16 |= bits << 8;
            break;
    }
}

/**
 * Unload all objects whose activeAreaIndex is areaIndex.
 */
void unload_objects_from_area(UNUSED s32 unused, s32 areaIndex) {
    struct Object *obj;
    struct ObjectNode *node;
    struct ObjectNode *list;
    s32 i;
    gObjectLists = gObjectListArray;

    for (i = 0; i < NUM_OBJ_LISTS; i++) {
        list = gObjectLists + i;
        node = list->next;

        while (node != list) {
            obj = (struct Object *) node;
            node = node->next;

            if (obj->header.gfx.unk19 == areaIndex) {
                unload_object(obj);
            }
        }
    }
}
#include "game/camera.h"
struct CameraTrigger C5Rooms[] = {
    { -1, 5, 7996, -3853, -3414, 14667, 1000, 2303, 0xffff },
    { -1, 1, 6049, -308, 1356, 3000, 3255, 2303, 0xffff },
    { -1, 3, -4586, 2368, 356, 3000, 3255, 3073, 0xffff },
    { -1, 6, -9535, 2368, -1649, 740, 3255, 3073, 0xffff },
    { -1, 4, -4700, -3703, 4154, 6332, 3255, 2303, 0xffff },
    { -1, 4, -10246, -3618, 1584, 1202, 2080, 3216, 0xffff },
    { -1, 2, -8198, 1484, 325, 688, 3255, 2326, 0xffff },
    { -1, 2, -3059, -3439, 368, 6217, 3255, 1573, 0xffff },
    { -1, 7, -10925, 2368, -1649, 740, 3255, 3073, 0xffff },
    { -1, 8, -12261, 2368, -1649, 740, 3255, 3073, 0xffff },
    { -1, 9, -13622, 2368, -1649, 777, 3255, 3073, 0xffff },
};

u32 scanRoomList(struct CameraTrigger *roomPositions, u32 boxCount, struct Object *this) {
    u8 roomsFound = 0;
    u8 i;
    u32 returnValue;
    for (i = 0; i < boxCount; i++) {
        if (absf(this->oPosX - roomPositions[i].centerX) < roomPositions[i].boundsX) {
            if (absf(this->oPosY - roomPositions[i].centerY) < roomPositions[i].boundsY) {
                if (absf(this->oPosZ - roomPositions[i].centerZ) < roomPositions[i].boundsZ) {
                    roomsFound++;
                    switch (roomsFound) {
                        case 0:
                            returnValue = (u32) roomPositions[i].event;
                            break;
                        case 1:
                            returnValue |= (u32) roomPositions[i].event << 8;
                            break;
                        case 2:
                            returnValue |= (u32) roomPositions[i].event << 16;
                            break;
                        case 3:
                            returnValue |= (u32) roomPositions[i].event << 24;
                            break;
                    }
                }
            }
        }
    }
    return returnValue;
}

void setRooms(struct Object *this) {
    if (gCurrLevelNum == LEVEL_BBH) {
        if (gCurrAreaIndex == 2) {
            this->oRoom = scanRoomList(&C5Rooms, 8, this);
        }
    } else {
        this->oRoom = 0;
    }
}
/**
 * Spawn objects given a list of SpawnInfos. Called when loading an area.
 */
void spawn_objects_from_info(UNUSED s32 unused, struct SpawnInfo *spawnInfo) {
    struct Surface *pfloor;
    gObjectLists = gObjectListArray;
    gTimeStopState = 0;

    while (spawnInfo != NULL) {
        struct Object *object;
        const BehaviorScript *script;

        script = segmented_to_virtual(spawnInfo->behaviorScript);

        // If the object was previously killed/collected, don't respawn it
        if ((spawnInfo->behaviorArg & (RESPAWN_INFO_DONT_RESPAWN << 8))
            != (RESPAWN_INFO_DONT_RESPAWN << 8)) {
            object = create_object(script);

            // Behavior parameters are often treated as four separate bytes, but
            // are stored as an s32.
            object->oBehParams = spawnInfo->behaviorArg;
            // The second byte of the behavior parameters is copied over to a special field
            // as it is the most frequently used by objects.
            object->oBehParams2ndByte = ((spawnInfo->behaviorArg) >> 16) & 0xFF;

            object->behavior = script;

            // Record death/collection in the SpawnInfo
            object->respawnInfoType = RESPAWN_INFO_TYPE_32;
            object->respawnInfo = &spawnInfo->behaviorArg;

            if (object->behavior == segmented_to_virtual(bhvMario)) {
                gMarioObject = object;
                geo_make_first_child(&object->header.gfx.node);
            }

            geo_obj_init_spawninfo(&object->header.gfx, spawnInfo);

            object->oPosX = spawnInfo->startPos[0];
            object->oPosY = spawnInfo->startPos[1];
            object->oPosZ = spawnInfo->startPos[2];

            object->oFaceAnglePitch = spawnInfo->startAngle[0];
            object->oFaceAngleYaw = spawnInfo->startAngle[1];
            object->oFaceAngleRoll = spawnInfo->startAngle[2];

            object->oMoveAnglePitch = spawnInfo->startAngle[0];
            object->oMoveAngleYaw = spawnInfo->startAngle[1];
            object->oMoveAngleRoll = spawnInfo->startAngle[2];
            object->oFloorHeight =
                find_floor(object->oPosX, object->oPosY, object->oPosZ, &object->oFloorPointer);

            setRooms(object);
        }

        spawnInfo = spawnInfo->next;
    }
}

/**
 * Clear objects, dynamic surfaces, and some miscellaneous level data used by objects.
 */
void clear_objects(void) {
    s32 i;

    gTimeStopState = 0;
    gMarioObject = NULL;

    init_free_object_list();
    clear_object_lists(gObjectListArray);

    for (i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        gObjectPool[i].activeFlags = ACTIVE_FLAG_DEACTIVATED;
        geo_reset_object_node(&gObjectPool[i].header.gfx);
    }

    gObjectMemoryPool = mem_pool_init(0x800, MEMORY_POOL_LEFT);
    gObjectLists = gObjectListArray;

    clear_dynamic_surfaces();
}

/**
 * Update spawner and surface objects.
 */
void update_terrain_objects(void) {
    update_objects_in_list(&gObjectLists[OBJ_LIST_SPAWNER]);
    update_objects_in_list(&gObjectLists[OBJ_LIST_SURFACE]);
}

/**
 * Update all other object lists besides spawner and surface objects, using
 * the order specified by sObjectListUpdateOrder.
 */
void update_non_terrain_objects(void) {
    s32 listIndex;

    s32 i = 2;
    while ((listIndex = sObjectListUpdateOrder[i]) != -1) {
        update_objects_in_list(&gObjectLists[listIndex]);
        i += 1;
    }
}

/**
 * Unload deactivated objects in any object list.
 */
void unload_deactivated_objects(void) {
    s32 listIndex;

    s32 i = 0;
    while ((listIndex = sObjectListUpdateOrder[i]) != -1) {
        unload_deactivated_objects_in_list(&gObjectLists[listIndex]);
        i += 1;
    }
}

/**
 * Unused profiling function.
 *//*
static u16 unused_get_elapsed_time(u64 *cycleCounts, s32 index) {
    u16 time;
    f64 cycles;

    cycles = cycleCounts[index] - cycleCounts[index - 1];
    if (cycles < 0) {
        cycles = 0;
    }

    time = (u16)(((u64) cycles * 1000000 / osClockRate) / 16667.0 * 1000.0);
    if (time > 999) {
        time = 999;
    }

    return time;
}*/

/**
 * Update all objects. This includes script execution, object collision detection,
 * and object surface management.
 */
void update_objects(UNUSED s32 unused) {
    s32 i = 0;

    gTimeStopState &= ~TIME_STOP_MARIO_OPENED_DOOR;

    // reset_debug_objectinfo();

    gObjectLists = gObjectListArray;

    // If time stop is not active, unload object surfaces
    clear_dynamic_surfaces();

    // Update spawners and objects with surfaces
    update_terrain_objects();

    // If Mario was touching a moving platform at the end of last frame, apply
    // displacement now
    //! If the platform object unloaded and a different object took its place,
    //  displacement could be applied incorrectly
    apply_mario_platform_displacement();

    // Detect which objects are intersecting
    detect_object_collisions();

    // Update all other objects that haven't been updated yet
    update_non_terrain_objects();

    // Unload any objects that have been deactivated
    unload_deactivated_objects();

    // Check if Mario is on a platform object and save this object
    update_mario_platform();

    LIGHTS();

    // If time stop was enabled this frame, activate it now so that it will
    // take effect next frame
    if (gTimeStopState & TIME_STOP_ENABLED) {
        gTimeStopState |= TIME_STOP_ACTIVE;
    } else {
        gTimeStopState &= ~TIME_STOP_ACTIVE;
    }
}
