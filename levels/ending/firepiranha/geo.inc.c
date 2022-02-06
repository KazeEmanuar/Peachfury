#include "src/game/envfx_snow.h"

const GeoLayout firepiranha_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_ALPHA, 0, 21, 0, firepiranha_Bone_005_mesh_layer_4),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, firepiranha_Bone_001_skinned_mesh_layer_4),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, firepiranha_Bone_001_mesh_layer_4),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_ALPHA, firepiranha_Bone_002_skinned_mesh_layer_4),
					GEO_ANIMATED_PART(LAYER_ALPHA, 0, 49, 0, firepiranha_Bone_002_mesh_layer_4),
					GEO_OPEN_NODE(),
						GEO_DISPLAY_LIST(LAYER_ALPHA, firepiranha_Bone_003_skinned_mesh_layer_4),
						GEO_ANIMATED_PART(LAYER_ALPHA, 0, 51, 0, firepiranha_Bone_003_mesh_layer_4),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_ALPHA, firepiranha_Bone_004_skinned_mesh_layer_4),
							GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 6, 0, -1, 91, 38),
							GEO_OPEN_NODE(),
								GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, firepiranha_Bone_004_mesh_layer_4),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 21, 0, 1, 0, -51),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, firepiranha_Bone_006_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 21, 0, 1, 0, 51),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, firepiranha_Bone_007_mesh_layer_4),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, firepiranha_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
