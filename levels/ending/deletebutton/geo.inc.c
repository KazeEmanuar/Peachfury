#include "src/game/envfx_snow.h"

const GeoLayout deletebutton_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, deletebutton_Plane_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, deletebutton_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
