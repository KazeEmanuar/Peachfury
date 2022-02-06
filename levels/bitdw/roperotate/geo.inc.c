#include "src/game/envfx_snow.h"

const GeoLayout roperotate_geo[] = {
	GEO_CULLING_RADIUS(3000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(4, roperotate_Plane_mesh_layer_4),
		GEO_DISPLAY_LIST(1, roperotate_Plane_mesh_layer_1),
		GEO_DISPLAY_LIST(1, roperotate_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, roperotate_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
