#include "src/game/envfx_snow.h"

const GeoLayout windmill_geo[] = {
	GEO_CULLING_RADIUS(4500),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 0, 0, -16, 0, 0),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, 0, 0, windmill_Bone_mesh_layer_5),
			GEO_OPEN_NODE(),
				GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 1403, 0, 101, 27, 26),
				GEO_OPEN_NODE(),
					GEO_ANIMATED_PART(LAYER_TRANSPARENT, 0, 0, 0, windmill_Bone_001_mesh_layer_5),
					GEO_OPEN_NODE(),
						GEO_DISPLAY_LIST(LAYER_OPAQUE, windmill_Bone_001_mesh_layer_1),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	/*	GEO_DISPLAY_LIST(LAYER_OPAQUE, windmill_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, windmill_material_revert_render_settings),*/
	GEO_CLOSE_NODE(),
	GEO_END(),
};
