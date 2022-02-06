Lights1 podoboo_sm64_material_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Gfx podoboo_podoboo1_ci4_aligner[] = {gsSPEndDisplayList()};
u8 podoboo_podoboo1_ci4[] = {
	0x0, 0x0, 0x1, 0x11, 0x11, 0x10, 0x0, 0x0, 0x0, 
	0x1, 0x12, 0x22, 0x22, 0x21, 0x10, 0x0, 0x0, 0x12, 
	0x23, 0x33, 0x33, 0x32, 0x21, 0x0, 0x1, 0x23, 0x34, 
	0x33, 0x33, 0x43, 0x32, 0x10, 0x1, 0x23, 0x44, 0x43, 
	0x34, 0x44, 0x32, 0x10, 0x12, 0x33, 0x44, 0x43, 0x34, 
	0x44, 0x33, 0x21, 0x12, 0x33, 0x44, 0x43, 0x34, 0x44, 
	0x33, 0x21, 0x12, 0x33, 0x34, 0x33, 0x33, 0x43, 0x33, 
	0x21, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x21, 
	0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x21, 0x12, 
	0x33, 0x32, 0x33, 0x33, 0x23, 0x33, 0x21, 0x11, 0x23, 
	0x22, 0x33, 0x33, 0x22, 0x32, 0x11, 0x1, 0x22, 0x21, 
	0x23, 0x32, 0x12, 0x22, 0x10, 0x1, 0x12, 0x11, 0x12, 
	0x21, 0x11, 0x21, 0x10, 0x0, 0x11, 0x0, 0x11, 0x11, 
	0x0, 0x11, 0x0, 0x0, 0x0, 0x0, 0x1, 0x10, 0x0, 
	0x0, 0x0, 
};

Gfx podoboo_podoboo1_ci4_pal_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 podoboo_podoboo1_ci4_pal_rgba16[] = {
	0xFF, 0xFE, 0xB1, 0x1, 0xF4, 0x1, 0xF5, 0xC1, 0x0, 
	0x1, 
};

Vtx podoboo_Cube_mesh_vtx_cull[8] = {
	{{{-125, -125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-125, -125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-125, 125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-125, 125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{125, -125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{125, -125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{125, 125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{125, 125, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
};

Vtx podoboo_Cube_mesh_vtx_0[4] = {
	{{{-125, -125, 0},0, {2, 511},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-125, 125, 0},0, {2, 1},{0x0, 0x0, 0x81, 0xFF}}},
	{{{125, 125, 0},0, {513, 1},{0x0, 0x0, 0x81, 0xFF}}},
	{{{125, -125, 0},0, {513, 511},{0x0, 0x0, 0x81, 0xFF}}},
};

Gfx podoboo_Cube_mesh_tri_0[] = {
	gsSPVertex(podoboo_Cube_mesh_vtx_0 + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSPEndDisplayList(),
};


Gfx mat_podoboo_sm64_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, ENVIRONMENT, 0),
	gsSPClearGeometryMode(G_CULL_BACK),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, podoboo_podoboo1_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 4),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 8, podoboo_podoboo1_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 1, 0, 7, 0, G_TX_CLAMP | G_TX_NOMIRROR, 4, 0, G_TX_CLAMP | G_TX_NOMIRROR, 4, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 30, 60),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 1, 0, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 4, 0, G_TX_CLAMP | G_TX_NOMIRROR, 4, 0),
	gsDPSetTileSize(0, 0, 0, 60, 60),
	gsSPSetLights1(podoboo_sm64_material_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_podoboo_sm64_material[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_CULL_BACK),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};


Gfx podoboo_Cube_mesh[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(podoboo_Cube_mesh_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_podoboo_sm64_material),
	gsSPDisplayList(podoboo_Cube_mesh_tri_0),
	gsSPDisplayList(mat_revert_podoboo_sm64_material),
	gsSPEndDisplayList(),
};



Gfx podoboo_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};
