#include "src/game/envfx_snow.h"

const GeoLayout zoobush_geo[] = {
	GEO_CULLING_RADIUS(800),
	GEO_OPEN_NODE(),
		GEO_SHADOW(2, 178, 200),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, zoobush_Circle_009_mesh),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, zoobush_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
