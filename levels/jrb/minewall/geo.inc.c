#include "src/game/envfx_snow.h"

const GeoLayout minewall_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, minewall_Circle_022_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, minewall_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
