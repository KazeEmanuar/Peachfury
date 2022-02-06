extern const u8 lava_splash_seg4_texture_0402A5C8[];
extern const u8 lava_splash_seg4_texture_0402B5C8[] ;
extern const u8 lava_splash_seg4_texture_0402C5C8[] ;
extern const u8 lava_splash_seg4_texture_0402D5C8[] ;
extern const u8 lava_splash_seg4_texture_0402E5C8[];
extern const u8 lava_splash_seg4_texture_0402F5C8[] ;
extern const u8 lava_splash_seg4_texture_040305C8[];
extern const u8 lava_splash_seg4_texture_040315C8[];

const GeoLayout lava_splash_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_04032640),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_04032658),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_04032670),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_04032688),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_040326A0),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_040326B8),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_040326D0),
      GEO_DISPLAY_LIST(LAYER_ALPHA, lava_splash_seg4_dl_040326E8),
   GEO_CLOSE_NODE(),
   GEO_END(),
};
