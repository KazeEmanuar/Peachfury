#include "src/game/envfx_snow.h"

const GeoLayout skullblock_Bone_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, skullblock_Bone_001_mesh_layer_4_mat_override_f3d_material_001_0),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout skullblock_geo[] = {
	GEO_CULLING_RADIUS(600),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(2, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_ALPHA, skullblock_Bone_001_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, skullblock_Bone_opt1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, skullblock_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
