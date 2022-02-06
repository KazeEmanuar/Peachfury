#include "src/game/envfx_snow.h"

const GeoLayout bowserhead_geo[] = {
	GEO_CULLING_RADIUS(4200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bowserhead_mechabowser_001_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bowserhead_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
