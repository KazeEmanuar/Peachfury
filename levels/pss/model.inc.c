Lights1 pss_dl_f3d_material_008_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Vtx pss_dl_Plane_001_mesh_layer_1_vtx_cull[8] = {
	{{{-160, 0, 160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-160, 0, 160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-160, 0, -160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-160, 0, -160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{160, 0, 160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{160, 0, 160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{160, 0, -160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{160, 0, -160},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
};

Vtx pss_dl_Plane_001_mesh_layer_1_vtx_0[4] = {
	{{{-160, 0, 160},0, {-16, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{160, 0, 160},0, {1008, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{160, 0, -160},0, {1008, -16},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-160, 0, -160},0, {-16, -16},{0x0, 0x7F, 0x0, 0xFF}}},
};

Gfx pss_dl_Plane_001_mesh_layer_1_tri_0[] = {
	gsSPVertex(pss_dl_Plane_001_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSPEndDisplayList(),
};

Gfx mat_pss_dl_f3d_material_008[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(pss_dl_f3d_material_008_lights),
	gsSPEndDisplayList(),
};

Gfx pss_dl_Plane_001_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(pss_dl_Plane_001_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_pss_dl_f3d_material_008),
	gsSPDisplayList(pss_dl_Plane_001_mesh_layer_1_tri_0),
	gsSPEndDisplayList(),
};

Gfx pss_dl_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

