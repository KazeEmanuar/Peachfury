#include "src/game/envfx_snow.h"

const GeoLayout table_geo[] = {
	GEO_CULLING_RADIUS(1500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, table_Plane_024_mesh_layer_1),
		GEO_DISPLAY_LIST(1, table_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
