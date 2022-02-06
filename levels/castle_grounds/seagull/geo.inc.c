#include "src/game/envfx_snow.h"

const GeoLayout seagull_Bone_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F2_0),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt2[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F3_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt3[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F4_2),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt4[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F5_3),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt5[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F6_4),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt6[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F7_5),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt7[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F8_6),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_Bone_opt8[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4_mat_override_F9_7),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout seagull_geo[] = {
	GEO_CULLING_RADIUS(700),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(9, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_ALPHA, 0, 50, 0, seagull_Bone_001_mesh_layer_4),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, seagull_Bone_opt1),
			GEO_BRANCH(1, seagull_Bone_opt2),
			GEO_BRANCH(1, seagull_Bone_opt3),
			GEO_BRANCH(1, seagull_Bone_opt4),
			GEO_BRANCH(1, seagull_Bone_opt5),
			GEO_BRANCH(1, seagull_Bone_opt6),
			GEO_BRANCH(1, seagull_Bone_opt7),
			GEO_BRANCH(1, seagull_Bone_opt8),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, seagull_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
