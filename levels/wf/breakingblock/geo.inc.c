#include "src/game/envfx_snow.h"

const GeoLayout breakingblock_geo[] = {
	GEO_CULLING_RADIUS(600),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, breakingblock_Circle_030_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, breakingblock_Transparent_mesh_layer_6),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, breakingblock_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, breakingblock_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
