#include "src/game/envfx_snow.h"

const GeoLayout hammer_geo[] = {
	GEO_CULLING_RADIUS(3200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, hammer_Cube_010_mesh),
		GEO_DISPLAY_LIST(LAYER_ALPHA, hammer_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
