#include "src/game/envfx_snow.h"

const GeoLayout cloud_geo[] = {
	GEO_CULLING_RADIUS(1000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, cloud_Plane_001_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, cloud_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
