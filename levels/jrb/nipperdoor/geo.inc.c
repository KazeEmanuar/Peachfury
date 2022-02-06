#include "src/game/envfx_snow.h"

const GeoLayout nipperdoor_geo[] = {
	GEO_CULLING_RADIUS(1200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, nipperdoor_Circle_042_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, nipperdoor_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
