#include "src/game/envfx_snow.h"

const GeoLayout stardomedoor_geo[] = {
	GEO_CULLING_RADIUS(1000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, stardomedoor_thing_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, stardomedoor_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
