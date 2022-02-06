#include "src/game/envfx_snow.h"

const GeoLayout ccm_area_1_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_AAAAAAAAAAAAAAAAAAAAAAAZipLine_002_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_AAAAAAAAAAAAAAAAAAAAAAAZipLine_003_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_AAAAAAAAAGoombaCage_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, ccm_dl_AAAAAAAAAGoombaCage_001_mesh_layer_6),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_AAAAAAAAAGras_001_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, ccm_dl_AAAAAAAAAGras_002_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_AStones_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, ccm_dl_AStones_mesh_layer_4),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_AWarioHouse_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_FORCE, ccm_dl_BackGround_mesh_layer_0),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_n14b8_001_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_Plane_008_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ccm_dl_WaterAnimation_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ccm_dl_waterstill_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_ZAAAAAAAAAChompCage_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_FORCE, ccm_dl_zzzzpLANECONVEXNOZBUFFER_mesh_layer_0),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_zzzzpLANENOTCONVEX_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ccm_dl_ZZZZWATERFALL_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, ccm_dl_ZZZZWATERFALL_mesh_layer_6),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_ZZZZZZZZZZZZZMAINFLOOR_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_ALPHA, ccm_dl_ZZZZZZZZZZZZZMAINFLOOR_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout ccm_area_1[] = {
	GEO_NODE_SCREEN_AREA(10, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
	GEO_OPEN_NODE(),
		GEO_ZBUFFER(0),
		GEO_OPEN_NODE(),
			GEO_NODE_ORTHO(100.0000),
			GEO_OPEN_NODE(),
				GEO_BACKGROUND_COLOR(0x1883),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_ZBUFFER(1),
		GEO_OPEN_NODE(),
			GEO_CAMERA_FRUSTUM_WITH_FUNC(45.0000, 100, 30000, geo_camera_fov),
			GEO_OPEN_NODE(),
				GEO_CAMERA(CAMERA_MODE_8_DIRECTIONS, 0, 0, 0, 0, -1, 0, geo_camera_main),
				GEO_OPEN_NODE(),
					GEO_BRANCH(1, ccm_area_1_geo),
					GEO_RENDER_OBJ(),
					GEO_ASM(ENVFX_MODE_NONE, geo_envfx_main),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_FORCE, ccm_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, ccm_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_ALPHA, ccm_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, ccm_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, ccm_dl_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
