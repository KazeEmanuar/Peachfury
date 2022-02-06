#include "src/game/envfx_snow.h"

const GeoLayout arm1_geo[] = {
	GEO_CULLING_RADIUS(4200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, arm1_mechabowser_002_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, arm1_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
