#include "src/game/envfx_snow.h"

const GeoLayout sellguy_geo[] = {
	GEO_CULLING_RADIUS(300),
	GEO_OPEN_NODE(),
		GEO_BONE(LAYER_OPAQUE, 0, 61, 28, -5503, 0, 0, sellguy_Bone_002_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_BONE(LAYER_OPAQUE, 44, 53, -23, -3388, -3426, -25526, sellguy_arm_l_mesh_layer_1),
			GEO_BONE(LAYER_OPAQUE, -44, 53, -23, -3388, 3426, 25526, sellguy_arm_r_mesh_layer_1),
			GEO_BONE(LAYER_OPAQUE, 22, -24, -93, 13425, -5111, 26057, sellguy_foot_l_mesh_layer_1),
			GEO_BONE(LAYER_OPAQUE, -22, -24, -93, 13425, 5111, -26057, sellguy_foot_r_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, sellguy_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
