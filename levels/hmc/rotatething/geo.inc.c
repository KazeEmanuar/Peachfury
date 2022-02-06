#include "src/game/envfx_snow.h"

const GeoLayout rotatething_geo[] = {
	GEO_CULLING_RADIUS(950),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, rotatething_Circle_001_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, rotatething_Circle_001_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, rotatething_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, rotatething_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
