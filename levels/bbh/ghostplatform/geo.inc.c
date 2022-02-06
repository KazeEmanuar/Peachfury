#include "src/game/envfx_snow.h"

const GeoLayout ghostplatform_Armature_001[] = {
	GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 100, 0, 0, 0, 0),
	GEO_RETURN(),
};
const GeoLayout ghostplatform_geo[] = {
	GEO_CULLING_RADIUS(1500),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ghostplatform_Bone_mesh_layer_1),
		GEO_SWITCH_CASE(2, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ghostplatform_Bone_002_mesh_layer_5),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, ghostplatform_Armature_001),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ghostplatform_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ghostplatform_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
