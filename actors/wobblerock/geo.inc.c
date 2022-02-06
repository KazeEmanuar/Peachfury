#include "src/game/envfx_snow.h"

const GeoLayout wobblerock[] = {
   GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(1, 0, 0, 0, 0, 0, 0, wobblerock_Circle_mesh),
		GEO_DISPLAY_LIST(1, wobblerock_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
