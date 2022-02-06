#include "src/game/envfx_snow.h"

const GeoLayout bonzaibob_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_SHADOW(11, 204, 125),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, bonzaibob_bonzaibill_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bonzaibob_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
