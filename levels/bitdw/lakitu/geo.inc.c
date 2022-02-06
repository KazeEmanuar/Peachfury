#include "src/game/envfx_snow.h"

const GeoLayout titlelakitu_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_BONE(LAYER_OPAQUE, 0, 0, 0, -3521, -32768, 0, lakitu_Bone_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, lakitu_Bone_001_skinned_mesh_layer_1),
			GEO_BONE(LAYER_OPAQUE, 0, 76, 0, -3509, 0, 0, lakitu_Bone_001_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_BONE(LAYER_ALPHA, 0, 31, 0, 7030, 0, 0, lakitu_Bone_002_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_BONE(LAYER_OPAQUE, 0, 68, 7, 3521, 0, 0, lakitu_Bone_003_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, lakitu_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, lakitu_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
