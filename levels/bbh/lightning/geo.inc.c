#include "src/game/envfx_snow.h"

const GeoLayout lightning_geo[] = {
	GEO_CULLING_RADIUS(8500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(5, lightning_Cube_mesh_layer_5),
		GEO_DISPLAY_LIST(5, lightning_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
