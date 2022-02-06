#include "src/game/envfx_snow.h"

const GeoLayout wariocar_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, wariocar_polygon1_mesh_layer_1),
		GEO_DISPLAY_LIST(1, wariocar_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
