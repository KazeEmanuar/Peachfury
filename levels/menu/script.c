#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/area.h"
#include "game/level_update.h"
#include "menu/file_select.h"
#include "menu/star_select.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/menu/header.h"

const LevelScript level_main_menu_entry_1[] = {
    INIT_LEVEL(),
    FIXED_LOAD(/*loadAddr*/ _goddardSegmentStart, /*romStart*/ _goddardSegmentRomStart, /*romEnd*/ _goddardSegmentRomEnd),
    LOAD_MIO0(/*seg*/ 0x07, _menu_segment_7SegmentRomStart, _menu_segment_7SegmentRomEnd),
    LOAD_RAW(/*seg*/ 0x13, _behaviorSegmentRomStart, _behaviorSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    FREE_LEVEL_POOL(),
    SET_REG(/*value*/ LEVEL_HMC),
    EXIT_AND_EXECUTE(/*seg*/ 0x15, _scriptsSegmentRomStart, _scriptsSegmentRomEnd, level_main_scripts_entry),
};

const LevelScript level_main_menu_entry_2[] = {
    /*0*/ CALL(/*arg*/ 0, /*func*/ lvl_set_current_level),
    /*42*/ EXIT(),
    /*2*/ JUMP_IF(/*op*/ OP_EQ, /*arg*/ 0, level_main_menu_entry_2 + 42),
    /*5*/ INIT_LEVEL(),
    /*6*/ FIXED_LOAD(/*loadAddr*/ _goddardSegmentStart, /*romStart*/ _goddardSegmentRomStart, /*romEnd*/ _goddardSegmentRomEnd),
    /*10*/ LOAD_MIO0(/*seg*/ 0x07, _menu_segment_7SegmentRomStart, _menu_segment_7SegmentRomEnd),
    /*13*/ ALLOC_LEVEL_POOL(),

    /*14*/ AREA(/*index*/ 2, geo_menu_act_selector_strings),
        /*16*/ OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, -100, 0, /*angle*/ 0, 0, 0, /*behParam*/ 0x04000000, /*beh*/ bhvActSelector),
        /*22*/ TERRAIN(/*terrainData*/ main_menu_seg7_collision),
    /*24*/ END_AREA(),

    /*25*/ FREE_LEVEL_POOL(),
    /*26*/ LOAD_AREA(/*area*/ 2),
#ifdef NO_SEGMENTED_MEMORY
           // sVisibleStars is set to 0 during FIXED_LOAD above on N64, but not when NO_SEGMENTED_MEMORY is used.
           // lvl_init_act_selector_values_and_stars must be called here otherwise the previous
           // value is retained and causes incorrect drawing during the 16 transition frames.
           CALL(/*arg*/ 0, /*func*/ lvl_init_act_selector_values_and_stars),
#endif
    /*27*/ TRANSITION(/*transType*/ WARP_TRANSITION_FADE_FROM_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    /*29*/ SLEEP(/*frames*/ 16),
    /*30*/ SET_MENU_MUSIC(/*seq*/ 0x0032),
#ifndef NO_SEGMENTED_MEMORY
    /*31*/ CALL(/*arg*/ 0, /*func*/ lvl_init_act_selector_values_and_stars),
#endif
    /*33*/ CALL_LOOP(/*arg*/ 0, /*func*/ lvl_update_obj_and_load_act_button_actions),
    /*35*/ GET_OR_SET(/*op*/ OP_SET, /*var*/ VAR_CURR_ACT_NUM),
    /*36*/ STOP_MUSIC(/*fadeOutTime*/ 0x00BE),
    /*37*/ TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    /*39*/ SLEEP(/*frames*/ 16),
    /*40*/ CLEAR_LEVEL(),
    /*41*/ SLEEP_BEFORE_EXIT(/*frames*/ 1),
    // L1:
    /*42*/ EXIT(),
};
