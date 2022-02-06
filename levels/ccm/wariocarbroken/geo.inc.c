#include "src/game/envfx_snow.h"

const GeoLayout wariocarbroken_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, wariocarbroken_polygon1_mesh_layer_1),
		GEO_DISPLAY_LIST(1, wariocarbroken_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
