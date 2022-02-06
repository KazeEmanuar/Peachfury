#include "src/game/envfx_snow.h"

const GeoLayout warp_pipe_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, warp_pipe_skinned_mesh),
		//GEO_DISPLAY_LIST(LAYER_OPAQUE, warp_pipe_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
