#include "src/game/envfx_snow.h"

const GeoLayout stargate_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, stargate_Cube_005_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, stargate_Cube_005_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, stargate_Cube_001_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, stargate_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, stargate_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
