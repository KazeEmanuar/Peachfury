#include "src/game/envfx_snow.h"

const GeoLayout paintguy_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(0, set_prim_to_opacity),
		GEO_DISPLAY_LIST(6, paintguy_Plane_027_mesh_layer_6),
		GEO_DISPLAY_LIST(6, paintguy_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
