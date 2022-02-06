#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

/* Fast64 begin persistent block [includes] */
/* Fast64 end persistent block [includes] */

#include "make_const_nonconst.h"
#include "levels/ending/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_ending_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0(0x07, _ending_segment_7SegmentRomStart, _ending_segment_7SegmentRomEnd), 
	LOAD_MIO0(0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	LOAD_MODEL_FROM_GEO(MODEL_THI_WARP_PIPE, warp_pipe_geo), 
	LOAD_MODEL_FROM_GEO(0x35, cardboardboxclose_geo), 
	LOAD_MODEL_FROM_GEO(0x36, selectDisplay_geo), 
	LOAD_MODEL_FROM_GEO(0x37, cardboardpipe_geo), 
	LOAD_MODEL_FROM_GEO(0x38, deletebutton_geo), 
	LOAD_MODEL_FROM_GEO(0x39, firepiranha_geo), 
	LOAD_MODEL_FROM_GEO(0x3A, cardboardbox_geo), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, ending_area_1),
		WARP_NODE(0x0A, LEVEL_CASTLE_GROUNDS, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(0x35, 0, 481, 0, 0, 0, 0, 0x00000000, bhvIntroBox),
		OBJECT(0x36, 0, 931, -1142, 0, 0, 0, 0x00000000, bhvFileDisplay),
		OBJECT(0x37, -1159, 531, -885, 0, 0, 0, 0x010A0000, bhvWarpPipe),
		OBJECT(0x37, 1159, 531, -885, 0, 0, 0, 0x040A0000, bhvWarpPipe),
		OBJECT(0x37, 386, 531, -885, 0, 0, 0, 0x030A0000, bhvWarpPipe),
		OBJECT(0x37, -386, 531, -885, 0, 0, 0, 0x020A0000, bhvWarpPipe),
		OBJECT(0x38, -1159, 531, -585, 0, 0, 0, 0x010A0000, bhvDeleteButton),
		OBJECT(0x38, 1159, 531, -585, 0, 0, 0, 0x040A0000, bhvDeleteButton),
		OBJECT(0x38, 386, 531, -585, 0, 0, 0, 0x030A0000, bhvDeleteButton),
		OBJECT(0x38, -386, 531, -585, 0, 0, 0, 0x020A0000, bhvDeleteButton),
		MARIO_POS(0x01, -180, 0, 531, -375),
		TERRAIN(ending_area_1_collision),
		MACRO_OBJECTS(ending_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_INTRO),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, -180, 0, 531, -375),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
