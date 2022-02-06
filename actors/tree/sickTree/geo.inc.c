#include "src/game/envfx_snow.h"

const GeoLayout sickTree_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(4, 0, 0, 0, 0, 0, 0, sickTree_Circle_009_mesh),
		GEO_DISPLAY_LIST(0, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(1, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(2, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(3, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(5, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(6, sickTree_material_revert_render_settings),
		GEO_DISPLAY_LIST(7, sickTree_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
