#include "src/game/envfx_snow.h"

const GeoLayout boss2_geo[] = {
	GEO_CULLING_RADIUS(1200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, boss2_Cylinder_001_mesh_layer_1),
		GEO_DISPLAY_LIST(1, boss2_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
