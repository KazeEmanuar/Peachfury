#include "src/game/envfx_snow.h"

const GeoLayout icequad[] = {
	GEO_CULLING_RADIUS(350),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(7, icequad_Circle_mesh_layer_7),
		GEO_DISPLAY_LIST(7, icequad_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
