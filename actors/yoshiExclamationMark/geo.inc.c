#include "src/game/envfx_snow.h"

const GeoLayout yoshiExclamationMark_Bone_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 3, 0, yoshiExclamationMark_Bone_001_mesh_layer_4_mat_override_f3d_material_005_0),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout yoshiExclamationMark_Bone_opt2[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 3, 0, yoshiExclamationMark_Bone_001_mesh_layer_4_mat_override_f3d_material_006_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout yoshiExclamationMark_Bone_opt3[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 3, 0, yoshiExclamationMark_Bone_001_mesh_layer_4_mat_override_f3d_material_007_2),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout yoshiExclamationMark_Bone_opt4[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 3, 0, yoshiExclamationMark_Bone_001_mesh_layer_4_mat_override_f3d_material_008_3),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout yoshiExclamationMark_Bone_opt5[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 3, 0, yoshiExclamationMark_Bone_001_mesh_layer_4_mat_override_f3d_material_009_4),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout yoshiExclamationMark_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(6, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 3, 0, yoshiExclamationMark_Bone_001_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, yoshiExclamationMark_Bone_opt1),
			GEO_BRANCH(1, yoshiExclamationMark_Bone_opt2),
			GEO_BRANCH(1, yoshiExclamationMark_Bone_opt3),
			GEO_BRANCH(1, yoshiExclamationMark_Bone_opt4),
			GEO_BRANCH(1, yoshiExclamationMark_Bone_opt5),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, yoshiExclamationMark_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
