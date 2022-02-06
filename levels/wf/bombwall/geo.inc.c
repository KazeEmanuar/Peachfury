#include "src/game/envfx_snow.h"

const GeoLayout bombwall_geo[] = {
	GEO_CULLING_RADIUS(800),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, bombwall_Icosphere_mesh_layer_1),
		GEO_DISPLAY_LIST(1, bombwall_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
