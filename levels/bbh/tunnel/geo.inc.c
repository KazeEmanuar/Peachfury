#include "src/game/envfx_snow.h"

const GeoLayout tunnel_geo[] = {
	GEO_CULLING_RADIUS(7000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, tunnel_Plane_002_mesh_layer_1),
		GEO_DISPLAY_LIST(1, tunnel_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
