// 0x16000FE8
const GeoLayout bubbly_tree_geo[] = {
   GEO_CULLING_RADIUS(800),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_ALPHA, tree_seg3_dl_0302FEE8),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16001000
const GeoLayout spiky_tree_geo[] = {
   GEO_CULLING_RADIUS(800),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_ALPHA, tree_seg3_dl_03030FA0),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16001018
const GeoLayout snow_tree_geo[] = {
   GEO_CULLING_RADIUS(800),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_ALPHA, tree_seg3_dl_03032088),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16001030
const GeoLayout spiky_tree1_geo[] = {
   GEO_CULLING_RADIUS(800),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_ALPHA, tree_seg3_dl_03032170),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16001048
const GeoLayout palm_tree_geo[] = {
   GEO_CULLING_RADIUS(800),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_ALPHA, tree_seg3_dl_03033258),
   GEO_CLOSE_NODE(),
   GEO_END(),
};


#include "src/game/envfx_snow.h"

const GeoLayout sickTree_geo[] = {
      GEO_CULLING_RADIUS(1500),
	   GEO_OPEN_NODE(),
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xa8, 375),
      GEO_OPEN_NODE(),
		   GEO_TRANSLATE_ROTATE_WITH_DL(4, 0, 0, 0, 0, 0, 0, sickTree_Circle_009_mesh),
		   GEO_DISPLAY_LIST(4, sickTree_material_revert_render_settings),
	   GEO_CLOSE_NODE(),
	   GEO_CLOSE_NODE(),
	GEO_END(),
};


const GeoLayout sickTree2_geo[] = {
	GEO_CULLING_RADIUS(1500),
	GEO_OPEN_NODE(),
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xa8, 375),
      GEO_OPEN_NODE(),
		GEO_TRANSLATE_ROTATE_WITH_DL(4, 0, 0, 0, 0, 0, 0, sickTree2_Circle_009_mesh),
		GEO_DISPLAY_LIST(4, sickTree2_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
