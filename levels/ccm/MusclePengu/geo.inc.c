#include "src/game/envfx_snow.h"

const GeoLayout MusclePengu_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x96, 200),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_Bone_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_Bone_002_skinned_mesh_layer_1),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, -160, 0, 0),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 74, 0, 58, 0, 0),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_Bone_002_mesh_layer_1),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, 21, 0, 0),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 28, 0, 22, 0, 0),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_Bone_008_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_ALPHA, MusclePengu_Bone_008_mesh_layer_4),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_thigh_l_skinned_mesh_layer_1),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, 156, 90, 12),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 70, 0, 24, -3, -12),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_thigh_l_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_foot_l_skinned_mesh_layer_1),
							GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 33, 0, -3, -2, 88),
							GEO_OPEN_NODE(),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_foot_l_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_thigh_r_skinned_mesh_layer_1),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, 156, -90, -12),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 70, 0, 24, 3, 12),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_thigh_r_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_foot_r_skinned_mesh_layer_1),
							GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 33, 0, -3, 2, -88),
							GEO_OPEN_NODE(),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_foot_r_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, 0, 0, -86),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 63, 0, 0, 0, -4),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_arm_l_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_forearm_l_skinned_mesh_layer_1),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 50, 0, MusclePengu_forearm_l_mesh_layer_1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, 0, 0, 86),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 63, 0, 0, 0, 4),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, MusclePengu_arm_r_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_forearm_r_skinned_mesh_layer_1),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 50, 0, MusclePengu_forearm_r_mesh_layer_1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, MusclePengu_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, MusclePengu_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
