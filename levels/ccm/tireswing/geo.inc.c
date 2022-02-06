#include "src/game/envfx_snow.h"

const GeoLayout tireswing_geo[] = {
	GEO_CULLING_RADIUS(2200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, tireswing_taiya1_001_mesh_layer_1),
		GEO_DISPLAY_LIST(1, tireswing_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
