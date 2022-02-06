#include "src/game/envfx_snow.h"

const GeoLayout pumpking_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, pumpking_cullthispumpkin_mesh),
		GEO_DISPLAY_LIST(LAYER_ALPHA, pumpking_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
