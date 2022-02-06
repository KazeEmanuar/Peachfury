// wdw_water_level.c.inc
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
extern u8 talkToMario(int dialogID, int actionArg);
extern u8 askMario(int dialogID, u8 rotate, int actionArg);
extern f32 vec3f_dist(Vec3f a, Vec3f b);
#define m gMarioState
#define o gCurrentObject

u8 doorOpen() {
    struct ObjectNode *listHead;
    struct Object *obj;
    listHead = &gObjectLists[OBJ_LIST_SURFACE];
    obj = (struct Object *) listHead->next;

    while (obj != (struct Object *) listHead) {
        if (obj->behavior == segmented_to_virtual(bhvUnderwaterdoor)) {
            if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && obj != o) {
                if (obj->oBehParams2ndByte) {
                    if ((obj->oAction == 1) || ((obj->oAction == 2) && (obj->oTimer < 12))) {
                        return 1;
                    }
                }
            }
        }
        obj = (struct Object *) obj->header.next;
    }
    return 0;
}
extern u8 gAreaEchoLevel[LEVEL_COUNT][8];
// called when WDW is loaded.
void bhv_init_changing_water_level_loop(void) {
    if (gCurrentObject->oAction == 0) {
        if (gEnvironmentRegions != NULL)
            gCurrentObject->oAction++;
    } else if (gCurrentObject->oTimer < 10)
        *gEnvironmentLevels = gEnvironmentRegions[6];
    else {
        gEnvironmentRegions[6] = *gEnvironmentLevels + sins(o->oWaterLevelTriggerUnkF4) * 20.0f;
        gCurrentObject->oWaterLevelTriggerUnkF4 += 0x200;
    }
    if (gMarioState->floor) {
        if (SURFACETYPE(gMarioState->floor) == SURFACE_PAINTING_WOBBLE_A8) {
            if (!doorOpen()) {
                *gEnvironmentLevels = -16000.f;
                    gAreaEchoLevel[LEVEL_WF][gCurrAreaIndex - 1] = 0x5F;
            } else {
                *gEnvironmentLevels = 8192.f;
                    gAreaEchoLevel[LEVEL_WF][gCurrAreaIndex - 1] = 0;
            }
        } else {
            if (doorOpen()) {
                *gEnvironmentLevels = 8192.f;
                    gAreaEchoLevel[LEVEL_WF][gCurrAreaIndex - 1] = 0x5F;
            }
        }
    }
}

void bhv_water_level_diamond_loop(void) {
}
