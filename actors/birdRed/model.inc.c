Lights1 birdRed_f3d_material_002_lights = gdSPDefLights1(
	0x7F, 0xC, 0x17,
	0xFE, 0x19, 0x2E, 0x28, 0x28, 0x28);

Lights1 birdRed_f3d_material_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Lights1 birdRed_f3d_material_001_lights = gdSPDefLights1(
	0x7F, 0x3C, 0xD,
	0xFE, 0x78, 0x1A, 0x28, 0x28, 0x28);

Lights1 birdRed_f3d_material_003_lights = gdSPDefLights1(
	0x2F, 0x17, 0xE,
	0x5F, 0x2F, 0x1C, 0x28, 0x28, 0x28);

Gfx birdRed_unnamed_ia4_aligner[] = {gsSPEndDisplayList()};
u8 birdRed_unnamed_ia4[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x22, 0x22, 0x2e, 0xe2, 0x22, 0x24, 0x00, 
	0x00, 0x2b, 0xd1, 0x0e, 0xe2, 0x1d, 0xb2, 0x00, 
	0x00, 0x5d, 0x51, 0x2e, 0xe2, 0x15, 0xd5, 0x00, 
	0x00, 0x11, 0x11, 0x2e, 0xe2, 0x11, 0x11, 0x00, 
	0x00, 0x01, 0x11, 0x2e, 0xe2, 0x11, 0x10, 0x00, 
	0x00, 0x02, 0x12, 0x2e, 0xe2, 0x21, 0x22, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	
};

Vtx birdRed_Bone_mesh_layer_1_vtx_0[25] = {
	{{{-4, 24, -37},0, {-16, -16},{0x0, 0x79, 0x26, 0xFF}}},
	{{{0, 18, -19},0, {-16, -16},{0x0, 0x79, 0x26, 0xFF}}},
	{{{4, 24, -37},0, {-16, -16},{0x0, 0x79, 0x26, 0xFF}}},
	{{{2, 22, -37},0, {-16, -16},{0x3F, 0x6C, 0x16, 0xFF}}},
	{{{1, 19, -20},0, {-16, -16},{0x3F, 0x6C, 0x16, 0xFF}}},
	{{{7, 18, -34},0, {-16, -16},{0x3F, 0x6C, 0x16, 0xFF}}},
	{{{-7, 18, -34},0, {-16, -16},{0xC1, 0x6C, 0x16, 0xFF}}},
	{{{0, 19, -20},0, {-16, -16},{0xC1, 0x6C, 0x16, 0xFF}}},
	{{{-2, 22, -37},0, {-16, -16},{0xC1, 0x6C, 0x16, 0xFF}}},
	{{{-5, 29, 21},0, {8, -86},{0xC0, 0x69, 0x21, 0xFF}}},
	{{{-10, 16, 23},0, {-377, 299},{0x99, 0xDA, 0x41, 0xFF}}},
	{{{-3, 17, 30},0, {129, 459},{0xB2, 0xC1, 0x4E, 0xFF}}},
	{{{0, 22, 30},0, {240, 223},{0x0, 0x4B, 0x67, 0xFF}}},
	{{{5, 29, 21},0, {472, -86},{0x40, 0x69, 0x21, 0xFF}}},
	{{{2, 17, 30},0, {351, 459},{0x4E, 0xC1, 0x4E, 0xFF}}},
	{{{10, 16, 23},0, {857, 299},{0x67, 0xDA, 0x41, 0xFF}}},
	{{{-14, 13, 4},0, {-16, -16},{0x85, 0xE1, 0xF6, 0xFF}}},
	{{{-10, 16, 23},0, {-16, -16},{0x99, 0xDA, 0x41, 0xFF}}},
	{{{-5, 29, 21},0, {-16, -16},{0xC0, 0x69, 0x21, 0xFF}}},
	{{{-3, 19, -24},0, {-16, -16},{0xC0, 0x29, 0x9A, 0xFF}}},
	{{{3, 19, -24},0, {-16, -16},{0x40, 0x29, 0x9A, 0xFF}}},
	{{{5, 29, 21},0, {-16, -16},{0x40, 0x69, 0x21, 0xFF}}},
	{{{14, 13, 4},0, {-16, -16},{0x7B, 0xE1, 0xF6, 0xFF}}},
	{{{10, 16, 23},0, {-16, -16},{0x67, 0xDA, 0x41, 0xFF}}},
	{{{0, 11, -21},0, {-16, -16},{0x0, 0x9C, 0xB2, 0xFF}}},
};

