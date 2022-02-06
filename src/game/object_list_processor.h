#ifndef OBJECT_LIST_PROCESSOR_H
#define OBJECT_LIST_PROCESSOR_H

#include <PR/ultratypes.h>

#include "area.h"
#include "macros.h"
#include "types.h"
#include "src/objpool/objpool.h"

/**
 * Flags for gTimeStopState. These control which objects are processed each frame
 * and also track some miscellaneous info.
 */
#define TIME_STOP_UNKNOWN_0         (1 << 0)
#define TIME_STOP_ENABLED           (1 << 1)
#define TIME_STOP_DIALOG            (1 << 2)
#define TIME_STOP_MARIO_AND_DOORS   (1 << 3)
#define TIME_STOP_ALL_OBJECTS       (1 << 4)
#define TIME_STOP_MARIO_OPENED_DOOR (1 << 5)
#define TIME_STOP_ACTIVE            (1 << 6)


/**
 * The maximum number of objects that can be loaded at once.
 */
//#define OBJECT_POOL_CAPACITY 440

/**
 * Every object is categorized into an object list, which controls the order
 * they are processed and which objects they can collide with.
 */
enum ObjectList
{
    OBJ_LIST_PLAYER,      //  (0) Mario
    OBJ_LIST_UNUSED_1,    //  (1) (unused)
    OBJ_LIST_DESTRUCTIVE, //  (2) things that can be used to destroy other objects, like
                          //      bob-ombs and corkboxes
    OBJ_LIST_UNUSED_3,    //  (3) (unused)
    OBJ_LIST_GENACTOR,    //  (4) general actors. most normal 'enemies' or actors are
                          //      on this list. (MIPS, bullet bill, bully, etc)
    OBJ_LIST_PUSHABLE,    //  (5) pushable actors. This is a group of objects which
                          //      can push each other around as well as their parent
                          //      objects. (goombas, koopas, spinies)
    OBJ_LIST_LEVEL,       //  (6) level objects. general level objects such as heart, star
    OBJ_LIST_UNUSED_7,    //  (7) (unused)
    OBJ_LIST_DEFAULT,     //  (8) default objects. objects that didnt start with a 00
                          //      command are put here, so this is treated as a default.
    OBJ_LIST_SURFACE,     //  (9) surface objects. objects that specifically have surface
                          //      collision and not object collision. (thwomp, whomp, etc)
    OBJ_LIST_POLELIKE,    // (10) polelike objects. objects that attract or otherwise
                          //      "cling" Mario similar to a pole action. (hoot,
                          //      whirlpool, trees/poles, etc)
    OBJ_LIST_SPAWNER,     // (11) spawners
    OBJ_LIST_UNIMPORTANT, // (12) unimportant objects. objects that will not load
                          //      if there are not enough object slots: they will also
                          //      be manually unloaded to make room for slots if the list
                          //      gets exhausted.
    NUM_OBJ_LISTS
};


extern struct ObjectNode gObjectListArray[];

struct NumTimesCalled {
    /*0x00*/ s16 floor;
    /*0x02*/ s16 ceil;
    /*0x04*/ s16 wall;
};

extern s16 gDebugInfo[][8];

extern u32 gTimeStopState;
#ifdef OLDOBJPOOLADD
extern struct Object gObjectPool[];
#endif
extern struct Object gMacroObjectDefaultParent;
extern struct ObjectNode *gObjectLists;
extern struct ObjectNode gFreeObjectList;

extern struct Object *gMarioObject;
extern struct Object *gLuigiObject;
extern struct Object *gCurrentObject;

extern const BehaviorScript *gCurBhvCommand;

extern s32 gSurfaceNodesAllocated;
extern s32 gSurfacesAllocated;
extern s32 gNumStaticSurfaceNodes;
extern s32 gNumStaticSurfaces;

extern struct MemoryPool *gObjectMemoryPool;

extern s16 *gEnvironmentRegions;
extern s32 gEnvironmentLevels[20];


void bhv_mario_update(void);
void set_object_respawn_info_bits(struct Object *obj, u8 bits);
void unload_objects_from_area(UNUSED s32 unused, s32 areaIndex);
void spawn_objects_from_info(UNUSED s32 unused, struct SpawnInfo *spawnInfo);
void clear_objects(void);
void update_objects(UNUSED s32 unused);


#endif // OBJECT_LIST_PROCESSOR_H