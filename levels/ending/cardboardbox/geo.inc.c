#include "src/game/envfx_snow.h"

const GeoLayout cardboardbox_geo[] = {
	GEO_CULLING_RADIUS(30000),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, -500, 0, cardboardbox_Bone_mesh_layer_4),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, cardboardbox_Bone_005_skinned_mesh_layer_4),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardbox_Bone_005_skinned_mesh_layer_1),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 500, 0, 0, 0, 35),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 3127, 0, 0, 0, 86),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, cardboardbox_Bone_005_mesh_layer_4),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardbox_Bone_005_mesh_layer_1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, cardboardbox_Bone_006_skinned_mesh_layer_4),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 500, 0, -26, 0, 0),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 2840, 0, -95, 0, 0),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, cardboardbox_Bone_006_mesh_layer_4),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, cardboardbox_Bone_007_skinned_mesh_layer_4),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardbox_Bone_007_skinned_mesh_layer_1),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 500, 0, 0, 0, -35),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 3127, 0, 0, 0, -86),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, cardboardbox_Bone_007_mesh_layer_4),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardbox_Bone_007_mesh_layer_1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
			GEO_DISPLAY_LIST(LAYER_ALPHA, cardboardbox_Bone_008_skinned_mesh_layer_4),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 500, 0, 26, 0, 0),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 2840, 0, 95, 0, 0),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_ALPHA, 0, 0, 0, cardboardbox_Bone_008_mesh_layer_4),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, cardboardbox_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, cardboardbox_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
