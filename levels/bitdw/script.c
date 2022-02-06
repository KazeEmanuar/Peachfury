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
#include "levels/bitdw/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_bitdw_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0(0x07, _bitdw_segment_7SegmentRomStart, _bitdw_segment_7SegmentRomEnd), 
	LOAD_MIO0(0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	LOAD_MIO0(0xb, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	LOAD_MODEL_FROM_GEO(0x35, chainbridge_geo), 
	LOAD_MODEL_FROM_GEO(0x36, chianplatform_geo), 
	LOAD_MODEL_FROM_GEO(0x37, roperotate_geo), 
	LOAD_MODEL_FROM_GEO(0x38, bowsercart_geo), 
	LOAD_MODEL_FROM_GEO(0x39, bowser1flag_geo), 
	LOAD_MODEL_FROM_GEO(0x3A, lavafloor_geo), 
	LOAD_MODEL_FROM_GEO(0x3B, mechboss_geo), 
	LOAD_MODEL_FROM_GEO(0x3C, clowncarfoe_geo), 
	LOAD_MODEL_FROM_GEO(0x3D, bonzaibob_geo), 
	LOAD_MODEL_FROM_GEO(0x3E, bowserB1_geo), 
	LOAD_MODEL_FROM_GEO(0x3F, curtain_geo), 
	LOAD_MODEL_FROM_GEO(0x40, bossplatform_geo), 
	LOAD_MODEL_FROM_GEO(0x41, bossplatform2_geo), 
	LOAD_MODEL_FROM_GEO(0x42, mechboss2_geo), 
	LOAD_MODEL_FROM_GEO(0x43, mechboss3_geo), 
	LOAD_MODEL_FROM_GEO(0x44, arm1_geo), 
	LOAD_MODEL_FROM_GEO(0x45, arm2_geo), 
	LOAD_MODEL_FROM_GEO(0x46, bowserhead_geo), 
	LOAD_MODEL_FROM_GEO(0x47, titlelakitu_geo), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, bitdw_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_CASTLE_GROUNDS, 0x02, 0xED, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_CASTLE_GROUNDS, 0x02, 0xDD, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		INSTANT_WARP(0, 0x02, 0, 0, -20000),
		OBJECT(MODEL_NONE, 915, 5089, 16596, 0, -162, 0, 0x000A0000, bhvSpinAirborneWarp),
		OBJECT(MODEL_NONE, 1338, 0, -1345, 0, 0, 0, 0x00100000, bhvTextureAnims),
		OBJECT(MODEL_CANNON_BASE, 0, 13004, 0, 0, -135, 0, 0x00C00000, bhvCannon),
		OBJECT(0x35, -3561, 3537, -1624, 0, 90, 0, 0x00100001, bhvChainBridge),
		OBJECT(0x35, -3561, 3728, -1024, 0, 90, 0, 0x00100000, bhvChainBridge),
		OBJECT(0x35, -3561, 3807, -424, 0, 90, 0, 0x00100000, bhvChainBridge),
		OBJECT(0, 4, 7991, 3566, 0, 0, 0, 0x00000001, bhvChainElevator),
		OBJECT(0x37, -1573, 8130, 3346, 0, 177, 0, 0x00000001, bhvRopePlatform),
		MARIO_POS(0x01, -162, 909, 5085, 16597),
		OBJECT(0x38, 915, 5089, 16596, 0, -162, 0, 0x000A0000, bhvBowserCart),
		OBJECT(MODEL_BREAKABLE_BOX, 3239, 200, -280, 0, 0, 0, 0x00030000, bhvBreakableBox),
		OBJECT(MODEL_BREAKABLE_BOX, 2989, 200, -330, 0, 0, 0, 0x00010000, bhvBreakableBox),
		OBJECT(MODEL_NONE, 3017, 569, -877, 0, 0, 0, 0x00040000, bhvFlamethrower),
		OBJECT(MODEL_NONE, 3017, 1194, -2864, 0, 0, 0, 0x00040000, bhvFlamethrower),
		OBJECT(MODEL_RED_COIN, 1294, 2419, -4223, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_BREAKABLE_BOX, -3339, 3962, 395, 0, -180, 0, 0x00030000, bhvBreakableBox),
		OBJECT(MODEL_BREAKABLE_BOX, -3089, 3962, 445, 0, -180, 0, 0x00010000, bhvBreakableBox),
		OBJECT(MODEL_RED_COIN, 3422, 869, 256, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(0x35, -3456, 5647, 1762, 0, 90, 0, 0x00100001, bhvChainBridge),
		OBJECT(0x35, -3456, 5837, 2363, 0, 90, 0, 0x00100000, bhvChainBridge),
		OBJECT(0x35, -3456, 5916, 2963, 0, 90, 0, 0x00100000, bhvChainBridge),
		OBJECT(MODEL_NONE, -740, 6100, 2844, 0, 0, 0, 0x00040000, bhvFlamethrower),
		OBJECT(MODEL_RED_COIN, -740, 6400, 2844, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(0x39, 3284, 8732, 498, 0, -180, 0, 0x00100003, bhvCheckpointFlag),
		OBJECT(0, 3284, 8732, 498, 0, -180, 0, 0x000B0000, bhvSpinAirborneWarp),
		OBJECT(0x35, 1203, 9358, -3963, 0, -29, 0, 0x00100001, bhvChainBridge),
		OBJECT(0x35, 677, 9358, -4252, 0, -29, 0, 0x00100000, bhvChainBridge),
		OBJECT(0x35, 151, 9437, -4542, 0, -29, 0, 0x00100000, bhvChainBridge),
		OBJECT(MODEL_NONE, -1646, 9614, -3002, 0, 0, 0, 0x00040000, bhvFlamethrower),
		OBJECT(MODEL_NONE, -642, 9137, -3315, 0, 0, 0, 0x00040000, bhvFlamethrower),
		OBJECT(MODEL_NONE, -1414, 11376, -2710, 0, 0, 0, 0x01000000, bhvBowserCourseRedCoinStar),
		OBJECT(MODEL_RED_COIN, 6, 8604, 3666, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, 2114, 9331, 1636, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -3354, 3962, 386, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -2395, 6846, 1973, 0, -90, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, 664, 9612, -4275, 0, -118, 0, 0x00000000, bhvRedCoin),
		OBJECT(0xf7, -417, 16015, 3723, 0, 157, 0, 0x000A0071, bhvYoshiTalkingB1),
		OBJECT(0xf7, -338, 10732, -1204, 0, 113, 0, 0x00080000, bhvYoshiWalking),
		OBJECT(0xf7, 411, 16025, 3681, 0, 157, 0, 0x00070070, bhvYoshiTalkingB1),
		OBJECT(0xf7, 585, 10732, -1208, 0, 68, 0, 0x00090000, bhvYoshiWalking),
		OBJECT(0xf7, -1158, 10732, -701, 0, 157, 0, 0x00060000, bhvYoshiWalking),
		OBJECT(0x3A, 0, 0, 0, 0, -90, 0, 0x00000000, bhvLavaFloor),
		OBJECT(0x3C, -2934, 3600, -3060, 0, 0, 0, 0x00100000, bhvClownFoe),
		OBJECT(0x3C, 1627, 8684, 3031, 0, 0, 0, 0x00100000, bhvClownFoe),
		TERRAIN(bitdw_area_1_collision),
		MACRO_OBJECTS(bitdw_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, 0),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(2, bitdw_area_2),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_CASTLE_GROUNDS, 0x02, 0xED, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_CASTLE_GROUNDS, 0x02, 0xDD, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_PURPLE_SWITCH, 3582, 8145, 7, 0, -90, 0, 0x00000000, bhvFloorSwitchAnimatesObject),
		OBJECT(MODEL_PURPLE_SWITCH, 1788, 8145, -3099, 0, -30, 0, 0x00000000, bhvFloorSwitchAnimatesObject),
		OBJECT(MODEL_PURPLE_SWITCH, -1799, 8145, -3099, 0, 30, 0, 0x00000000, bhvFloorSwitchAnimatesObject),
		OBJECT(MODEL_PURPLE_SWITCH, -3593, 8145, 7, 0, 90, 0, 0x00000000, bhvFloorSwitchAnimatesObject),
		OBJECT(MODEL_PURPLE_SWITCH, -1799, 8145, 3114, 0, 150, 0, 0x00000000, bhvFloorSwitchAnimatesObject),
		OBJECT(MODEL_PURPLE_SWITCH, 1779, 8145, 3098, 0, -60, 0, 0x00000000, bhvFloorSwitchAnimatesObject),
		OBJECT(0x3B, -5, 6990, 7, 0, -180, 0, 0x00000000, bhvMechBowserBoss),
		OBJECT(0x3D, 3218, 8181, 7, 0, -90, 0, 0x00000000, bhvBonzaiBob),
		OBJECT(0x3D, 1606, 8181, -2784, 0, -30, 0, 0x00000000, bhvBonzaiBob),
		OBJECT(0x3D, -1617, 8181, -2784, 0, 30, 0, 0x00000000, bhvBonzaiBob),
		OBJECT(0x3D, -3229, 8181, 7, 0, 90, 0, 0x00000000, bhvBonzaiBob),
		OBJECT(0x3D, -1617, 8181, 2799, 0, 150, 0, 0x00000000, bhvBonzaiBob),
		OBJECT(0x3D, 1598, 8181, 2784, 0, -150, 0, 0x00000000, bhvBonzaiBob),
		OBJECT(0x3E, -5, 7216, 6473, 0, -180, 0, 0x00000000, bhvBossBOne),
		OBJECT(0, -5, 8763, 5087, 0, -180, 0, 0x00000000, bhvStaticObject),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x020009D4, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x040009D4, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x060009D4, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x080009D4, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x0A0009D4, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x0C0009D4, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x01000534, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x05000534, bhvBossPlatform),
		OBJECT(0x41, 2511, 6130, 546, 0, -180, 0, 0x09000534, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x03000534, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x07000534, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x0B000534, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x010009D4, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x030009D4, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x050009D4, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x070009D4, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x090009D4, bhvBossPlatform),
		OBJECT(0x40, 2511, 6130, 546, 0, -180, 0, 0x0B0009D4, bhvBossPlatform),
		OBJECT(0, 1788, 8195, -3099, 0, -150, 0, 0x00060000, bhvCoinFormation),
		OBJECT(0, -1799, 8195, -3099, 0, -90, 0, 0x00060000, bhvCoinFormation),
		OBJECT(0, -3592, 8195, 7, 0, -30, 0, 0x00060000, bhvCoinFormation),
		OBJECT(0, -1799, 8195, 3114, 0, 30, 0, 0x00060000, bhvCoinFormation),
		OBJECT(0, 1788, 8195, 3114, 4, 90, 4, 0x00060000, bhvCoinFormation),
		OBJECT(0, 3581, 8195, 7, 0, 150, 0, 0x00060000, bhvCoinFormation),
		OBJECT(MODEL_NONE, -443, 12105, 696, 0, 0, 0, 0x00100000, bhvTextureAnims),
		OBJECT(0x47, -5, 6948, 7, 0, -131, 0, 0x00000000, bhvTitleLakitu),
		TERRAIN(bitdw_area_2_collision),
		MACRO_OBJECTS(bitdw_area_2_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, 0),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, -162, 909, 5085, 16597),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
