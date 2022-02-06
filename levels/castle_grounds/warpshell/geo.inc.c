#include "src/game/envfx_snow.h"

const GeoLayout warpshell_geo[] = {
	GEO_CULLING_RADIUS(700),
	GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(1, warpshell_DL_AnimalShell_Bevel__Shell_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(1, warpshell_material_revert_render_settings),
	GEO_END(),
};
