#include "src/game/envfx_snow.h"

const GeoLayout goomflagg_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 2, 0, goomflagg_Bone_002_mesh_layer_4),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, goomflagg_Bone_003_skinned_mesh_layer_4),
			GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, -84, goomflagg_Bone_003_mesh_layer_4),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, goomflagg_Bone_004_skinned_mesh_layer_4),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, -78, goomflagg_Bone_004_mesh_layer_4),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, goomflagg_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
