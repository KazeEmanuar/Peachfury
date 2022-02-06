#include "src/game/envfx_snow.h"

const GeoLayout zoobush2_geo[] = {
	GEO_CULLING_RADIUS(500),
	GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, zoobush2_Circle_009_mesh),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
