#include "src/game/envfx_snow.h"

const GeoLayout banana_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SHADOW(1, 204, 35),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(1, banana_n0b0_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(1, banana_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
