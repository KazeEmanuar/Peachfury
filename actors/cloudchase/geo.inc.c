#include "src/game/envfx_snow.h"

const GeoLayout cloudchase[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(4, 0, 0, 0, 0, 0, 0, cloudchase_Plane_mesh),
		GEO_DISPLAY_LIST(4, cloudchase_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
