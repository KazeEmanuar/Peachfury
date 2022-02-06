#include "src/game/envfx_snow.h"

const GeoLayout boss1_geo[] = {
	GEO_CULLING_RADIUS(1200),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, boss1_Cylinder_002_mesh_layer_1),
		GEO_DISPLAY_LIST(1, boss1_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
