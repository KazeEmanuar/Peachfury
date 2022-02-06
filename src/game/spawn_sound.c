#include <PR/ultratypes.h>

#include "audio/external.h"
#include "behavior_data.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "sm64.h"
#include "spawn_sound.h"
#include "thread6.h"
#include "OPT_FOR_SIZE.h"

/*
 * execute an object's current sound state with a provided array
 * of sound states. Used for the stepping sounds of various
 * objects. (King Bobomb, Bowser, King Whomp)
 */
void exec_anim_sound_state(struct SoundState *soundStates) {
    s32 stateIdx = gCurrentObject->oSoundStateID;

    switch (soundStates[stateIdx].playSound) {
        // since we have an array of sound states corresponding to
        // various behaviors, not all entries intend to play sounds. the
        // boolean being 0 for unused entries skips these states.
        case FALSE:
            break;
        case TRUE: {
            s32 animFrame;

            // in the sound state information, -1 (0xFF) is for empty
            // animFrame entries. These checks skips them.
            if ((animFrame = soundStates[stateIdx].animFrame1) >= 0) {
                if (cur_obj_check_anim_frame(animFrame)) {
                    cur_obj_play_sound_2(soundStates[stateIdx].soundMagic);
                }
            }

            if ((animFrame = soundStates[stateIdx].animFrame2) >= 0) {
                if (cur_obj_check_anim_frame(animFrame)) {
                    cur_obj_play_sound_2(soundStates[stateIdx].soundMagic);
                }
            }
        } break;
    }
}

/*
 * Create a sound spawner for objects that need a sound play once.
 * (Breakable walls, King Bobomb exploding, etc)
 *//*
void create_sound_spawner(s32 soundMagic) {
   spawn_object(gCurrentObject, 0, bhvSoundSpawner)->oSoundEffectUnkF4 = soundMagic;
}*/

/*
 * The following 2 functions are relevant to the sound state function
 * above. While only cur_obj_play_sound_2 is used, they may have been intended as
 * separate left/right leg functions that went unused.
 */
void cur_obj_play_sound_1(s32 soundMagic) {
    if (gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE) {
        play_sound(soundMagic, gCurrentObject->header.gfx.cameraToObject);
    }
}