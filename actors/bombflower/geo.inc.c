#include "src/game/envfx_snow.h"

const GeoLayout bombflowergeo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bombflower_Circle_mesh),
		GEO_DISPLAY_LIST(LAYER_ALPHA, bombflower_Cloud_Flower_mesh),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_ALPHA, 0, 0, 0, bombflower_Cloud_Flower_002_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bombflower_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, bombflower_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