Gfx birdRed_Bone_mesh_layer_1_tri_0[] = {
	gsSPVertex(birdRed_Bone_mesh_layer_1_vtx_0 + 0, 25, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
	gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
	gsSP2Triangles(11, 12, 9, 0, 9, 12, 13, 0),
	gsSP2Triangles(13, 12, 14, 0, 14, 15, 13, 0),
	gsSP2Triangles(16, 17, 18, 0, 19, 16, 18, 0),
	gsSP2Triangles(20, 19, 18, 0, 20, 18, 21, 0),
	gsSP2Triangles(22, 20, 21, 0, 21, 23, 22, 0),
	gsSP2Triangles(22, 24, 20, 0, 24, 19, 20, 0),
	gsSP1Triangle(19, 24, 16, 0),
	gsSPEndDisplayList(),
};

Vtx birdRed_Bone_mesh_layer_1_vtx_1[8] = {
	{{{-3, 17, 30},0, {-16, -16},{0xB2, 0xC1, 0x4E, 0xFF}}},
	{{{-10, 16, 23},0, {-16, -16},{0x99, 0xDA, 0x41, 0xFF}}},
	{{{0, 4, 4},0, {-16, -16},{0x0, 0x81, 0x3, 0xFF}}},
	{{{-14, 13, 4},0, {-16, -16},{0x85, 0xE1, 0xF6, 0xFF}}},
	{{{0, 11, -21},0, {-16, -16},{0x0, 0x9C, 0xB2, 0xFF}}},
	{{{14, 13, 4},0, {-16, -16},{0x7B, 0xE1, 0xF6, 0xFF}}},
	{{{10, 16, 23},0, {-16, -16},{0x67, 0xDA, 0x41, 0xFF}}},
	{{{2, 17, 30},0, {-16, -16},{0x4E, 0xC1, 0x4E, 0xFF}}},
};

Gfx birdRed_Bone_mesh_layer_1_tri_1[] = {
	gsSPVertex(birdRed_Bone_mesh_layer_1_vtx_1 + 0, 8, 0),
	gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0),
	gsSP2Triangles(4, 2, 3, 0, 5, 2, 4, 0),
	gsSP2Triangles(2, 5, 6, 0, 6, 7, 2, 0),
	gsSP1Triangle(7, 0, 2, 0),
	gsSPEndDisplayList(),
};

Vtx birdRed_Bone_mesh_layer_1_vtx_2[4] = {
	{{{0, 22, 30},0, {-16, -16},{0x0, 0x4B, 0x67, 0xFF}}},
	{{{-3, 17, 30},0, {-16, -16},{0xB2, 0xC1, 0x4E, 0xFF}}},
	{{{0, 17, 37},0, {-16, -16},{0x0, 0xEB, 0x7D, 0xFF}}},
	{{{2, 17, 30},0, {-16, -16},{0x4E, 0xC1, 0x4E, 0xFF}}},
};

Gfx birdRed_Bone_mesh_layer_1_tri_2[] = {
	gsSPVertex(birdRed_Bone_mesh_layer_1_vtx_2 + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0),
	gsSP1Triangle(3, 0, 2, 0),
	gsSPEndDisplayList(),
};

