#include "src/game/envfx_snow.h"

const GeoLayout ukiki_000_switch_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_NODE_START(),
		GEO_OPEN_NODE(),
			GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, ukiki_000_offset_002_mesh_layer_1_mat_override_eyes_0),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout ukiki_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SHADOW(0, 150, 80),
		GEO_OPEN_NODE(),
			GEO_SCALE(LAYER_FORCE, 16384),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, ukiki_000_offset_mesh_layer_1),
				GEO_OPEN_NODE(),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 99, -11, NULL),
					GEO_OPEN_NODE(),
						GEO_SWITCH_CASE(2, geo_switch_anim_state),
						GEO_OPEN_NODE(),
							GEO_NODE_START(),
							GEO_OPEN_NODE(),
								GEO_NODE_START(),
								GEO_OPEN_NODE(),
									GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, ukiki_000_offset_002_mesh_layer_1),
								GEO_CLOSE_NODE(),
							GEO_CLOSE_NODE(),
							GEO_BRANCH(1, ukiki_000_switch_opt1),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_004_skinned_mesh_layer_1),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 71, 69, -9, NULL),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 86, 0, 0, ukiki_000_offset_004_mesh_layer_1),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_005_skinned_mesh_layer_1),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 64, 0, 0, ukiki_000_offset_005_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
					GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_007_skinned_mesh_layer_1),
					GEO_ANIMATED_PART(LAYER_OPAQUE, -71, 68, -9, NULL),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 86, 0, 0, ukiki_000_offset_007_mesh_layer_1),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_008_skinned_mesh_layer_1),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 64, 0, 0, ukiki_000_offset_008_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 43, -22, -1, NULL),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, ukiki_000_offset_009_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_010_skinned_mesh_layer_1),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 46, 0, 0, ukiki_000_offset_010_mesh_layer_1),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_011_skinned_mesh_layer_1),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 36, 0, 0, ukiki_000_offset_011_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
					GEO_ANIMATED_PART(LAYER_OPAQUE, -43, -22, -1, NULL),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, ukiki_000_offset_012_mesh_layer_1),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_013_skinned_mesh_layer_1),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 46, 0, 0, ukiki_000_offset_013_mesh_layer_1),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_014_skinned_mesh_layer_1),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 36, 0, 0, ukiki_000_offset_014_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 0, -15, -57, NULL),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
						GEO_OPEN_NODE(),
							GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_016_skinned_mesh_layer_1),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 45, 0, 0, ukiki_000_offset_016_mesh_layer_1),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_000_offset_017_skinned_mesh_layer_1),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 46, 0, 0, ukiki_000_offset_017_mesh_layer_1),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ukiki_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
