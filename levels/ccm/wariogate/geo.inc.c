#include "src/game/envfx_snow.h"

const GeoLayout wariogate_geo[] = {
	GEO_CULLING_RADIUS(1200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(4, wariogate_Plane_004_mesh_layer_4),
		GEO_DISPLAY_LIST(4, wariogate_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
