#include "src/game/envfx_snow.h"

const GeoLayout submarine_geo[] = {
	GEO_CULLING_RADIUS(4000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, submarine_freeplaysub_mesh),
		GEO_DISPLAY_LIST(4, submarine_freeplaysub_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, submarine_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, submarine_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
