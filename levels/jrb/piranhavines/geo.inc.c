#include "src/game/envfx_snow.h"

const GeoLayout piranhavines_geo[] = {
	GEO_CULLING_RADIUS(5000),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, piranhavines_Bone_mesh),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavines_Bone_001_skinned),
			GEO_ANIMATED_PART(LAYER_ALPHA, 0, 422, 0, piranhavines_Bone_001_mesh),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavines_Bone_002_skinned),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 472, 0, piranhavines_Bone_002_mesh),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavines_Bone_003_skinned),
					GEO_ANIMATED_PART(LAYER_ALPHA, 0, 472, 0, piranhavines_Bone_003_mesh),
					GEO_OPEN_NODE(),
						GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavines_Bone_004_skinned),
						GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 458, 0, -2, 0, 0),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, piranhavines_Bone_004_mesh),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavines_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
