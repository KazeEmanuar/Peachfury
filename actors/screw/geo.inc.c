#include "src/game/envfx_snow.h"

const GeoLayout screw[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(1, 0, 0, 0, 0, 0, 0, screw_Circle_mesh),
		GEO_DISPLAY_LIST(0, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(1, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(2, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(3, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(4, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(5, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(6, screw_material_revert_render_settings),
		GEO_DISPLAY_LIST(7, screw_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
