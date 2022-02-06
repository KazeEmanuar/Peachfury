#include "src/game/envfx_snow.h"

const GeoLayout birds_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_BONE(LAYER_OPAQUE, 0, 0, 0, 0, 0, 0, bird_Bone_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_BONE(LAYER_OPAQUE, 10, 22, 9, 0, 8891, -16384, bird_bird_l_mesh_layer_1),
			GEO_BONE(LAYER_OPAQUE, -10, 22, 9, 0, -8891, 16384, bird_bird_r_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bird_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
