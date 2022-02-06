#include "src/game/envfx_snow.h"

const GeoLayout jelly_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_ASM(5, set_prim_to_opacity),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, jelly_Icosphere_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, jelly_Icosphere_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, jelly_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, jelly_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
