#include "src/game/envfx_snow.h"

const GeoLayout underwaterbush_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, underwaterbush_Circle_009_mesh),
		GEO_DISPLAY_LIST(LAYER_ALPHA, underwaterbush_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
