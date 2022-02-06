#include "src/game/envfx_snow.h"

const GeoLayout mips_geo[] = {
		GEO_SHADOW(0, 150, 100),
		GEO_OPEN_NODE(),
			GEO_SCALE(LAYER_FORCE, 16384),
			GEO_OPEN_NODE(),
				GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
				GEO_OPEN_NODE(),
					GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, mips_000_offset_001_mesh),
					GEO_OPEN_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, 64, 0, 0, mips_000_offset_002_mesh),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 52, -24, -32, NULL),
							GEO_OPEN_NODE(),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, mips_000_offset_004_mesh),
								GEO_OPEN_NODE(),
									GEO_DISPLAY_LIST(LAYER_OPAQUE, mips_000_offset_005_skinned),
									GEO_ANIMATED_PART(LAYER_OPAQUE, 79, 0, 0, mips_000_offset_005_mesh),
								GEO_CLOSE_NODE(),
							GEO_CLOSE_NODE(),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 52, -24, 32, NULL),
							GEO_OPEN_NODE(),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, mips_000_offset_006_mesh),
								GEO_OPEN_NODE(),
									GEO_DISPLAY_LIST(LAYER_OPAQUE, mips_000_offset_007_skinned),
									GEO_ANIMATED_PART(LAYER_OPAQUE, 79, 0, 0, mips_000_offset_007_mesh),
								GEO_CLOSE_NODE(),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, -31, 4, -39, NULL),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, mips_000_offset_009_skinned),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 45, 0, 0, mips_000_offset_009_mesh),
								GEO_OPEN_NODE(),
									GEO_DISPLAY_LIST(LAYER_OPAQUE, mips_000_offset_010_skinned),
									GEO_ANIMATED_PART(LAYER_OPAQUE, 52, 0, 0, mips_000_offset_010_mesh),
								GEO_CLOSE_NODE(),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
						GEO_ANIMATED_PART(LAYER_OPAQUE, -31, 4, 40, NULL),
						GEO_OPEN_NODE(),
							GEO_ANIMATED_PART(LAYER_OPAQUE, 0, 0, 0, NULL),
							GEO_OPEN_NODE(),
								GEO_DISPLAY_LIST(LAYER_OPAQUE, mips_000_offset_012_skinned),
								GEO_ANIMATED_PART(LAYER_OPAQUE, 45, 0, 0, mips_000_offset_012_mesh),
								GEO_OPEN_NODE(),
									GEO_DISPLAY_LIST(LAYER_OPAQUE, mips_000_offset_013_skinned),
									GEO_ANIMATED_PART(LAYER_OPAQUE, 52, 0, 0, mips_000_offset_013_mesh),
								GEO_CLOSE_NODE(),
							GEO_CLOSE_NODE(),
						GEO_CLOSE_NODE(),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	GEO_END(),
};
