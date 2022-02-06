#include "src/game/envfx_snow.h"

const GeoLayout bunnyarrows_geo[] = {
	GEO_CULLING_RADIUS(750),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, bunnyarrows_Cube_mesh_layer_1),
		GEO_DISPLAY_LIST(1, bunnyarrows_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
