#include "src/game/envfx_snow.h"

const GeoLayout cloudspawn[] = {
	GEO_CULLING_RADIUS(8096),
	GEO_OPEN_NODE(),
		GEO_SHADOW(11, 204, 400),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, cloudspawn_n0b0_mesh),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, cloudspawn_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
