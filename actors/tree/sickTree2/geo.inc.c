#include "src/game/envfx_snow.h"

const GeoLayout sickTree2_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(4, 0, 0, 0, 0, 0, 0, sickTree2_Circle_009_mesh),
		GEO_DISPLAY_LIST(0, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(1, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(2, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(3, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(5, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(6, sickTree2_material_revert_render_settings),
		GEO_DISPLAY_LIST(7, sickTree2_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
