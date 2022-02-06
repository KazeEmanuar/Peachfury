#include "src/game/envfx_snow.h"

const GeoLayout icecube_geo[] = {
	GEO_CULLING_RADIUS(5000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, icecube_Cube_mesh_layer_1),
		GEO_DISPLAY_LIST(5, icecube_Cube_mesh_layer_5),
		GEO_DISPLAY_LIST(1, icecube_material_revert_render_settings),
		GEO_DISPLAY_LIST(5, icecube_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
