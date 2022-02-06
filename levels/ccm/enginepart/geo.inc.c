#include "src/game/envfx_snow.h"

const GeoLayout enginepart_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xC8, 100),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, enginepart_Circle_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_ALPHA, enginepart_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
