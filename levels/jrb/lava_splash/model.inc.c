// lava Splash

// 0x0402A588
 const Vtx lava_splash_seg4_vertex_0402A588[] = {
    {{{   -64,      0,      0}, 0, {     0,   2016}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    64,      0,      0}, 0, {   992,   2016}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    64,    256,      0}, 0, {   992,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -64,    256,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
};


// 0x040325C8 - 0x04032640
const Gfx lava_splash_seg4_dl_040325C8[] = {
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPVertex(lava_splash_seg4_vertex_0402A588, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x04032640 - 0x04032658
const Gfx lava_splash_seg4_dl_04032640[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_0402A5C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x04032658 - 0x04032670
const Gfx lava_splash_seg4_dl_04032658[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_0402B5C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x04032670 - 0x04032688
const Gfx lava_splash_seg4_dl_04032670[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_0402C5C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x04032688 - 0x040326A0
const Gfx lava_splash_seg4_dl_04032688[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_0402D5C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x040326A0 - 0x040326B8
const Gfx lava_splash_seg4_dl_040326A0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_0402E5C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x040326B8 - 0x040326D0
const Gfx lava_splash_seg4_dl_040326B8[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_0402F5C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x040326D0 - 0x040326E8
const Gfx lava_splash_seg4_dl_040326D0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_040305C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};

// 0x040326E8 - 0x04032700
const Gfx lava_splash_seg4_dl_040326E8[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lava_splash_seg4_texture_040315C8),
    gsSPBranchList(lava_splash_seg4_dl_040325C8),
};
