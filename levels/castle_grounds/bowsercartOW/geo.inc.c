#include "src/game/envfx_snow.h"

const GeoLayout bowsercartOW_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
		GEO_SHADOW(11, 191, 200),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, bowsercartOW_Cube_mesh_layer_1),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, bowsercartOW_Cube_mesh_layer_6),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bowsercartOW_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, bowsercartOW_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
