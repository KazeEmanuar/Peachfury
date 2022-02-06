Lights1 fishbone_f3d_material_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Gfx fishbone_fishboneplatform_ci4_aligner[] = {gsSPEndDisplayList()};
u8 fishbone_fishboneplatform_ci4[] = {
	0x00, 0x01, 0x11, 0x10, 0x00, 0x10, 0x00, 0x10, 
	0x00, 0x10, 0x00, 0x10, 0x01, 0x11, 0x10, 0x00, 
	0x00, 0x12, 0x22, 0x31, 0x11, 0x21, 0x11, 0x21, 
	0x11, 0x21, 0x11, 0x21, 0x13, 0x22, 0x21, 0x00, 
	0x01, 0x21, 0x12, 0x21, 0x22, 0x12, 0x22, 0x12, 
	0x22, 0x12, 0x22, 0x12, 0x12, 0x21, 0x12, 0x10, 
	0x12, 0x11, 0x32, 0x21, 0x33, 0x13, 0x33, 0x13, 
	0x33, 0x13, 0x33, 0x13, 0x12, 0x23, 0x11, 0x21, 
	0x12, 0x33, 0x22, 0x31, 0x11, 0x21, 0x11, 0x21, 
	0x11, 0x21, 0x11, 0x21, 0x13, 0x22, 0x33, 0x21, 
	0x11, 0x31, 0x31, 0x11, 0x01, 0x21, 0x01, 0x21, 
	0x01, 0x21, 0x01, 0x21, 0x11, 0x13, 0x13, 0x11, 
	0x12, 0x12, 0x11, 0x00, 0x00, 0x10, 0x00, 0x10, 
	0x00, 0x10, 0x00, 0x10, 0x00, 0x11, 0x21, 0x21, 
	0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 
	
};

Gfx fishbone_fishboneplatform_ci4_pal_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 fishbone_fishboneplatform_ci4_pal_rgba16[] = {
	0xff, 0xfe, 0x00, 0x01, 0xff, 0xff, 0xad, 0x6b, 
	
};

Vtx fishbone_Plane_mesh_layer_4_vtx_cull[8] = {
	{{{-150, -38, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-150, -38, 150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-150, 0, 150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-150, 0, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{150, -38, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{150, -38, 150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{150, 0, 150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{150, 0, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
};

Vtx fishbone_Plane_mesh_layer_4_vtx_0[20] = {
	{{{-132, -4, 132},0, {768, 160},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{132, -4, 132},0, {768, 160},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{132, -4, -132},0, {768, 160},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-132, -4, -132},0, {768, 160},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-150, 0, 132},0, {1023, 0},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-150, -37, 132},0, {1023, 256},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{150, -37, 132},0, {1, 256},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{150, 0, 132},0, {1, 0},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{150, 0, -132},0, {1023, 0},{0x0, 0x0, 0x81, 0xFF}}},
	{{{150, -38, -132},0, {1023, 256},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-150, -38, -132},0, {1, 256},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-150, 0, -132},0, {1, 0},{0x0, 0x0, 0x81, 0xFF}}},
	{{{132, 0, 150},0, {1023, 0},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{132, -37, 150},0, {1023, 256},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{132, -38, -150},0, {1, 256},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{132, 0, -150},0, {1, 0},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{-132, 0, -150},0, {1023, 0},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-132, -38, -150},0, {1023, 256},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-132, -37, 150},0, {1, 256},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-132, 0, 150},0, {1, 0},{0x81, 0x0, 0x0, 0xFF}}},
};

Gfx fishbone_Plane_mesh_layer_4_tri_0[] = {
	gsSPVertex(fishbone_Plane_mesh_layer_4_vtx_0 + 0, 20, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
	gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
	gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
	gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
	gsSPEndDisplayList(),
};


Gfx mat_fishbone_f3d_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, ENVIRONMENT, 0),
	gsSPClearGeometryMode(G_CULL_BACK),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fishbone_fishboneplatform_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 3),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 16, fishbone_fishboneplatform_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 28),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 28),
	gsSPSetLights1(fishbone_f3d_material_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_fishbone_f3d_material[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_CULL_BACK),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

Gfx fishbone_Plane_mesh_layer_4[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(fishbone_Plane_mesh_layer_4_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_fishbone_f3d_material),
	gsSPDisplayList(fishbone_Plane_mesh_layer_4_tri_0),
	gsSPDisplayList(mat_revert_fishbone_f3d_material),
	gsSPEndDisplayList(),
};

Gfx fishbone_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

