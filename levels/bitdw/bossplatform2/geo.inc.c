#include "src/game/envfx_snow.h"

const GeoLayout bossplatform2_geo[] = {
	GEO_CULLING_RADIUS(3000),
	GEO_OPEN_NODE(),
		GEO_ASM(0, geo_set_env_from_robo),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bossplatform2_Circle_002_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, bossplatform2_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
