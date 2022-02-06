#include "src/game/envfx_snow.h"

const GeoLayout palmtree_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(1, palmtree_Circle_mesh_layer_1),
		GEO_TRANSLATE_NODE_WITH_DL(4, 0, 510, 0, palmtree_Plane_001_mesh_layer_4),
		GEO_DISPLAY_LIST(4, palmtree_material_revert_render_settings),
		GEO_DISPLAY_LIST(1, palmtree_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
