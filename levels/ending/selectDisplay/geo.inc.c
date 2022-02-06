#include "src/game/envfx_snow.h"

const GeoLayout selectDisplay_geo[] = {
	GEO_CULLING_RADIUS(32000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, selectDisplay_dsplay_mesh),
		GEO_DISPLAY_LIST(LAYER_ALPHA, selectDisplay_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
