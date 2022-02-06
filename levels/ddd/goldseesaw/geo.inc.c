#include "src/game/envfx_snow.h"

const GeoLayout goldseesaw_geo[] = {
	GEO_CULLING_RADIUS(3000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, goldseesaw_wall1_002_mesh_layer_1),
		GEO_DISPLAY_LIST(1, goldseesaw_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
