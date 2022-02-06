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
#include "levels/bob/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_bob_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0(0x07, _bob_segment_7SegmentRomStart, _bob_segment_7SegmentRomEnd), 
	LOAD_MIO0_TEXTURE(0x09, _generic_mio0SegmentRomStart, _generic_mio0SegmentRomEnd), 
	LOAD_MIO0(0x0A, _bitfs_skybox_mio0SegmentRomStart, _bitfs_skybox_mio0SegmentRomEnd), 
	LOAD_MIO0(0x05, _group3_mio0SegmentRomStart, _group3_mio0SegmentRomEnd), 
	LOAD_RAW(0x0C, _group3_geoSegmentRomStart, _group3_geoSegmentRomEnd), 
	LOAD_MIO0(0x06, _group15_mio0SegmentRomStart, _group15_mio0SegmentRomEnd), 
	LOAD_RAW(0x0D, _group15_geoSegmentRomStart, _group15_geoSegmentRomEnd), 
	LOAD_MIO0(0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	JUMP_LINK(script_func_global_4), 
	JUMP_LINK(script_func_global_16), 
	LOAD_MODEL_FROM_GEO(0x16, warp_pipe_geo), 
	LOAD_MODEL_FROM_GEO(0x35, goombaking_geo), 
	LOAD_MODEL_FROM_GEO(0x37, goombabunny_geo), 
	LOAD_MODEL_FROM_GEO(0x38, bunnyarrows_geo), 
	LOAD_MODEL_FROM_GEO(0x39, windmill_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_MIPS, bunny_geo), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, bob_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_BOB, 0x02, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0C, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0D, LEVEL_BOB, 0x03, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0x0E, LEVEL_BOB, 0x04, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x01, 0xe1, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x01, 0xd1, WARP_NO_CHECKPOINT),
		OBJECT(0xf0, -4036, 2053, -917, 0, 0, 0, 0x001c0000, bhvBounceFlower),
		OBJECT(0xf0, -5027, 1617, 7793, 0, 0, 0, 0x00000000, bhvBounceFlower),
		OBJECT(0, -4029, 2381, -918, 0, 0, 0, 0x00010000, bhvCoinFormation),
		OBJECT(0, -2294, 2065, -3522, 0, 0, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, 385, 1012, -591, 0, 0, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, -1424, 129, -2223, 0, 0, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 822, 129, -1847, 0, -15, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -2366, 858, -1259, 0, 91, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -1876, 718, 375, 0, 99, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 2643, 1870, 1436, 0, 0, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, -527, 34, 3133, 0, 0, 0, 0x00010000, bhvCoinFormation),
		OBJECT(0, -1570, 801, 3943, 0, 0, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 547, 958, 9306, 0, -61, 0, 0x00000001, bhvCoinFormation),
		OBJECT(0, -3333, 2611, 489, 0, -3, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -1426, -230, 3788, 0, 0, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, -3419, 2652, -1959, 0, 0, 0, 0x00020000, bhvCoinFormation),
		OBJECT_WITH_ACTS(0xf0, 139, 3, -3350, 0, 0, 0, 0x000a0000, bhvBounceFlower, ACT_3),
		OBJECT_WITH_ACTS(0, -4179, 1363, 7525, 0, 0, 0, 0x00000000, bhvGoombaTripletSpawner, ACT_1 | ACT_2 | ACT_3),
		OBJECT_WITH_ACTS(0, 0, 958, 7849, 0, 0, 0, 0x00000000, bhvGoombaTripletSpawner, ACT_1 | ACT_2 | ACT_3),
		OBJECT_WITH_ACTS(0, -1303, 489, 2078, 0, 0, 0, 0x00000000, bhvGoombaTripletSpawner, ACT_1 | ACT_2 | ACT_3),
		OBJECT(0, -910, 386, 4756, 0, 0, 0, 0x00000000, bhvSpawnBlueOnGP),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, 2093, 842, 1819, 0, -71, 0, 0x00000000, bhvGoombaTroop, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, 1998, 842, 1519, 0, -71, 0, 0x00000000, bhvGoombaTroop, ACT_2),
		OBJECT_WITH_ACTS(0x35, 154, 4, -866, 0, -148, 0, 0x02000000, bhvGoombaKing, ACT_3),
		OBJECT(0, 463, 386, 3108, 0, 0, 0, 0x00000000, bhvSpawnBlueOnGP),
		OBJECT(MODEL_GRASSHOPPER, -714, 485, 995, 0, 0, 0, 0x00000000, bhvGrassHopper),
		OBJECT_WITH_ACTS(MODEL_GRASSHOPPER, -4058, 2079, 1068, 0, 0, 0, 0x00000000, bhvGrassHopper, ACT_1 | ACT_2 | ACT_3),
		OBJECT_WITH_ACTS(MODEL_GRASSHOPPER, -4077, 1363, 6616, 0, 0, 0, 0x00000000, bhvGrassHopper, ACT_1 | ACT_2 | ACT_3),
		OBJECT(MODEL_GRASSHOPPER, -1060, 1152, 9090, 0, 0, 0, 0x00000000, bhvGrassHopper),
		OBJECT_WITH_ACTS(MODEL_GRASSHOPPER, -222, 888, 6607, 0, 0, 0, 0x00000000, bhvGrassHopper, ACT_1 | ACT_2 | ACT_3),
		OBJECT_WITH_ACTS(0x37, -3229, 1363, 8591, 0, 151, 0, 0x00000000, bhvGoombaBunny, ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x37, -4190, 1347, 5241, 0, 47, 0, 0x00000000, bhvGoombaBunny, ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x37, 663, 958, 8343, 0, -136, 0, 0x00000000, bhvGoombaBunny, ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT(0, -423, 506, -4198, 0, 0, 0, 0xE00a0000, bhvAirborneWarp),
		OBJECT(0, 1949, 841, 1752, 0, -70, 0, 0x000C0000, bhvAirborneWarp),
		OBJECT(0, 2547, 690, 1554, 0, -70, 0, 0x300B0000, bhvWarp),
		OBJECT_WITH_ACTS(MODEL_MIPS, 623, 4, -2580, 0, -75, 0, 0x009f0000, bhvMipsDance, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_MIPS, -1268, 4, -1209, 0, 126, 0, 0x00a00000, bhvMips, ACT_1 | ACT_2),
		OBJECT(MODEL_MIPS, 2222, 547, -1832, 0, -47, 0, 0x00a10000, bhvMips),
		OBJECT(MODEL_MIPS, -4501, 2176, 2207, 0, 149, 0, 0x00a20000, bhvMips),
		OBJECT_WITH_ACTS(MODEL_MIPS, -2020, 1135, -2779, 0, -16, 0, 0x00a30000, bhvMips, ACT_1 | ACT_2),
		OBJECT_WITH_ACTS(MODEL_MIPS, 1746, 835, 2637, 0, -127, 0, 0x00a40000, bhvMips, ACT_2),
		OBJECT_WITH_ACTS(MODEL_MIPS, 581, 4, -2278, 0, -94, 0, 0x00a50000, bhvMips, ACT_1 | ACT_2),
		OBJECT_WITH_ACTS(MODEL_MIPS, -1137, 4, -1014, 0, 126, 0, 0x009E0000, bhvMipsDance, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_MIPS, -2020, 1135, -2779, 0, -16, 0, 0x00a30000, bhvMipsDance, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x38, 2499, 548, -1415, 0, -36, 0, 0x00a10000, bhvMipsArrow, ACT_4),
		OBJECT(0x16, 1689, 546, -1749, 0, 76, 0, 0x000D0000, bhvWarpPipe),
		OBJECT(MODEL_RED_COIN, 508, 728, 6695, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -1265, 2336, 7058, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(0, -2970, 973, 7377, 0, 0, 0, 0x00010000, bhvCoinFormation),
		OBJECT(MODEL_BIRDS2, -2129, 1847, 7454, 0, -145, 0, 0x00000000, bhvRedBird),
		OBJECT(MODEL_RED_COIN, 1326, 1091, 7787, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -5028, 2617, 7792, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -3747, 1501, 9217, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_RED_COIN, -5249, 1501, 6145, 0, 0, 0, 0x00000000, bhvRedCoin),
		OBJECT(MODEL_BIRDS2, -586, 968, 5514, 0, 149, 0, 0x00000000, bhvRedBird),
		OBJECT(MODEL_BIRDS, -1297, 1383, -3547, 0, 0, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, -3289, 2497, -341, 0, 63, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, 100, 753, -3573, 0, 0, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, 1297, 698, -2499, 0, -63, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, 2304, 1272, 83, 0, -80, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, -2327, 1961, 5291, 0, 143, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, -2737, 1382, 8859, 0, -117, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BIRDS, -2635, 1427, 8745, 0, 84, 0, 0x00000000, bhvFreeBird),
		OBJECT(MODEL_BUTTERFLY, -5560, 1473, 7461, 0, 0, 0, 0x00000000, bhvTripletButterfly),
		OBJECT(MODEL_BUTTERFLY, 112, 1068, 9439, 0, 0, 0, 0x00000000, bhvTripletButterfly),
		OBJECT(MODEL_BUTTERFLY, 1134, 823, 5459, 0, 0, 0, 0x00000000, bhvTripletButterfly),
		OBJECT(0x39, 2992, 540, 4367, 0, -120, 0, 0x00000000, bhvWindMill),
		OBJECT(0, -1398, 913, 5423, 0, 0, 0, 0x04000000, bhvHiddenRedCoinStar),
		OBJECT(0xf6, -1613, -231, 2683, 0, 0, 0, 0x000E0001, bhvWarpPipe),
		OBJECT(MODEL_STAR, -3570, 3436, 2766, 0, 0, 0, 0x00000000, bhvStar),
		TERRAIN(bob_area_1_collision),
		MACRO_OBJECTS(bob_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x01, SEQ_LEVEL_BUNNY),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(2, bob_area_2),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_BOB, 0x01, 0x0C, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x01, 0xe1, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x01, 0xd1, WARP_NO_CHECKPOINT),
		OBJECT(0xf0, -1366, -10, -1066, 0, 0, 0, 0x000a0000, bhvBounceFlower),
		OBJECT(0, -101, -74, -1759, 0, 0, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, 632, -1262, 1899, 0, 90, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -1366, 140, -1066, 0, 0, 0, 0x00010000, bhvCoinFormation),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, -1235, 444, 1838, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, -81, -454, 2191, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, -103, -77, -1718, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, -1246, -10, -2227, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, 2248, -529, -2277, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(0x37, 3040, -529, -2004, 0, 0, 0, 0x00000000, bhvGoombaBunny, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, -1580, -1314, 1242, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(0x37, 1672, -1590, 150, 0, 0, 0, 0x00000000, bhvGoombaBunny, ACT_2),
		OBJECT_WITH_ACTS(0x37, -1051, -1590, -110, 0, 0, 0, 0x00000000, bhvGoombaBunny, ACT_2),
		OBJECT_WITH_ACTS(MODEL_GOOMBA, 2330, -912, 1710, 0, 0, 0, 0x00000000, bhvGoomba, ACT_2),
		OBJECT_WITH_ACTS(0, 449, -1390, -47, 0, 0, 0, 0x01000000, bhvGoombaStarSpawn, ACT_2),
		OBJECT(0, -2522, 770, 993, 0, 90, 0, 0x200a0000, bhvAirborneWarp),
		OBJECT(0, -3512, 1142, 988, 0, 90, 0, 0x300B0000, bhvWarp),
		TERRAIN(bob_area_2_collision),
		MACRO_OBJECTS(bob_area_2_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_CARROT),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(3, bob_area_3),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0D, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x01, 0xe1, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x01, 0xd1, WARP_NO_CHECKPOINT),
		OBJECT(0, 8795, 8677, 1901, 0, 5, 0, 0x00000000, bhvTextureAnims),
		OBJECT(0, 8091, 7633, 727, 0, 5, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -9953, -292, 2870, 0, 38, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 3196, -6238, -13002, 0, 80, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -1425, 4401, 1530, 0, 27, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 7693, 7920, 4067, 0, -7, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -5127, 1098, 8485, 0, 46, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0x16, 8124, 7981, 6721, 0, -180, 0, 0x200a0000, bhvWarpPipe),
		OBJECT(0x16, 7423, -6028, -12256, 0, -180, 0, 0x000a0000, bhvWarpPipe),
		OBJECT(MODEL_STAR, 6673, -5828, -11947, 0, -180, 0, 0x03000000, bhvStar),
		TERRAIN(bob_area_3_collision),
		MACRO_OBJECTS(bob_area_3_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_SLIDER),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(4, bob_area_4),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0E, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x01, 0xe1, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x01, 0xd1, WARP_NO_CHECKPOINT),
		OBJECT(0, 1019, 0, -321, 0, 80, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0, 2043, 1200, -3213, 0, 80, 0, 0x00110000, bhvCoinFormation),
		OBJECT(MODEL_STAR, 261, 5409, -2034, 0, 0, 0, 0x05000000, bhvStar),
		OBJECT(0x16, -639, -300, -2859, 0, 0, 0, 0xC00A0000, bhvWarpPipe),
		TERRAIN(bob_area_4_collision),
		MACRO_OBJECTS(bob_area_4_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_W11),
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
