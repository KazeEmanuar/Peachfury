#include "src/game/envfx_snow.h"

const GeoLayout fallplatform1_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, fallplatform1_Cube_mesh_layer_1),
		GEO_DISPLAY_LIST(1, fallplatform1_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
