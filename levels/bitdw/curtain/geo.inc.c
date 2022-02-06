#include "src/game/envfx_snow.h"

const GeoLayout curtain_geo[] = {
	GEO_CULLING_RADIUS(2500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, curtain_Layer1_026_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_ALPHA, curtain_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
