#include "src/game/envfx_snow.h"

const GeoLayout castle_grounds_area_1_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_APenisHills_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_APlane_mesh),
		GEO_TRANSLATE_NODE_WITH_DL(LAYER_TRANSPARENT, -1595, 9565, -81, castle_grounds_dl_CloudOcean_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, castle_grounds_dl_rainbow_003_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_transparentHay_002_mesh),
		GEO_TRANSLATE_NODE_WITH_DL(LAYER_TRANSPARENT, 0, 20000, 0, castle_grounds_dl_WateeTransparent_001_mesh_layer_5),
		GEO_TRANSLATE_NODE_WITH_DL(LAYER_TRANSPARENT, 7912, 108, -5550, castle_grounds_dl_WaterAnimationOW_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_YachtHoo_mesh),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, castle_grounds_dl_ZEyes_mesh),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, castle_grounds_dl_ZEyes_mesh_layer_6),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, castle_grounds_dl_ZZSun_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_FORCE, castle_grounds_dl_ZZSun_mesh_layer_0),
		GEO_DISPLAY_LIST(LAYER_ALPHA, castle_grounds_dl_ZZZsand_002_mesh),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_zzzzzsAND_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_zzzzzsAND_mesh),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout castle_grounds_area_1[] = {
	GEO_NODE_SCREEN_AREA(10, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
	GEO_OPEN_NODE(),
		GEO_ZBUFFER(0),
		GEO_OPEN_NODE(),
			GEO_NODE_ORTHO(100.0000),
			GEO_OPEN_NODE(),
				GEO_BACKGROUND_COLOR(0x0001),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_ZBUFFER(1),
		GEO_OPEN_NODE(),
			GEO_CAMERA_FRUSTUM_WITH_FUNC(45.0000, 100, 30000, geo_camera_fov),
			GEO_OPEN_NODE(),
				GEO_CAMERA(CAMERA_MODE_8_DIRECTIONS, 0, 0, 0, 0, -1, 0, geo_camera_main),
				GEO_OPEN_NODE(),
					GEO_BRANCH(1, castle_grounds_area_1_geo),
					GEO_RENDER_OBJ(),
					GEO_ASM(ENVFX_MODE_NONE, geo_envfx_main),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_FORCE, castle_grounds_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_grounds_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, castle_grounds_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, castle_grounds_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, castle_grounds_dl_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};