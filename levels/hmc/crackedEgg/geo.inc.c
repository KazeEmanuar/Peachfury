#include "src/game/envfx_snow.h"

const GeoLayout crackedEgg_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(0, geo_set_eggColor),
		GEO_DISPLAY_LIST(LAYER_ALPHA, crackedEgg_Icosphere_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_ALPHA, crackedEgg_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
