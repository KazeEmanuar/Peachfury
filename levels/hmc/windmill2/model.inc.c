Vtx windmill2_Circle_001_mesh_layer_1_vtx_0[16] = {
	{{{180, -1538, 344},0, {-16, 1008},{0xFF, 0x81, 0x0, 0xFF}}},
	{{{322, 23, 54},0, {-16, 1008},{0x36, 0x1E, 0x0, 0xFF}}},
	{{{240, -28, 289},0, {-16, 1008},{0x72, 0x3F, 0x0, 0xFF}}},
	{{{322, -1538, -11},0, {-16, 1008},{0xB9, 0x5E, 0x0, 0xFF}}},
	{{{180, 1021, 1234},0, {-16, 1008},{0xFF, 0x81, 0x0, 0xFF}}},
	{{{240, 231, -50},0, {-16, 1008},{0x72, 0x3F, 0x0, 0xFF}}},
	{{{322, 714, 1412},0, {-16, 1008},{0xB9, 0x5E, 0x0, 0xFF}}},
	{{{180, 523, -1425},0, {-16, 1008},{0xFF, 0x81, 0x0, 0xFF}}},
	{{{240, -192, -104},0, {-16, 1008},{0x72, 0x3F, 0x0, 0xFF}}},
	{{{322, 830, -1247},0, {-16, 1008},{0xB9, 0x5E, 0x0, 0xFF}}},
	{{{-303, 80, -138},0, {-16, 1008},{0x0, 0x0, 0x0, 0xFF}}},
	{{{-303, 80, 138},0, {-16, 1008},{0x0, 0x0, 0x0, 0xFF}}},
	{{{415, 79, 54},0, {-16, 1008},{0xFF, 0x8D, 0x0, 0xFF}}},
	{{{415, -33, 119},0, {-16, 1008},{0xFF, 0x8D, 0x0, 0xFF}}},
	{{{-303, -159, 0},0, {-16, 1008},{0x0, 0x0, 0x0, 0xFF}}},
	{{{415, -33, -11},0, {-16, 1008},{0xFF, 0x8D, 0x0, 0xFF}}},
};

Gfx windmill2_Circle_001_mesh_layer_1_tri_0[] = {
	gsSPVertex(windmill2_Circle_001_mesh_layer_1_vtx_0 + 0, 16, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
	gsSP2Triangles(4, 1, 5, 0, 4, 6, 1, 0),
	gsSP2Triangles(7, 1, 8, 0, 7, 9, 1, 0),
	gsSP2Triangles(10, 11, 12, 0, 11, 13, 12, 0),
	gsSP2Triangles(11, 14, 13, 0, 14, 15, 13, 0),
	gsSP2Triangles(14, 10, 15, 0, 10, 12, 15, 0),
	gsSP1Triangle(15, 12, 13, 0),
	gsSPEndDisplayList(),
};


Gfx mat_windmill2_f3d_material_015[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPClearGeometryMode(G_CULL_BACK | G_LIGHTING),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPEndDisplayList(),
};

Gfx mat_revert_windmill2_f3d_material_015[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING),
	gsSPEndDisplayList(),
};

Gfx windmill2_Circle_001_mesh_layer_1[] = {
	gsSPDisplayList(mat_windmill2_f3d_material_015),
	gsSPDisplayList(windmill2_Circle_001_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_windmill2_f3d_material_015),
	gsSPEndDisplayList(),
};

Gfx windmill2_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

