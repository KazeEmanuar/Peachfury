#include "src/game/envfx_snow.h"

const GeoLayout piranhavineswithint_geo[] = {
	GEO_CULLING_RADIUS(30000),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 0, 0, 0, -180, 0),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, piranhavineswithint_Bone_mesh),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavineswithint_Bone_001_skinned),
									GEO_ASM(0, geo_get_joint_position),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 422, 0, piranhavineswithint_Bone_001_mesh),
				GEO_OPEN_NODE(),
					GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavineswithint_Bone_002_skinned),
									GEO_ASM(1, geo_get_joint_position),
					GEO_ANIMATED_PART(LAYER_ALPHA, 0, 472, 0, piranhavineswithint_Bone_002_mesh),
					GEO_OPEN_NODE(),
						GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavineswithint_Bone_003_skinned),
									GEO_ASM(2, geo_get_joint_position),
						GEO_ANIMATED_PART(LAYER_ALPHA, 0, 472, 0, piranhavineswithint_Bone_003_mesh),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavineswithint_Bone_004_skinned),
									GEO_ASM(3, geo_get_joint_position),
							GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 458, 0, -2, 0, 0),
							GEO_OPEN_NODE(),
								GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, piranhavineswithint_Bone_004_mesh),
									GEO_ASM(4, geo_get_joint_position),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, piranhavineswithint_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
