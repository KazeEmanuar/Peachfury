#include "src/game/envfx_snow.h"

const GeoLayout rumbottle_geo[] = {
	GEO_CULLING_RADIUS(200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, rumbottle_Plane_mesh_layer_1),
		GEO_DISPLAY_LIST(1, rumbottle_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
