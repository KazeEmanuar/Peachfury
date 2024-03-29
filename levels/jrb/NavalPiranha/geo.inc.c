#include "src/game/envfx_snow.h"

const GeoLayout NavalPiranha_geo[] = {
	GEO_CULLING_RADIUS(32000),
	GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 0, 0, 0, -180, 0),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, NavalPiranha_Bone_mesh),
			GEO_OPEN_NODE(),
				GEO_ASM(1, geo_get_joint_position),
				GEO_DISPLAY_LIST(LAYER_ALPHA, NavalPiranha_c_skinned),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 419, 0, NavalPiranha_c_mesh),
				GEO_OPEN_NODE(),
					GEO_ASM(2, geo_get_joint_position),
					GEO_DISPLAY_LIST(LAYER_ALPHA, NavalPiranha_d_skinned),
					GEO_ANIMATED_PART(LAYER_ALPHA, 0, 472, 0, NavalPiranha_d_mesh),
					GEO_OPEN_NODE(),
						GEO_ASM(3, geo_get_joint_position),
						GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 412, 0, 12, 0, 0),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, NavalPiranha_e_mesh),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_ALPHA, NavalPiranha_f_skinned),
								GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 112, 0, -81, 0, 0),
								GEO_OPEN_NODE(),
									GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, NavalPiranha_f_mesh),
									GEO_OPEN_NODE(),
										GEO_ASM(4, geo_get_joint_position),
										GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 565, 0, 12, -1, -14),
										GEO_OPEN_NODE(),
										GEO_ASM(5, geo_get_joint_position),
											GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
										GEO_CLOSE_NODE(),
									GEO_CLOSE_NODE(),
								GEO_CLOSE_NODE(),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, NavalPiranha_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
