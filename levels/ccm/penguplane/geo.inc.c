#include "src/game/envfx_snow.h"

const GeoLayout penguplane_geo[] = {
	GEO_CULLING_RADIUS(1250),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, penguplane_Cube_mesh_layer_1),
		GEO_DISPLAY_LIST(4, penguplane_Cube_001_mesh_layer_4),
		GEO_DISPLAY_LIST(1, penguplane_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, penguplane_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
