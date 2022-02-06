#include "src/game/envfx_snow.h"

const GeoLayout tablebroken_geo[] = {
	GEO_CULLING_RADIUS(1500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, tablebroken_Plane_024_mesh_layer_1),
		GEO_DISPLAY_LIST(1, tablebroken_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
