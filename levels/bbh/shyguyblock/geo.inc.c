#include "src/game/envfx_snow.h"

const GeoLayout shyguyblock_Armature_002[] = {
	GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, -100, 0, 0, 0, 0),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, shyguyblock_22_222_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout shyguyblock_Armature_001[] = {
	GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, -100, 0, 0, 0, 0),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, shyguyblock_444_44444_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout shyguyblock_Armature_003[] = {
	GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, -100, 0, 0, 0, 0),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, shyguyblock_33_333_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout shyguyblock_geo[] = {
	GEO_CULLING_RADIUS(2000),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(4, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, shyguyblock_Bone_001_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, shyguyblock_Armature_002),
			GEO_BRANCH(1, shyguyblock_Armature_001),
			GEO_BRANCH(1, shyguyblock_Armature_003),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, shyguyblock_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, shyguyblock_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
