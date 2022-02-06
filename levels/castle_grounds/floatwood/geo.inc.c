#include "src/game/envfx_snow.h"

const GeoLayout floatwood_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, floatwood_Cube_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, floatwood_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
