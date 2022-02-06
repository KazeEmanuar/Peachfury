#include "src/game/envfx_snow.h"

const GeoLayout silverstar_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, silverstar_Cube_mesh),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 0, 0, 0, silverstar_Cube_001_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, silverstar_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, silverstar_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
