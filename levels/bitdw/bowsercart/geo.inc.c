#include "src/game/envfx_snow.h"

const GeoLayout bowsercart_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_SHADOW(11, 191, 200),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(1, bowsercart_Cube_mesh_layer_1),
			GEO_DISPLAY_LIST(6, bowsercart_Cube_mesh_layer_6),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(1, bowsercart_material_revert_render_settings),
		GEO_DISPLAY_LIST(6, bowsercart_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
