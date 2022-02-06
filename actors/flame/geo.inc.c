// 0x16000B10
const GeoLayout red_flame_shadow_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x50, 20),
   GEO_OPEN_NODE(),
      GEO_BRANCH_AND_LINK(red_flame_geo),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16000B2C
const GeoLayout red_flame_geo[] = {
   GEO_NODE_START(),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(8, geo_switch_anim_state),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3B0),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3C8),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3E0),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3F8),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B410),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B428),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B440),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B458),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16000B8C
const GeoLayout blue_flame_geo[] = {
   GEO_NODE_START(),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(8, geo_switch_anim_state),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B500),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B518),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B530),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B548),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B560),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B578),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B590),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, flame_seg3_dl_0301B5A8),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

const GeoLayout grey_flame_geo[] = {
   GEO_NODE_START(),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(8, geo_switch_anim_state),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3B02),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3C82),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3E02),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3F82),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4102),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4282),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4402),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4582),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

const GeoLayout bluegreen_flame_geo[] = {
   GEO_NODE_START(),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(8, geo_switch_anim_state),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3B03),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3C83),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3E03),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B3F83),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4103),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4283),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4403),
         GEO_DISPLAY_LIST(7, flame_seg3_dl_0301B4583),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};
