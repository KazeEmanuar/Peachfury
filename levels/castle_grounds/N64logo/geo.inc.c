#include "src/game/envfx_snow.h"

const GeoLayout N64logo_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, N64logo_CrtDisplay_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, N64logo_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
