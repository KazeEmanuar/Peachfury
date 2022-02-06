#include "src/game/envfx_snow.h"

const GeoLayout chompgate_geo[] = {
	GEO_CULLING_RADIUS(2500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(4, chompgate_Plane_018_mesh_layer_4),
		GEO_DISPLAY_LIST(4, chompgate_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
