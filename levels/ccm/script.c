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

#include "actors/group0.h"
#include "actors/common1.h"

/* Fast64 begin persistent block [includes] */
/* Fast64 end persistent block [includes] */

#include "make_const_nonconst.h"
#include "levels/ccm/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_ccm_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0(0x07, _ccm_segment_7SegmentRomStart, _ccm_segment_7SegmentRomEnd), 
	LOAD_MIO0(0x0A, _bits_skybox_mio0SegmentRomStart, _bits_skybox_mio0SegmentRomEnd), 
	LOAD_MIO0(0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	LOAD_MIO0(0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd), 
	LOAD_RAW(0x0D, _group17_geoSegmentRomStart, _group17_geoSegmentRomEnd), 
	LOAD_MIO0(0x05, _group7_mio0SegmentRomStart, _group7_mio0SegmentRomEnd), 
	LOAD_RAW(0x0C, _group7_geoSegmentRomStart, _group7_geoSegmentRomEnd), 
	LOAD_MIO0(0xb, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	JUMP_LINK(script_func_global_8), 
	JUMP_LINK(script_func_global_18), 
	LOAD_MODEL_FROM_GEO(0x35, tireswing_geo), 
	LOAD_MODEL_FROM_GEO(0x36, toybone_geo), 
	LOAD_MODEL_FROM_GEO(0x37, floatboat_geo), 
	LOAD_MODEL_FROM_GEO(0x38, MusclePengu_geo), 
	LOAD_MODEL_FROM_GEO(0x39, sneakypengu_geo), 
	LOAD_MODEL_FROM_GEO(0x3A, smartpenguin_geo), 
	LOAD_MODEL_FROM_GEO(0x3B, bosspenguin_geo), 
	LOAD_MODEL_FROM_GEO(0x3C, chompgate_geo), 
	LOAD_MODEL_FROM_GEO(0x3D, table_geo), 
	LOAD_MODEL_FROM_GEO(0x3E, tablebroken_geo), 
	LOAD_MODEL_FROM_GEO(0x3F, zoobush_geo), 
	LOAD_MODEL_FROM_GEO(0x40, zoobush2_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_COURTYARD_SPIKY_TREE, pinetreeNew_geo), 
	LOAD_MODEL_FROM_GEO(0x41, wariocar_geo), 
	LOAD_MODEL_FROM_GEO(0x42, waluigi_geo), 
	LOAD_MODEL_FROM_GEO(0x43, wariogate_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_HMC_METAL_DOOR, metal_door_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_HMC_HAZY_MAZE_DOOR, hazy_maze_door_geo), 
	LOAD_MODEL_FROM_GEO(0x44, pengucage_geo), 
	LOAD_MODEL_FROM_GEO(0x45, wario_geo), 
	LOAD_MODEL_FROM_GEO(0x46, sellguy_geo), 
	LOAD_MODEL_FROM_GEO(0x47, banana_geo), 
	LOAD_MODEL_FROM_GEO(0x47, enginepart_geo), 
	LOAD_MODEL_FROM_GEO(0x48, wariocarbroken_geo), 
	LOAD_MODEL_FROM_GEO(0x49, pengucannon_geo), 
	LOAD_MODEL_FROM_GEO(0x4A, crab_geo), 
	LOAD_MODEL_FROM_GEO(0x4B, blooper_geo), 
	LOAD_MODEL_FROM_GEO(0x4C, smolturtle_geo), 
	LOAD_MODEL_FROM_GEO(0x4D, sushinew_geo), 
	LOAD_MODEL_FROM_GEO(0x4E, mantanew_geo), 
	LOAD_MODEL_FROM_GEO(0x4F, cheepcheepnew_geo), 
	LOAD_MODEL_FROM_GEO(0x50, penguplane_geo), 
	LOAD_MODEL_FROM_GEO(0x16, warp_pipe_geo), 
	LOAD_MODEL_FROM_GEO(0x51, skullblock_geo), 
	LOAD_MODEL_FROM_GEO(0x52, spikeball_geo), 
	LOAD_MODEL_FROM_GEO(0x53, fishbone_geo), 
	LOAD_MODEL_FROM_GEO(0x54, bitpipegreyscale_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_RED_FLAME, grey_flame_geo), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, ccm_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_CCM, 0x02, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0x0C, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0D, LEVEL_BOB, 0x03, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0x0E, LEVEL_CCM, 0x04, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x02, 0xe4, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x02, 0xd4, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0F, LEVEL_CCM, 0x03, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x10, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(0, 1301, 187, 4711, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0x36, 1588, 0, 5498, 0, -31, 0, 0x00000000, bhvChompBone),
		OBJECT(0x54, -7060, 23, 7980, 0, -180, 0, 0x000E0001, bhvWarpPipe),
		OBJECT(0, -442, -200, 9274, 0, -153, 0, 0xE00a0000, bhvAirborneWarp),
		OBJECT(0, -5160, -346, 6978, 0, 27, 0, 0x00030000, bhvTextureAnims),
		OBJECT(MODEL_GOOMBA, -8029, -713, -91, 0, 27, 0, 0x00010000, bhvGoomba),
		OBJECT(MODEL_GOOMBA, -7243, -717, -906, 0, 27, 0, 0x00000000, bhvGoomba),
		OBJECT(MODEL_GOOMBA, -6130, -718, 213, 0, 27, 0, 0x00000000, bhvGoomba),
		OBJECT(0, 4452, 253, 6264, 0, -161, 0, 0x2000B0000, bhvWarp),
		OBJECT(MODEL_DORRIE, -5014, -344, 6882, 0, 27, 0, 0x00000000, bhvDorrie),
		OBJECT(MODEL_GOOMBA, -7033, -710, 2655, 0, 27, 0, 0x00020000, bhvGoomba),
		OBJECT(0x35, -7580, 246, -1585, 0, 15, 0, 0x00000000, bhvTireSwing),
		OBJECT(MODEL_GOOMBA, -7574, -624, -1586, 0, 16, 0, 0x00000000, bhvGoombaTireSwin),
		OBJECT(MODEL_CHAIN_CHOMP, 33, -39, 4216, 0, 15, 0, 0x00000000, bhvChainChomp),
		OBJECT_WITH_ACTS(0, 2304, 474, 5627, 0, 15, 0, 0x04000000, bhvHiddenRedCoinStar, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT(MODEL_GOOMBA, -9732, -714, -597, 0, 108, 0, 0x00010000, bhvGoombaSleeping),
		OBJECT_WITH_ACTS(0x41, 2764, 256, 6260, 0, 144, 0, 0x00000000, bhvWarioCar, ACT_1 | ACT_2 | ACT_3),
		OBJECT_WITH_ACTS(0x43, 4490, 253, 6102, 0, 0, 0, 0x00000000, bhvWarioGate, ACT_1),
		OBJECT(0, 4584, 204, 5373, 0, -161, 0, 0xE00C0000, bhvAirborneWarp),
		OBJECT(0, 4319, 253, 6154, 0, -161, 0, 0x2000B0000, bhvWarp),
		OBJECT(0, 4542, 253, 6339, 0, -161, 0, 0x2000B0000, bhvWarp),
		OBJECT(0, 4247, 253, 6094, 0, -161, 0, 0x2000B0000, bhvWarp),
		OBJECT(0, 5876, 174, 3703, 0, 15, 0, 0x300F0000, bhvWarp),
		OBJECT(0, 5999, 174, 4370, 0, 15, 0, 0x300F0000, bhvWarp),
		OBJECT(0, 5932, 174, 4008, 0, 15, 0, 0x300F0000, bhvWarp),
		OBJECT(0, 4677, 139, 4274, 0, -75, 0, 0x00100000, bhvAirborneWarp),
		OBJECT(MODEL_PENGUIN, -10100, -44, -3182, 0, -31, 0, 0x00000000, bhvSmallPenguin),
		OBJECT(MODEL_PENGUIN, -7459, -44, -3694, 0, -31, 0, 0x00000000, bhvSmallPenguin),
		OBJECT(MODEL_PENGUIN, -4030, -44, -9292, 0, -31, 0, 0x00000000, bhvSmallPenguin),
		OBJECT(MODEL_PENGUIN, -6772, 156, -7810, 0, -31, 0, 0x00010000, bhvSmallPenguin),
		OBJECT(0x37, -4482, -67, -7064, 0, 0, 0, 0x00000000, bhvFloatBoatZoo),
		OBJECT_WITH_ACTS(0x38, -7419, -44, -6094, 0, 84, 0, 0x00400037, bhvMusclePenguin, ACT_1),
		OBJECT_WITH_ACTS(0x39, -8686, 256, -5357, 0, -31, 0, 0x0140004B, bhvSneakyPengu, ACT_2),
		OBJECT_WITH_ACTS(0x3A, -8293, 256, -6811, 0, 45, 0, 0x00400039, bhvSmartPengu, ACT_1),
		OBJECT_WITH_ACTS(0x3B, -8679, 556, -6132, 0, 61, 0, 0x00400038, bhvBossPenguin, ACT_1),
		OBJECT(0, -566, 73, -3540, 0, -31, 0, 0x00760000, bhvPoleGrabbing),
		OBJECT(0, 457, 69, -1292, 0, -31, 0, 0x00760000, bhvPoleGrabbing),
		OBJECT(MODEL_UKIKI, 1408, -526, -726, 0, -31, 0, 0x00000000, bhvMacroUkiki),
		OBJECT(MODEL_UKIKI, 1416, -226, -4166, 0, 157, 0, 0x00000000, bhvMacroUkiki),
		OBJECT(MODEL_UKIKI, -950, -226, -4408, 0, 35, 0, 0x00000000, bhvMacroUkiki),
		OBJECT(0x3D, 4954, 258, -457, 0, 0, 0, 0x00000000, bhvTableBreaking),
		OBJECT_WITH_ACTS(0x47, -883, -76, -396, 0, -31, 0, 0x00000000, bhvEnginePart, ACT_2),
		OBJECT_WITH_ACTS(0x47, 1, -526, -2895, 0, -31, 0, 0x00000000, bhvEnginePart, ACT_2),
		OBJECT_WITH_ACTS(0x47, -941, -226, -4479, 0, -31, 0, 0x00000000, bhvEnginePart, ACT_2),
		OBJECT_WITH_ACTS(0x47, 24, 142, -2735, 0, -31, 0, 0x00000000, bhvEnginePart, ACT_2),
		OBJECT_WITH_ACTS(0x47, 2239, -402, -1070, 0, -31, 0, 0x00000000, bhvEnginePart, ACT_2),
		OBJECT(0x3D, 228, 258, -7645, 0, 66, 0, 0x00000000, bhvTableBreaking),
		OBJECT(0x3D, 1229, 258, -7200, 0, 75, 0, 0x00000000, bhvTableBreaking),
		OBJECT(0, -5343, -593, 6479, 0, -31, 0, 0x00000000, bhvFish3),
		OBJECT(0x42, 5488, 258, -2767, 0, -102, 0, 0x00000000, bhvWaluigi),
		OBJECT_WITH_ACTS(MODEL_UKIKI, 1245, -76, -2536, 0, 106, 0, 0x01000000, bhvDemolishUkiki, 0),
		OBJECT(0x46, 3173, 259, -6700, 0, -28, 0, 0x00000000, bhvSellGuy),
		OBJECT_WITH_ACTS(0x38, 1752, 258, -4730, 0, 142, 0, 0x00400084, bhvMusclePenguin, ACT_2),
		OBJECT_WITH_ACTS(0x45, -651, -200, 8901, 0, 30, 0, 0x02400037, bhvWarioTWO, ACT_3),
		OBJECT_WITH_ACTS(0x3B, -8679, 556, -6132, 0, 61, 0, 0x00400049, bhvBossPenguin, ACT_2),
		OBJECT_WITH_ACTS(0x3A, -8293, 256, -6811, 0, 45, 0, 0x0040004A, bhvSmartPengu, ACT_2),
		OBJECT_WITH_ACTS(0x3B, -3463, -884, 3037, 0, 16, 0, 0x00400017, bhvBossPenguin, ACT_3),
		OBJECT_WITH_ACTS(0x3A, -3036, -884, 2925, 0, 1, 0, 0x00400016, bhvSmartPengu, ACT_3),
		OBJECT_WITH_ACTS(0x39, 1838, 0, 2383, 0, 41, 0, 0x01400018, bhvSneakyPengu, ACT_3),
		OBJECT_WITH_ACTS(0x38, -1601, -234, 6965, 0, 30, 0, 0x00400037, bhvMusclePenguin, ACT_3),
		OBJECT(MODEL_COURTYARD_SPIKY_TREE, -5166, -718, 745, 0, -31, 0, 0x00200000, bhvTree),
		OBJECT_WITH_ACTS(0x38, -7404, -44, -7059, 0, 15, 0, 0x00400059, bhvMusclePenguin, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x3B, -6913, -44, -5991, 0, 61, 0, 0x0040005B, bhvBossPenguin, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x39, -7893, 256, -6301, 0, -31, 0, 0x0140005D, bhvSneakyPengu, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x3A, -6497, -44, -6245, 0, 45, 0, 0x0040005E, bhvSmartPengu, ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x50, -6996, -44, -6412, 0, -161, 0, 0x00400037, bhvAirPlane, ACT_4 | ACT_5 | ACT_6),
		OBJECT(0, 457, 339, -1292, 0, -31, 0, 0x00110000, bhvCoinFormation),
		OBJECT(MODEL_EXCLAMATION_BOX, -2115, 60, 7593, 0, -76, 0, 0x00020000, bhvExclamationBox),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, 940, 862, 3704, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, -191, -36, 4353, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, 1484, 0, 3790, 0, 19, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, -1274, 6, 5404, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, -914, 686, 3403, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, -1439, -79, 4559, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, -563, 103, 5584, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_RED_COIN, 518, 340, 2880, 0, 15, 0, 0x04000000, bhvRedCoin, ACT_1 | ACT_2 | ACT_4 | ACT_5 | ACT_6),
		OBJECT(0x3C, -1711, 220, 3807, 0, 0, 0, 0x04000000, bhvChompGate),
		OBJECT(0, -2326, -74, 6219, -18, 25, 0, 0x00100000, bhvCoinFormation),
		OBJECT(0, 151, 1315, -7153, -5, -140, 1, 0x00100000, bhvCoinFormation),
		OBJECT(0, 4089, 749, 616, -5, 164, 0, 0x00100000, bhvCoinFormation),
		OBJECT(0, 1523, 223, 2300, 23, -96, -45, 0x00100000, bhvCoinFormation),
		OBJECT_WITH_ACTS(0x49, -3281, -884, 2175, 0, 0, 0, 0x04000000, bhvPenguCannon, ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT(MODEL_BLUE_COIN_SWITCH, -4324, -100, 4150, 0, 34, 0, 0x00000000, bhvBlueCoinSwitch),
		OBJECT(MODEL_BLUE_COIN, -4792, -61, 3525, 0, 34, 0, 0x00000000, bhvHiddenBlueCoin),
		OBJECT(0, -8677, -717, 881, 0, 26, 0, 0x00000001, bhvCoinFormation),
		OBJECT(MODEL_BLUE_COIN, -5558, 0, 2677, 0, 34, 0, 0x00000000, bhvHiddenBlueCoin),
		OBJECT(0, 596, -53, 5694, 0, 92, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 3873, 1010, 6505, 0, 17, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, 5956, 258, -3750, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0, 4272, 259, -6174, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0, -1232, 134, -2775, 0, 64, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, 306, 881, -4199, 0, 87, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -9864, -44, -3356, 0, 17, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, -6322, -44, -6530, 0, 17, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, -47, 2351, -2232, 0, 17, 0, 0x00020000, bhvCoinFormation),
		OBJECT(0, -47, 2351, -2232, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT(MODEL_BLUE_COIN, -6011, -144, 1823, 0, 34, 0, 0x00000000, bhvHiddenBlueCoin),
		OBJECT(MODEL_BLUE_COIN, -6569, -115, 132, 0, 34, 0, 0x00000000, bhvHiddenBlueCoin),
		OBJECT(MODEL_BLUE_COIN, -6487, -94, -1024, 0, 34, 0, 0x00000000, bhvHiddenBlueCoin),
		OBJECT(0, -892, 10, -8981, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT_WITH_ACTS(MODEL_CANNON_BASE, -3281, -584, 2175, 0, 0, 0, 0x00000000, bhvCannon, ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT(0, 3260, 911, -3461, 0, -139, 1, 0x00100000, bhvCoinFormation),
		TERRAIN(ccm_area_1_collision),
		MACRO_OBJECTS(ccm_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_THEMEPARK),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(2, ccm_area_2),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_CCM, 0x01, 0x0C, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0C, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0D, LEVEL_BOB, 0x03, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0x0E, LEVEL_BOB, 0x04, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x02, 0xe4, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x02, 0xd4, WARP_NO_CHECKPOINT),
		OBJECT(0, -57, 0, 487, 0, 0, 0, 0x6000B0000, bhvWarp),
		OBJECT(0, -51, 0, 962, 0, 0, 0, 0x600a0000, bhvAirborneWarp),
		OBJECT(MODEL_HMC_METAL_DOOR, -562, 0, 3225, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_HMC_METAL_DOOR, -562, 0, 5981, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_HMC_METAL_DOOR, -2606, -750, 12007, 0, 90, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_HMC_METAL_DOOR, -9121, -750, 9467, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_BLACK_BOBOMB, -5229, -750, 11000, 0, 0, 0, 0x0002001C, bhvPatrollingBobOmb),
		OBJECT(MODEL_BLACK_BOBOMB, -2196, -728, 7350, 0, -180, 0, 0x00010030, bhvPatrollingBobOmb),
		OBJECT(MODEL_BLACK_BOBOMB, -1595, -750, 8199, 0, 129, 0, 0x00030090, bhvPatrollingBobOmb),
		OBJECT(MODEL_BLACK_BOBOMB, -730, -750, 11936, 0, 90, 0, 0x00010060, bhvPatrollingBobOmb),
		OBJECT(MODEL_BLACK_BOBOMB, 657, -750, 10104, 0, -90, 0, 0x00010090, bhvPatrollingBobOmb),
		OBJECT(MODEL_BLACK_BOBOMB, -8182, -750, 10614, 0, 90, 0, 0x00010028, bhvPatrollingBobOmb),
		OBJECT(MODEL_BLACK_BOBOMB, -5333, -750, 9898, 0, 55, 0, 0x00030090, bhvPatrollingBobOmb),
		OBJECT(0, -4807, -163, 11000, 0, 55, 0, 0x00060000, bhvTextureAnims),
		OBJECT(0, -948, 45, 10855, 0, 80, 0, 0x00000000, bhvCoinFormation),
		OBJECT(0, -57, 0, 69, 0, 0, 0, 0x6000B0000, bhvWarp),
		OBJECT(0, -260, 374, 3298, 0, 0, 0, 0x00000000, bhvWaterDrop),
		OBJECT(0, 872, -518, 6092, 0, 0, 0, 0x00000000, bhvWaterDrop),
		OBJECT(0, 870, 628, 6281, 0, 0, 0, 0x00000000, bhvWaterDrop),
		OBJECT(0, -2366, -390, 12465, 0, 0, 0, 0x00000000, bhvWaterDrop),
		OBJECT(0, 955, -257, 12410, 0, 0, 0, 0x00000000, bhvWaterDrop),
		OBJECT(0, 201, 130, 12414, 0, 0, 0, 0x00000000, bhvWaterDrop),
		OBJECT(0x40, 1021, 0, 950, 0, 0, 0, 0x00200000, bhvBush),
		OBJECT(0x40, 441, 0, 950, 0, 0, 0, 0x00200000, bhvBush),
		OBJECT(0x40, -759, 0, 1022, 0, 0, 0, 0x00200000, bhvBush),
		OBJECT(0x40, -759, 0, 2022, 0, 0, 0, 0x00200000, bhvBush),
		OBJECT(0x40, -759, 0, 2999, 0, 0, 0, 0x00200000, bhvBush),
		OBJECT_WITH_ACTS(0x39, -10039, -600, 7864, 0, -31, 0, 0x0040003A, bhvSneakyPengu, ACT_1),
		OBJECT_WITH_ACTS(0x44, -10039, -600, 7864, 0, -31, 0, 0x00400000, bhvPenguCage, ACT_1),
		OBJECT_WITH_ACTS(0x45, -10527, -630, 9001, 0, 93, 0, 0x00000000, bhvWario, ACT_1 | ACT_2),
		OBJECT(MODEL_UKIKI, -1183, 0, 3703, 0, 77, 0, 0x0040003A, bhvPrisonUkiki),
		OBJECT_WITH_ACTS(MODEL_BLUE_COIN_SWITCH, -9742, -600, 7879, 0, 0, 0, 0x00400000, bhvBlueCoinSwitch, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_BLUE_COIN, -9442, -600, 7879, 0, 0, 0, 0x00400000, bhvHiddenBlueCoin, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_BLUE_COIN, -10042, -600, 7879, 0, 0, 0, 0x00400000, bhvHiddenBlueCoin, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_BLUE_COIN, -9742, -600, 7729, 0, 0, 0, 0x00400000, bhvHiddenBlueCoin, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(MODEL_BLUE_COIN, -9742, -600, 8029, 0, 0, 0, 0x00400000, bhvHiddenBlueCoin, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		TERRAIN(ccm_area_2_collision),
		MACRO_OBJECTS(ccm_area_2_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_JAIL),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(3, ccm_area_3),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_CCM, 0x01, 0x10, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0C, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0D, LEVEL_BOB, 0x03, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0x0E, LEVEL_BOB, 0x04, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x02, 0xe4, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x02, 0xd4, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_STAR, -1705, 1450, -3752, 0, 90, 0, 0x03000000, bhvStar),
		OBJECT(MODEL_WOODEN_SIGNPOST, -548, 0, -1420, 0, -180, 0, 0x00530000, bhvMessagePanel),
		OBJECT(MODEL_WOODEN_SIGNPOST, 1806, 100, -413, 0, -90, 0, 0x004E0000, bhvMessagePanel),
		OBJECT(0, -1766, 0, -2803, 0, 90, 0, 0x600a0000, bhvAirborneWarp),
		OBJECT(MODEL_HMC_HAZY_MAZE_DOOR, 2055, 1500, 2907, 0, 90, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_HMC_HAZY_MAZE_DOOR, 8105, 1500, 3620, 0, 90, 0, 0x00000000, bhvDoor),
		OBJECT(0, -3042, 0, -2468, 0, 15, 0, 0x300B0000, bhvWarp),
		OBJECT(0, -3042, 0, -2809, 0, 15, 0, 0x300B0000, bhvWarp),
		OBJECT(0, -3042, 0, -3174, 0, 15, 0, 0x300B0000, bhvWarp),
		OBJECT(MODEL_WOODEN_SIGNPOST, 1806, 100, 1359, 0, -90, 0, 0x00540000, bhvMessagePanel),
		OBJECT(0, 6110, 2326, 2203, 0, 90, 0, 0x00040000, bhvLargeFishGroup),
		OBJECT(0, 3799, 1939, 1316, 0, 90, 0, 0x00040000, bhvFish2),
		OBJECT(0, 12989, 1653, 1343, 0, 90, 0, 0x00040000, bhvLargeFishGroup),
		OBJECT(0, 12161, 1728, 3567, 0, 90, 0, 0x00050000, bhvLargeFishGroup),
		OBJECT(0, 12782, 1674, 5832, 0, 90, 0, 0x00040000, bhvLargeFishGroup),
		OBJECT(0x4A, 3869, 1617, 2227, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 5053, 1650, 1347, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 6422, 1771, 1343, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 6694, 1551, 2546, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 5099, 1632, 2613, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 11825, 1141, 1564, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 12734, 1490, 1562, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 12871, 1182, 2580, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 11942, 1082, 2755, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 12622, 1217, 3861, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 11821, 1203, 4821, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4A, 12801, 1342, 5554, 0, 90, 0, 0x00000000, bhvCrabKritter),
		OBJECT(0x4B, 4483, 1708, 2001, 0, 90, 0, 0x00040000, bhvBlooper),
		OBJECT(0x4B, 5715, 1908, 1540, 0, 90, 0, 0x00040000, bhvBlooper),
		OBJECT(0x4B, 6067, 1758, 2463, 0, 90, 0, 0x00040000, bhvBlooper),
		OBJECT(0x4B, 3510, 2058, 1535, 0, 90, 0, 0x00040000, bhvBlooper),
		OBJECT(0x4B, 4224, 2108, 2490, 0, 90, 0, 0x00040000, bhvBlooper),
		OBJECT(0x4D, 12936, 1413, 2819, 0, 0, 0, 0x00040000, bhvSushiShark),
		OBJECT(0x4C, 5569, 2096, 2191, 0, 90, 0, 0x00060000, bhvSmoltoise),
		OBJECT(0x4C, 6171, 1942, 1394, 0, 90, 0, 0x00000000, bhvSmoltoise),
		OBJECT(0x4C, 3731, 2294, 2381, 0, 90, 0, 0x00040000, bhvSmoltoise),
		OBJECT(0x4E, 11885, 1300, 1801, 0, 0, 0, 0x00200000, bhvMantaRay),
		OBJECT(0x4E, 13057, 1482, 2337, 0, -49, 0, 0x00300000, bhvMantaRay),
		OBJECT(0x4E, 11940, 1523, 4334, 0, 68, 0, 0x00400000, bhvMantaRay),
		OBJECT(0x4F, 12790, 1552, 5131, 0, 172, 0, 0x00200000, bhvBub),
		OBJECT(0x4F, 13270, 1888, 4541, 0, -151, 0, 0x00260000, bhvBub),
		OBJECT(0x4F, 12680, 1738, 3425, 0, -11, 0, 0x002B0000, bhvBub),
		OBJECT(0x4F, 11986, 1738, 1584, 0, 19, 0, 0x00300000, bhvBub),
		OBJECT(0x4F, 13171, 2164, 2082, 0, -28, 0, 0x00340000, bhvBub),
		OBJECT(MODEL_WOODEN_SIGNPOST, 532, 0, 2216, 0, -180, 0, 0x00550000, bhvMessagePanel),
		OBJECT(0, 2920, 1382, 2397, 0, -90, 0, 0x00570000, bhvSignOnWall),
		OBJECT(MODEL_WOODEN_SIGNPOST, -1385, 0, 3284, 0, 135, 0, 0x00560000, bhvMessagePanel),
		OBJECT(MODEL_WOODEN_SIGNPOST, -1945, 0, 1923, 0, 90, 0, 0x00580000, bhvMessagePanel),
		OBJECT(0, -1160, 1283, 1481, 0, 90, 0, 0x00FF0000, bhvPoleGrabbing),
		OBJECT(0, 506, 1219, 1534, 0, 90, 0, 0x00FF0000, bhvPoleGrabbing),
		OBJECT(0, 872, 1193, 2828, 0, 90, 0, 0x00FF0000, bhvPoleGrabbing),
		OBJECT(0, -1547, 1259, 2756, 0, 90, 0, 0x00FF0000, bhvPoleGrabbing),
		OBJECT(0, -369, 1755, 2762, 0, 90, 0, 0x00FF0000, bhvPoleGrabbing),
		OBJECT(0, 4968, 3200, 2193, 0, -132, 0, 0x00070000, bhvTextureAnims),
		OBJECT_WITH_ACTS(0x39, 2232, 3100, 61, 0, 64, 0, 0x01400051, bhvSneakyPengu, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x38, 10571, 1000, 5158, 0, -132, 0, 0x0040014D, bhvMusclePenguin, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x3B, 10856, 2636, 6327, 0, 168, 0, 0x00040050, bhvBossPenguin, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT_WITH_ACTS(0x3A, 10852, 2636, 879, 0, 12, 0, 0x00400052, bhvSmartPengu, ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
		OBJECT(0, 13326, 1117, 603, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0, 5057, 1662, 1514, 0, 17, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0, 869, 1392, 2834, 0, 17, 0, 0x00110000, bhvCoinFormation),
		TERRAIN(ccm_area_3_collision),
		MACRO_OBJECTS(ccm_area_3_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_UNDERGROUND),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(4, ccm_area_4),
		WARP_NODE(0x0A, LEVEL_CCM, 0x01, 0x0E, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_CCM, 0x01, 0x0C, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0C, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0D, LEVEL_BOB, 0x03, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0x0E, LEVEL_BOB, 0x04, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0xf0, LEVEL_CASTLE_GROUNDS, 0x02, 0xe4, WARP_NO_CHECKPOINT),
		WARP_NODE(0xf1, LEVEL_CASTLE_GROUNDS, 0x02, 0xd4, WARP_NO_CHECKPOINT),
		OBJECT(0x16, -3755, 300, 450, 0, 90, 0, 0x000A0000, bhvWarpPipe),
		OBJECT(0, 2450, 300, 566, 0, 90, 0, 0x00080000, bhvTextureAnims),
		OBJECT(0x51, -1955, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, -1355, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, -155, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 295, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 4045, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 4645, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 5245, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0, -155, 300, 450, 0, 90, 0, 0x00110000, bhvCoinFormation),
		OBJECT(0x51, 6295, 150, 1350, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 3445, 150, 450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x52, 6295, 1395, 3750, 0, 90, 0, 0x000A0000, bhvSpikeBall),
		OBJECT(0x52, 6295, 1395, 5250, 0, -90, 0, 0x000A0000, bhvSpikeBall),
		OBJECT(0x53, 6295, 225, 450, 0, 90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x53, 6295, 225, 450, 0, -90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x53, 3295, 1050, 5550, 0, 90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x53, 3295, 1050, 5550, 0, -90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x52, 1945, 2145, 3750, 0, -180, 0, 0x000A0000, bhvSpikeBall),
		OBJECT(0x51, 3295, 1050, 4650, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 2995, 1050, 4350, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 3295, 1050, 4350, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 2695, 1050, 3750, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 2395, 1050, 3600, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, 2395, 1050, 3900, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x53, 1195, 1050, 3750, 0, 90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x51, 295, 1050, 3750, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, -5, 1050, 3750, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x53, 1195, 1050, 3750, 0, -90, 0, 0x00000000, bhvFishBone),
		OBJECT(MODEL_STAR, -1655, 1250, 7275, 0, 90, 0, 0x05000000, bhvStar),
		OBJECT(0x53, -1655, 1050, 3750, 0, 90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x53, -1655, 1050, 3750, 0, -90, 0, 0x00000000, bhvFishBone),
		OBJECT(0x53, -1655, 1050, 4950, 0, -180, 0, 0x00000000, bhvFishBone),
		OBJECT(0x53, -1655, 1050, 4950, 0, 0, 0, 0x00000000, bhvFishBone),
		OBJECT(0x51, -1655, 1050, 6150, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0x51, -1655, 1050, 6450, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0, 6295, 300, 4500, 0, 0, 0, 0, bhvCoinFormation),
		OBJECT(0, -305, 1050, 3750, 0, 0, 0, 0, bhvCoinFormation),
		OBJECT(0x51, -755, 1050, 3750, 0, 90, 0, 0x000A0000, bhvSkullBlock),
		OBJECT(0, 1575, 300, -71, 0, 0, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 2175, 300, 970, 0, -180, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 6825, 300, 2324, 0, -90, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 6825, 300, 2924, 0, -90, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 5775, 300, 2474, 0, 90, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 2475, 1050, 7274, 0, 90, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 2475, 1050, 6974, 0, 90, 0, 0x000A0000, bhvBouncingFireball),
		OBJECT(0, 2475, 1050, 6674, 0, 90, 0, 0x000A0000, bhvBouncingFireball),
		TERRAIN(ccm_area_4_collision),
		MACRO_OBJECTS(ccm_area_4_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_WARIOCASTLE),
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
