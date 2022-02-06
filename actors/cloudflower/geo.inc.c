#include "src/game/envfx_snow.h"

const GeoLayout cloudflowergeo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(4, 0, 0, 0, 0, 0, 0, cloudflower_Cloud_Flower_mesh),
		GEO_OPEN_NODE(),
			GEO_TRANSLATE_ROTATE_WITH_DL(1, 0, 0, 0, 0, 0, 0, cloudflower_Cloud_Flower_001_mesh),
			GEO_BILLBOARD_WITH_PARAMS_AND_DL(4, 0, 0, 0, cloudflower_Cloud_Flower_002_mesh),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(1, cloudflower_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, cloudflower_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
