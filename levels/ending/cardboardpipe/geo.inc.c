#include "src/game/envfx_snow.h"

const GeoLayout cardboardpipe_geo[] = {
	GEO_CULLING_RADIUS(1000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardpipe_skinned_002_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardpipe_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
