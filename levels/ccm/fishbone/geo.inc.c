#include "src/game/envfx_snow.h"

const GeoLayout fishbone_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(4, fishbone_Plane_mesh_layer_4),
		GEO_DISPLAY_LIST(4, fishbone_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
