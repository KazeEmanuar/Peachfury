#include "src/game/envfx_snow.h"

const GeoLayout lavafloor_geo[] = {
	GEO_CULLING_RADIUS(14000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, lavafloor_LAVA_mesh_layer_1),
		GEO_DISPLAY_LIST(1, lavafloor_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
