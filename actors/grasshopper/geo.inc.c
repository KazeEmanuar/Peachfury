#include "src/game/envfx_snow.h"

const GeoLayout grasshopper_Bone_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, grasshopper_Bone_001_mesh_layer_4_mat_override_f3d_material_005_0),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout grasshopper_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(2, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, grasshopper_Bone_001_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, grasshopper_Bone_opt1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, grasshopper_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
