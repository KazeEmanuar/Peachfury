#include "src/game/envfx_snow.h"

const GeoLayout reznor_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, reznor_Bone_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_Bleg1_l_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 50, 71, -64, reznor_Bleg1_l_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_bfoot1_l_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 3, -34, 0, reznor_bfoot1_l_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_Bleg1_r_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, -50, 71, -64, reznor_Bleg1_r_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_bfoot1_r_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, -3, -34, 0, reznor_bfoot1_r_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 100, 84, reznor_Bone_002_mesh_layer_1),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_Bone_004_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 102, -96, reznor_Bone_004_mesh_layer_1),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_Fleg_l_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 55, 76, 52, reznor_Fleg_l_mesh_layer_1),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_Fleg_r_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, -55, 76, 52, reznor_Fleg_r_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, reznor_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