Vtx birdRed_Bone_mesh_layer_1_vtx_3[54] = {
	{{{5, 1, 0},0, {624, 112},{0x71, 0x25, 0xD4, 0xFF}}},
	{{{4, 2, -2},0, {624, -16},{0x71, 0x25, 0xD4, 0xFF}}},
	{{{4, 11, 5},0, {368, -16},{0x71, 0x25, 0xD4, 0xFF}}},
	{{{4, 2, -2},0, {624, 880},{0x8F, 0x25, 0xD4, 0xFF}}},
	{{{3, 1, 0},0, {624, 752},{0x8F, 0x25, 0xD4, 0xFF}}},
	{{{4, 11, 5},0, {368, 752},{0x8F, 0x25, 0xD4, 0xFF}}},
	{{{4, 11, 5},0, {368, 240},{0x0, 0xC5, 0x71, 0xFF}}},
	{{{3, 1, 0},0, {624, 240},{0x0, 0xC5, 0x71, 0xFF}}},
	{{{5, 1, 0},0, {624, 112},{0x0, 0xC5, 0x71, 0xFF}}},
	{{{4, -2, 5},0, {368, 240},{0x6B, 0xD2, 0xCD, 0xFF}}},
	{{{3, 1, 0},0, {624, 240},{0x6B, 0xD2, 0xCD, 0xFF}}},
	{{{4, 3, 1},0, {624, 176},{0x6B, 0xD2, 0xCD, 0xFF}}},
	{{{-1, -1, 4},0, {368, 240},{0x5C, 0xAE, 0x1F, 0xFF}}},
	{{{3, 1, 0},0, {624, 240},{0x5C, 0xAE, 0x1F, 0xFF}}},
	{{{4, 3, 1},0, {624, 176},{0x5C, 0xAE, 0x1F, 0xFF}}},
	{{{4, -2, 5},0, {368, 240},{0x99, 0xCD, 0xCA, 0xFF}}},
	{{{4, 3, 1},0, {624, 176},{0x99, 0xCD, 0xCA, 0xFF}}},
	{{{5, 1, 0},0, {624, 112},{0x99, 0xCD, 0xCA, 0xFF}}},
	{{{9, -1, 4},0, {368, 240},{0xA7, 0xAA, 0x1A, 0xFF}}},
	{{{4, 3, 1},0, {624, 176},{0xA7, 0xAA, 0x1A, 0xFF}}},
	{{{5, 1, 0},0, {624, 112},{0xA7, 0xAA, 0x1A, 0xFF}}},
	{{{-1, -1, 4},0, {368, 240},{0xC4, 0xF3, 0x91, 0xFF}}},
	{{{4, 3, 1},0, {624, 176},{0xC4, 0xF3, 0x91, 0xFF}}},
	{{{5, 0, 0},0, {624, 112},{0xC4, 0xF3, 0x91, 0xFF}}},
	{{{9, -1, 4},0, {368, 240},{0x3D, 0xF7, 0x91, 0xFF}}},
	{{{3, 0, 0},0, {624, 240},{0x3D, 0xF7, 0x91, 0xFF}}},
	{{{4, 3, 1},0, {624, 176},{0x3D, 0xF7, 0x91, 0xFF}}},
	{{{-5, 1, 0},0, {624, 112},{0x8F, 0x25, 0xD4, 0xFF}}},
	{{{-4, 11, 5},0, {368, -16},{0x8F, 0x25, 0xD4, 0xFF}}},
	{{{-4, 2, -2},0, {624, -16},{0x8F, 0x25, 0xD4, 0xFF}}},
	{{{-4, 2, -2},0, {624, 880},{0x71, 0x25, 0xD4, 0xFF}}},
	{{{-4, 11, 5},0, {368, 752},{0x71, 0x25, 0xD4, 0xFF}}},
	{{{-3, 1, 0},0, {624, 752},{0x71, 0x25, 0xD4, 0xFF}}},
	{{{-4, 11, 5},0, {368, 240},{0x0, 0xC5, 0x71, 0xFF}}},
	{{{-5, 1, 0},0, {624, 112},{0x0, 0xC5, 0x71, 0xFF}}},
	{{{-3, 1, 0},0, {624, 240},{0x0, 0xC5, 0x71, 0xFF}}},
	{{{-4, -2, 5},0, {368, 240},{0x69, 0xD0, 0xCC, 0xFF}}},
	{{{-5, 1, 0},0, {624, 112},{0x69, 0xD0, 0xCC, 0xFF}}},
	{{{-4, 3, 1},0, {624, 176},{0x69, 0xD0, 0xCC, 0xFF}}},
	{{{-4, -2, 5},0, {368, 240},{0x97, 0xD0, 0xCC, 0xFF}}},
	{{{-4, 3, 1},0, {624, 176},{0x97, 0xD0, 0xCC, 0xFF}}},
	{{{-3, 1, 0},0, {624, 240},{0x97, 0xD0, 0xCC, 0xFF}}},
	{{{1, -1, 4},0, {368, 240},{0x3D, 0xF6, 0x91, 0xFF}}},
	{{{-5, 0, 0},0, {624, 112},{0x3D, 0xF6, 0x91, 0xFF}}},
	{{{-4, 3, 1},0, {624, 176},{0x3D, 0xF6, 0x91, 0xFF}}},
	{{{1, -1, 4},0, {368, 240},{0xA6, 0xAB, 0x1C, 0xFF}}},
	{{{-4, 3, 1},0, {624, 176},{0xA6, 0xAB, 0x1C, 0xFF}}},
	{{{-3, 1, 0},0, {624, 240},{0xA6, 0xAB, 0x1C, 0xFF}}},
	{{{-9, -1, 4},0, {368, 240},{0x5A, 0xAB, 0x1C, 0xFF}}},
	{{{-5, 1, 0},0, {624, 112},{0x5A, 0xAB, 0x1C, 0xFF}}},
	{{{-4, 3, 1},0, {624, 176},{0x5A, 0xAB, 0x1C, 0xFF}}},
	{{{-9, -1, 4},0, {368, 240},{0xC3, 0xF6, 0x91, 0xFF}}},
	{{{-4, 3, 1},0, {624, 176},{0xC3, 0xF6, 0x91, 0xFF}}},
	{{{-3, 0, 0},0, {624, 240},{0xC3, 0xF6, 0x91, 0xFF}}},
};

