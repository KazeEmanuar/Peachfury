#include "src/game/envfx_snow.h"

const GeoLayout whalespout_geo[] = {
	GEO_CULLING_RADIUS(1500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(5, whalespout_Circle_mesh_layer_5),
		GEO_DISPLAY_LIST(5, whalespout_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
