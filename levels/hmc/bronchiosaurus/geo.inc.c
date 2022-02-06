#include "src/game/envfx_snow.h"

const GeoLayout bronchiosaurus_geo[] = {
	GEO_CULLING_RADIUS(32000),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, -1488, 651, NULL),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, bronchiosaurus_Bone_004_skinned_mesh_layer_1),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 1270, -651, bronchiosaurus_Bone_004_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, bronchiosaurus_Bone_004_mesh_layer_4),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bronchiosaurus_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, bronchiosaurus_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
