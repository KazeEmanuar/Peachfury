#include "src/game/envfx_snow.h"

const GeoLayout crab_Bone_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, crab_Bone_002_mesh_layer_4_mat_override_f3d_material_001_0),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout crab_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(2, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, crab_Bone_002_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, crab_Bone_opt1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, crab_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
