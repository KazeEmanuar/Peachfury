#include "src/game/envfx_snow.h"

const GeoLayout chandelier_geo[] = {
	GEO_CULLING_RADIUS(8000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, chandelier_Circle_mesh_layer_1),
		GEO_DISPLAY_LIST(4, chandelier_Circle_mesh_layer_4),
		GEO_DISPLAY_LIST(1, chandelier_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, chandelier_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
