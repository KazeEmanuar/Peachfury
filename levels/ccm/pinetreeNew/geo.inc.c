#include "src/game/envfx_snow.h"

const GeoLayout pinetreeNew_geo[] = {
	GEO_CULLING_RADIUS(1000),
	GEO_OPEN_NODE(),
		GEO_SHADOW(1, 204, 400),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, pinetreeNew_Circle_009_mesh),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, pinetreeNew_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
