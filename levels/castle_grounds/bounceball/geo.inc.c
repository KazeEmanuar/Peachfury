#include "src/game/envfx_snow.h"

const GeoLayout bounceball_geo[] = {
	GEO_CULLING_RADIUS(400),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bounceball_Icosphere_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bounceball_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
