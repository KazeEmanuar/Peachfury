#include "src/game/envfx_snow.h"

const GeoLayout spikeball_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, spikeball_Circle_mesh_layer_1),
		GEO_DISPLAY_LIST(1, spikeball_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
