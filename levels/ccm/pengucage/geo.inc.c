#include "src/game/envfx_snow.h"

const GeoLayout pengucage_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(4, pengucage_Cube_mesh_layer_4),
		GEO_DISPLAY_LIST(4, pengucage_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
