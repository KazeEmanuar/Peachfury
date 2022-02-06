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
#include "levels/castle_grounds/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_castle_grounds_entry[] = {
	INIT_LEVEL(),
		OBJECT(0, 589, 288, -2226, 0, 0, 0, 0x00010000, bhvBird),
		OBJECT(0, 747, 1622, 206, 0, 0, 0, 0x00010000, bhvBird),
		OBJECT(0, 4584, -284, -2416, 0, 0, 0, 0x00040000, bhvTextureAnims),
		OBJECT(0x41, 6772, 953, -1998, 0, 0, 0, 0x0000000D, bhvSeagull),
		OBJECT(0xf7, 1206, -136, -583, 0, 93, 0, 0x00000000, bhvYoshiVillager),
		OBJECT(0xf7, 628, -130, -213, 0, 93, 0, 0x00010000, bhvYoshiVillager),
		OBJECT(0xf7, 1171, -169, -1362, 0, 93, 0, 0x00020000, bhvYoshiVillager),
		OBJECT(0xf7, -978, -191, -2292, 0, 93, 0, 0x00030000, bhvYoshiVillager),
		OBJECT(0xf7, -200, -188, -2023, 0, 93, 0, 0x00040000, bhvYoshiVillager),
		OBJECT(0xf7, 580, -182, -1789, 0, 93, 0, 0x00050000, bhvYoshiVillager),
		OBJECT(0x3E, 1690, 367, 1873, 0, 64, 0, 0x051E0004, bhvMouseTalk2),
		OBJECT(0x3E, 1544, 367, 1807, 0, -150, 0, 0x05000000, bhvMouseTalk2),
		OBJECT(0x3F, 1461, 512, 1671, 0, 0, 0, 0x05000000, bhvMinewallOW),
		TERRAIN(castle_grounds_area_1_collision),
		MACRO_OBJECTS(castle_grounds_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_OWNEW),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(2, castle_grounds_area_2),
		WARP_NODE(0x13, LEVEL_JRB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xd3, LEVEL_ENDING, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xe3, LEVEL_ENDING, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x14, LEVEL_CCM, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xd4, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xe4, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x1e, LEVEL_PSS, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xde, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xee, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x10, LEVEL_CASTLE_GROUNDS, 0x01, 0xd0, WARP_NO_CHECKPOINT),
		WARP_NODE(0xd0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x1d, LEVEL_BITDW, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xdd, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xed, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x15, LEVEL_BBH, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xd5, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xe5, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x11, LEVEL_CASTLE_GROUNDS, 0x04, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x12, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0F, LEVEL_CASTLE, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(0, 2048, -63, -11009, 0, 0, 0, 0x7015FE04, bhvWarp),
		OBJECT(0, 998, 3445, 987, 0, 0, 0, 0xFF0F0000, bhvWarp),
		OBJECT(0, 2199, 137, -8013, 0, -177, 0, 0x20E50000, bhvAirborneStarCollectWarp),
		OBJECT(0, 2192, 137, -8018, 0, 180, 0, 0x20D50000, bhvDeathWarp),
		OBJECT(0, 494, 332, -3366, 0, -180, 0, 0x20Ee0000, bhvAirborneStarCollectWarp),
		OBJECT(0, 494, 332, -3366, 0, -180, 0, 0x20De0000, bhvDeathWarp),
		OBJECT(0, -455, 84, 5392, 0, 160, 0, 0x20DD0000, bhvDeathWarp),
		OBJECT(0, -455, 84, 5392, 0, 161, 0, 0x20ED0000, bhvAirborneStarCollectWarp),
		OBJECT(0x45, -613, -201, 5686, 0, -24, 0, 0x001D0000, bhvBowserCartOW),
		OBJECT(0, 3758, -144, 343, -3, 0, -1, 0x80100000, bhvWarp),
		OBJECT(0, 2281, -144, 1039, 0, -86, 0, 0x20D00000, bhvInstantActiveWarp),
		OBJECT(0, -1095, -448, 6724, -3, -21, -1, 0x401D20000, bhvWarp),
		OBJECT(0, 4068, 738, -3911, -3, 0, -1, 0x14110000, bhvWarp),
		OBJECT(0, 4068, 738, -3753, -3, 0, -1, 0x14110000, bhvWarp),
		OBJECT(0, 3312, 686, -3849, 0, -93, 0, 0x20120000, bhvInstantActiveWarp),
		OBJECT(MODEL_COURTYARD_SPIKY_TREE, -568, -144, 2534, 0, -180, 0, 0x00000001, bhvTree),
		OBJECT(MODEL_COURTYARD_SPIKY_TREE, 1924, 79, -5456, 0, -180, 0, 0x00000001, bhvTree),
		OBJECT(MODEL_COURTYARD_SPIKY_TREE, 1403, -144, -2095, 0, -180, 0, 0x00000001, bhvTree),
		OBJECT(MODEL_COURTYARD_SPIKY_TREE, -360, -22, -1837, 0, -180, 0, 0x00000001, bhvTree),
		OBJECT(0, -879, 1032, 6185, 0, 0, 0, 0x000E0000, bhvTextureAnims),
		OBJECT(0x42, 336, 123, -3625, 0, -180, 0, 0x001E0001, bhvSeaShell),
		OBJECT(0x41, -7637, 410, -1547, 0, 0, 0, 0x00000012, bhvSeagull),
		OBJECT(0xf7, 466, -144, 1627, 0, 93, 0, 0x00060000, bhvYoshiVillager),
		OBJECT(0xf7, 393, -144, -832, 0, 93, 0, 0x00070000, bhvYoshiVillager),
		OBJECT(0xf7, 1462, -144, -412, 0, 93, 0, 0x00080000, bhvYoshiVillager),
		OBJECT(0xf7, 3563, 686, -2610, 0, 93, 0, 0x00090000, bhvYoshiVillager),
		OBJECT(0xf7, -437, -144, 231, 0, 93, 0, 0x000A0000, bhvYoshiVillager),
		OBJECT(0x43, 780, -145, 5909, 0, -124, 0, 0x00000000, bhvMechBowser),
		OBJECT(0x44, -860, -299, 6190, 0, 0, 0, 0x12000000, bhvSolidUnitlB2Stars),
		OBJECT(0x47, -398, -144, 1367, 0, -63, 0, 0x00000000, bhvFatYosher),
		OBJECT(0, 2902, 176, 4369, 0, -90, 0, 0x30140000, bhvWarp),
		OBJECT(0, 2900, 176, 4772, 0, -90, 0, 0x30140000, bhvWarp),
		OBJECT(0, 2906, 176, 5169, 0, -90, 0, 0x30140000, bhvWarp),
		OBJECT(0, 2909, 176, 5573, 0, -90, 0, 0x30140000, bhvWarp),
		OBJECT(0, 1344, 376, 4804, 0, 90, 0, 0x20E40000, bhvAirborneStarCollectWarp),
		OBJECT(0, 1339, 376, 4812, 0, -90, 0, 0x20D40000, bhvDeathWarp),
		TERRAIN(castle_grounds_area_2_collision),
		MACRO_OBJECTS(castle_grounds_area_2_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_OWNEW),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(3, castle_grounds_area_3),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_CASTLE_GROUNDS, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, -60, 25, -622, 0, 0, 0, 0x000A0000, bhvSpinAirborneWarp),
		OBJECT(MODEL_COURTYARD_WOODEN_DOOR, 1, -25, 286, 0, -180, 0, 0x000B0000, bhvDoorWarp2),
		OBJECT(MODEL_NONE, 110, 221, -247, 0, 0, 0, 0x00020000, bhvTextureAnims),
		OBJECT(0x3C, -454, 161, 446, 0, 105, 0, 0x00000000, bhvParaKarry),
		TERRAIN(castle_grounds_area_3_collision),
		MACRO_OBJECTS(castle_grounds_area_3_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_MARIOSHOUSE),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	AREA(4, castle_grounds_area_4),
		WARP_NODE(0x0A, LEVEL_ENDING, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_CASTLE_GROUNDS, 0x02, 0x12, WARP_NO_CHECKPOINT),
		WARP_NODE(0xe5, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, -1842, 150, 2006, 0, 0, 0, 0xA0FF8040, bhvLight),
		OBJECT(MODEL_NONE, -1842, 150, -1594, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(MODEL_NONE, 1758, 150, 2006, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(MODEL_NONE, 1758, 150, -1594, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(MODEL_NONE, 1223, 4000, -6004, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(MODEL_NONE, -1307, 4000, -6004, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(MODEL_NONE, 1223, 5200, -12494, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(MODEL_NONE, -1307, 5200, -12494, 0, 0, 0, 0x20FF8040, bhvLight),
		OBJECT(0, -42, 0, 2708, 0, 0, 0, 0x400B0000, bhvWarp),
		OBJECT(0, -42, 0, 2006, 0, -180, 0, 0x200A0000, bhvInstantActiveWarp),
		OBJECT(0x46, 588, -2500, -214, 0, 0, 0, 0x400B0000, bhvUpCarpet),
		OBJECT(0x46, -672, -2500, -214, 0, 0, 0, 0x400B0000, bhvUpCarpet),
		TERRAIN(castle_grounds_area_4_collision),
		MACRO_OBJECTS(castle_grounds_area_4_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_OWNEW),
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
