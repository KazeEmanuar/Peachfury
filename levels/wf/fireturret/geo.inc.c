#include "src/game/envfx_snow.h"

const GeoLayout fireturret_geo[] = {
	GEO_CULLING_RADIUS(600),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, fireturret_Circle_001_mesh_layer_1),
		GEO_DISPLAY_LIST(1, fireturret_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
