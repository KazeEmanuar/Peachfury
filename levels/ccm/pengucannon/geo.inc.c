#include "src/game/envfx_snow.h"

const GeoLayout pengucannon_geo[] = {
	GEO_CULLING_RADIUS(3400),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, pengucannon_Plane_mesh_layer_1),
		GEO_DISPLAY_LIST(1, pengucannon_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