Gfx birdRed_Bone_mesh_layer_1_tri_3[] = {
	gsSPVertex(birdRed_Bone_mesh_layer_1_vtx_3 + 0, 30, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
	gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
	gsSP2Triangles(12, 13, 14, 0, 15, 16, 17, 0),
	gsSP2Triangles(18, 19, 20, 0, 21, 22, 23, 0),
	gsSP2Triangles(24, 25, 26, 0, 27, 28, 29, 0),
	gsSPVertex(birdRed_Bone_mesh_layer_1_vtx_3 + 30, 24, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
	gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
	gsSP2Triangles(12, 13, 14, 0, 15, 16, 17, 0),
	gsSP2Triangles(18, 19, 20, 0, 21, 22, 23, 0),
	gsSPEndDisplayList(),
};

Vtx birdRed_bird_l_mesh_layer_1_vtx_0[4] = {
	{{{16, 0, -4},0, {-16, -16},{0x43, 0xD, 0x6B, 0xFF}}},
	{{{-3, 32, 3},0, {-16, -16},{0x43, 0xD, 0x6B, 0xFF}}},
	{{{-10, -4, 10},0, {-16, -16},{0x43, 0xD, 0x6B, 0xFF}}},
	{{{12, 46, -10},0, {-16, -16},{0x43, 0xD, 0x6B, 0xFF}}},
};

Gfx birdRed_bird_l_mesh_layer_1_tri_0[] = {
	gsSPVertex(birdRed_bird_l_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
	gsSPEndDisplayList(),
};

Vtx birdRed_bird_r_mesh_layer_1_vtx_0[4] = {
	{{{-16, 1, -5},0, {-16, -16},{0xBC, 0xC, 0x6B, 0xFF}}},
	{{{10, -4, 10},0, {-16, -16},{0xBC, 0xC, 0x6B, 0xFF}}},
	{{{3, 32, 3},0, {-16, -16},{0xBC, 0xC, 0x6B, 0xFF}}},
	{{{-12, 46, -10},0, {-16, -16},{0xBC, 0xC, 0x6B, 0xFF}}},
};

Gfx birdRed_bird_r_mesh_layer_1_tri_0[] = {
	gsSPVertex(birdRed_bird_r_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSPEndDisplayList(),
};


Gfx mat_birdRed_f3d_material_002[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, SHADE, TEXEL0_ALPHA, SHADE, 0, 0, 0, ENVIRONMENT, TEXEL0, SHADE, TEXEL0_ALPHA, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPClearGeometryMode(G_CULL_BACK),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, birdRed_unnamed_ia4),
	gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, 7, 0, G_TX_CLAMP | G_TX_NOMIRROR, 3, 0, G_TX_CLAMP | G_TX_NOMIRROR, 4, 0),
	gsDPLoadSync(),
	gsDPLoadBlock(7, 0, 0, 31, 2048),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_4b, 1, 0, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 3, 0, G_TX_CLAMP | G_TX_NOMIRROR, 4, 0),
	gsDPSetTileSize(0, 0, 0, 60, 28),
	gsSPSetLights1(birdRed_f3d_material_002_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_birdRed_f3d_material_002[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_CULL_BACK),
	gsSPEndDisplayList(),
};

Gfx mat_birdRed_f3d_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(birdRed_f3d_material_lights),
	gsSPEndDisplayList(),
};

Gfx mat_birdRed_f3d_material_001[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(birdRed_f3d_material_001_lights),
	gsSPEndDisplayList(),
};

Gfx mat_birdRed_f3d_material_003[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPClearGeometryMode(G_CULL_BACK),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(birdRed_f3d_material_003_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_birdRed_f3d_material_003[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_CULL_BACK),
	gsSPEndDisplayList(),
};

Gfx birdRed_Bone_mesh_layer_1[] = {
	gsSPDisplayList(mat_birdRed_f3d_material_002),
	gsSPDisplayList(birdRed_Bone_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_birdRed_f3d_material_002),
	gsSPDisplayList(mat_birdRed_f3d_material),
	gsSPDisplayList(birdRed_Bone_mesh_layer_1_tri_1),
	gsSPDisplayList(mat_birdRed_f3d_material_001),
	gsSPDisplayList(birdRed_Bone_mesh_layer_1_tri_2),
	gsSPDisplayList(mat_birdRed_f3d_material_003),
	gsSPDisplayList(birdRed_Bone_mesh_layer_1_tri_3),
	gsSPDisplayList(mat_revert_birdRed_f3d_material_003),
	gsSPEndDisplayList(),
};

Gfx birdRed_bird_l_mesh_layer_1[] = {
	gsSPDisplayList(mat_birdRed_f3d_material_002),
	gsSPDisplayList(birdRed_bird_l_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_birdRed_f3d_material_002),
	gsSPEndDisplayList(),
};

Gfx birdRed_bird_r_mesh_layer_1[] = {
	gsSPDisplayList(mat_birdRed_f3d_material_002),
	gsSPDisplayList(birdRed_bird_r_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_birdRed_f3d_material_002),
	gsSPEndDisplayList(),
};

Gfx birdRed_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

