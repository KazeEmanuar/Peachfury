#include "src/game/envfx_snow.h"

const GeoLayout highlight_geo[] = {
		GEO_CULLING_RADIUS(2500),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_FORCE, highlight_Circle_mesh),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT, highlight_Circle_001_mesh),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_FORCE, highlight_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, highlight_material_revert_render_settings),
	GEO_END(),
};
