#include "src/game/envfx_snow.h"

const GeoLayout trex_geo[] = {
	GEO_CULLING_RADIUS(5000),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, trex_Bone_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, trex_Bone_mesh_layer_4),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_002_l_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 218, 571, 0, trex_Bone_002_l_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_003_l_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 43, -361, -113, trex_Bone_003_l_mesh_layer_1),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_004_l_skinned_mesh_layer_1),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 3, -180, 165, trex_Bone_004_l_mesh_layer_1),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_002_r_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, -218, 571, 0, trex_Bone_002_r_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_003_r_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, -43, -361, -113, trex_Bone_003_r_mesh_layer_1),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_004_r_skinned_mesh_layer_1),
					GEO_ANIMATED_PART(LAYER_OPAQUE, -3, -180, 165, trex_Bone_004_r_mesh_layer_1),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_005_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 527, -350, trex_Bone_005_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_006_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, -155, -218, trex_Bone_006_mesh_layer_1),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_007_skinned_mesh_layer_1),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 0, -106, -399, trex_Bone_007_mesh_layer_1),
					GEO_OPEN_NODE(),
						GEO_DISPLAY_LIST(LAYER_ALPHA, trex_Bone_007_mesh_layer_4),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_010_l_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 163, 778, 456, trex_Bone_010_l_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_011_l_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, -113, 34, trex_Bone_011_l_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_010_r_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, -163, 778, 456, trex_Bone_010_r_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Bone_011_r_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, -113, 34, trex_Bone_011_r_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Head_001_skinned_mesh_layer_1),
			GEO_DISPLAY_LIST(LAYER_ALPHA, trex_Head_001_skinned_mesh_layer_4),
			GEO_ASM(4, geo_trex_head_movement),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 1029, 443, 0, 0, 0),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Head_001_mesh_layer_1),
				GEO_DISPLAY_LIST(LAYER_ALPHA, trex_Head_001_mesh_layer_4),
				GEO_DISPLAY_LIST(LAYER_TRANSPARENT, trex_Head_001_mesh_layer_5),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Jaw2_skinned_mesh_layer_1),
				GEO_DISPLAY_LIST(LAYER_ALPHA, trex_Jaw2_skinned_mesh_layer_4),
				GEO_ASM(0, geo_trex_head_movement),
				GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 49, 193, 0, 0, 0),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_Jaw2_mesh_layer_1),
					GEO_DISPLAY_LIST(LAYER_ALPHA, trex_Jaw2_mesh_layer_4),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, trex_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, trex_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, trex_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
