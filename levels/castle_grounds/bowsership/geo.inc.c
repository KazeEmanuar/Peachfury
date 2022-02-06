#include "src/game/envfx_snow.h"

const GeoLayout bowsership_geo[] = {
	GEO_CULLING_RADIUS(20000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bowsership_hat_kuppaship_color1_png_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bowsership_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
