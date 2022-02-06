#include "src/game/envfx_snow.h"

const GeoLayout shipwings_geo[] = {
		GEO_CULLING_RADIUS(2500),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_ALPHA, 640, 100, 0, shipwings_Bone_002_l_mesh_layer_4),
			GEO_ANIMATED_PART(LAYER_ALPHA, -640, 100, 0, shipwings_Bone_002_r_mesh_layer_4),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, shipwings_material_revert_render_settings),
	GEO_END(),
};
