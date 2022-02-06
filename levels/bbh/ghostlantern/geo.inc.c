#include "src/game/envfx_snow.h"

const GeoLayout ghostlantern_geo[] = {
	GEO_CULLING_RADIUS(1500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, ghostlantern_Plane_mesh_layer_1),
		GEO_DISPLAY_LIST(4, ghostlantern_Plane_mesh_layer_4),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 0, 0, 0, ghostlantern_Cube_001_mesh),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ghostlantern_material_revert_render_settings),
		GEO_DISPLAY_LIST(1, ghostlantern_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, ghostlantern_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
