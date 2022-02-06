#include "src/game/envfx_snow.h"

const GeoLayout bounceShroom2_geo[] = {
	GEO_CULLING_RADIUS(650),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bounceShroom2_GamemodelShape_001_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bounceShroom2_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
