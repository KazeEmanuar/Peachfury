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
#include "levels/vcutm/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_vcutm_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0_TEXTURE(0x09, _outside_mio0SegmentRomStart, _outside_mio0SegmentRomEnd), 
	LOAD_MIO0(0x07, _vcutm_segment_7SegmentRomStart, _vcutm_segment_7SegmentRomEnd), 
	LOAD_MIO0(0x05, _group8_mio0SegmentRomStart, _group8_mio0SegmentRomEnd), 
	LOAD_RAW(0x0C, _group8_geoSegmentRomStart, _group8_geoSegmentRomEnd), 
	LOAD_MIO0(0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	LOAD_MIO0(0xa, _ccm_skybox_mio0SegmentRomStart, _ccm_skybox_mio0SegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	JUMP_LINK(script_func_global_9), 
	LOAD_MODEL_FROM_GEO(0x35, aircruise_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_VCUTM_WARP_PIPE, warp_pipe_geo), 
	LOAD_MODEL_FROM_GEO(0x36, cloud_geo), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, vcutm_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_CASTLE_GROUNDS, 0x01, 0xEF, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_CASTLE_GROUNDS, 0x01, 0xDF, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_EXCLAMATION_BOX, -3113, -1015, -6182, 0, 0, 0, 0x00010000, bhvExclamationBox),
		OBJECT(MODEL_EXCLAMATION_BOX, 5038, -26, -4916, 0, 0, 0, 0x00010000, bhvExclamationBox),
		OBJECT(MODEL_EXCLAMATION_BOX, 276, 511, 1183, 0, 0, 0, 0x00010000, bhvExclamationBox),
		OBJECT(MODEL_EXCLAMATION_BOX, 3197, 2354, 5913, 0, 0, 0, 0x00010000, bhvExclamationBox),
		OBJECT(0, 159, 339, 606, 0, 0, 0, 0x00000000, bhvHiddenRedCoinStar),
		OBJECT(MODEL_RED_COIN, -2058, -242, -1503, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, 1402, -328, -1653, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -1018, -411, -7170, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, 6210, 126, -4026, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -616, 625, 1436, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -5722, 165, -3950, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, 3295, 2079, 6281, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(0x36, -3243, -691, -7200, 0, 0, 0, 0x00010000, bhvCloudSafety),
		OBJECT(MODEL_EXCLAMATION_BOX, -2479, 2987, 7107, 0, 0, 0, 0x00010000, bhvExclamationBox),
		OBJECT(MODEL_RED_COIN, -3045, 2682, 5362, 0, -67, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_CAP_SWITCH, -2098, 3087, 7495, 0, 0, 0, 0x00010000, bhvCapSwitch),
		OBJECT(0x35, -2487, -985, -3721, 0, 100, 0, 0x00030000, bhvAircruise),
		OBJECT(0x35, 3592, -316, -133, 0, -8, 0, 0x00060000, bhvAircruise),
		OBJECT(0x35, 901, 491, 3802, 0, -72, 0, 0x00070000, bhvAircruise),
		OBJECT(0x35, -4929, -341, 1225, 0, -158, 0, 0x00000000, bhvAircruise),
		OBJECT(MODEL_NONE, -7822, -1309, -7038, 0, 89, 0, 0xC00A0000, bhvFlyingWarp),
		TERRAIN(vcutm_area_1_collision),
		MACRO_OBJECTS(vcutm_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_FLYING_SHIPS),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(1, 0, 0, 0, 0),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
