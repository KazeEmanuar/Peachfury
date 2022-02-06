#include "src/game/envfx_snow.h"

const GeoLayout star_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x9B, 100),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, star_Cube_mesh),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 0, 0, 0, star_Cube_001_mesh),
		GEO_DISPLAY_LIST(LAYER_ALPHA, star_Cube_002_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, star_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, star_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, star_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
