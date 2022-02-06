#include "src/game/envfx_snow.h"

const GeoLayout nessie_geo[] = {
	GEO_CULLING_RADIUS(600),
	GEO_OPEN_NODE(),
		GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, nessie_Bone_mesh_layer_1),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 167, -12, nessie_Bone_001_mesh_layer_1),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, nessie_Bone_002_skinned_mesh_layer_1),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 59, 79, nessie_Bone_002_mesh_layer_1),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, nessie_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
