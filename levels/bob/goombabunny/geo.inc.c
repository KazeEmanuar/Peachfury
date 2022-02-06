#include "src/game/envfx_snow.h"

const GeoLayout goombabunny_geo[] = {
		GEO_SHADOW(1, 204, 100),
		GEO_OPEN_NODE(),
			GEO_BONE(LAYER_ALPHA, 0, 22, 0, 0, 0, 0, goombabunny_Bone_001_mesh_layer_4),
			GEO_OPEN_NODE(),
				GEO_BONE(LAYER_OPAQUE, 0, -11, 0, 18155, 0, 0, goombabunny_Bone_003_l_mesh_layer_1),
				GEO_BONE(LAYER_OPAQUE, 31, 120, -9, -949, -3419, -3126, goombabunny_Bone_005_l_mesh_layer_1),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, goombabunny_Bone_006_l_skinned_mesh_layer_1),
					GEO_BONE(LAYER_OPAQUE, 0, 33, 0, 1906, -745, -1738, goombabunny_Bone_006_l_mesh_layer_1),
				GEO_CLOSE_NODE(),
				GEO_BONE(LAYER_OPAQUE, -31, 120, -9, -949, 3419, 3126, goombabunny_Bone_005_r_mesh_layer_1),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, goombabunny_Bone_006_r_skinned_mesh_layer_1),
					GEO_BONE(LAYER_OPAQUE, 0, 33, 0, 1906, 745, 1738, goombabunny_Bone_006_r_mesh_layer_1),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_ALPHA, 0, 22, 0, goombabunny_Bone_002_mesh_layer_4),
		GEO_CLOSE_NODE(),
	/*	GEO_DISPLAY_LIST(LAYER_OPAQUE, goombabunny_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, goombabunny_material_revert_render_settings),*/
	GEO_END(),
};
