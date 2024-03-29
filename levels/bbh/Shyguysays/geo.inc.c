#include "src/game/envfx_snow.h"

const GeoLayout Shyguysays_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, -50, 0, Shyguysays_Bone_mesh_layer_5),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT, Shyguysays_Bone_001_skinned_mesh_layer_5),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 71, 0, 18, 0, 0),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, 0, 0, Shyguysays_Bone_001_mesh_layer_5),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT, Shyguysays_Bone_003_skinned_mesh_layer_5),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 71, 0, -128, 0, 0),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 53, 0, 56, 0, 0),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, 0, 0, Shyguysays_Bone_003_mesh_layer_5),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT, Shyguysays_arm_l_skinned_mesh_layer_5),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 71, 0, 25, -27, -112),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 41, 0, 24, -2, -8),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, 0, 0, Shyguysays_arm_l_mesh_layer_5),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, Shyguysays_arm_l_mesh_layer_1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT, Shyguysays_arm_r_skinned_mesh_layer_5),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 71, 0, 25, 27, 112),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 41, 0, 24, 2, 8),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, 0, 0, Shyguysays_arm_r_mesh_layer_5),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, Shyguysays_arm_r_mesh_layer_1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, Shyguysays_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, Shyguysays_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
