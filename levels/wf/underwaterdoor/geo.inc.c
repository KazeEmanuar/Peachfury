#include "src/game/envfx_snow.h"


const GeoLayout underwaterdoor_geo[] = {
		GEO_CULLING_RADIUS(1200),
		GEO_OPEN_NODE(),
				GEO_TRANSLATE_ROTATE(LAYER_OPAQUE, 0, 247, 0, 180, 0, 0),
					GEO_OPEN_NODE(),
					GEO_SWITCH_CASE(2, geo_switch_anim_state),
						GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, underwaterdoor_Bone_002_mesh_layer_1),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, underwaterdoor_Bone_002_mesh_layer_1_mat_override_closeddoor_0),
			GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, underwaterdoor_material_revert_render_settings),
		GEO_CLOSE_NODE(),
	GEO_END(),
};
