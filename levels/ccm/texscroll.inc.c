void scroll_sts_mat_ccm_dl_ZOOWATER_layer5() {
	Gfx *mat = segmented_to_virtual(mat_ccm_dl_ZOOWATER_layer5);
	shift_s(mat, 13, PACK_TILESIZE(0, 1));
	shift_t_down(mat, 13, PACK_TILESIZE(0, 1));
	shift_s_down(mat, 21, PACK_TILESIZE(0, 1));
};

void scroll_ccm_dl_waterstill_mesh_layer_5_vtx_1() {
	int i = 0;
	int count = 4;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int timeX;
	float amplitudeX = 10.0;
	float frequencyX = 0.009999999776482582;
	float offsetX = 500.0;
	static int currentY = 0;
	int deltaY;
	static int timeY;
	float amplitudeY = 10.0;
	float frequencyY = 0.009999999776482582;
	float offsetY = 0.0;
	Vtx *vertices = segmented_to_virtual(ccm_dl_waterstill_mesh_layer_5_vtx_1);

	deltaX = (int)(amplitudeX * frequencyX * coss((frequencyX * timeX + offsetX) * (1024 * 16 - 1) / 6.28318530718) * 0x20);
	deltaY = (int)(amplitudeY * frequencyY * coss((frequencyY * timeY + offsetY) * (1024 * 16 - 1) / 6.28318530718) * 0x20);

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}
	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
		vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	timeX += 1;	currentY += deltaY;	timeY += 1;
}

void scroll_ccm_dl_ZZZZWATERFALL_mesh_layer_5_vtx_0() {
	int i = 0;
	int count = 14;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(ccm_dl_ZZZZWATERFALL_mesh_layer_5_vtx_0);

	deltaY = (int)(1.25 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_ccm_dl_ZZZZWATERFALL_mesh_layer_5_vtx_1() {
	int i = 0;
	int count = 10;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	Vtx *vertices = segmented_to_virtual(ccm_dl_ZZZZWATERFALL_mesh_layer_5_vtx_1);

	deltaX = (int)(-2.5 * 0x20) % width;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
	}
	currentX += deltaX;
}

void scroll_ccm_dl_ZZZZWATERFALL_mesh_layer_6_vtx_0() {
	int i = 0;
	int count = 8;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	Vtx *vertices = segmented_to_virtual(ccm_dl_ZZZZWATERFALL_mesh_layer_6_vtx_0);

	deltaX = (int)(1.0 * 0x20) % width;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
	}
	currentX += deltaX;
}

void scroll_ccm_dl_Circle_007_mesh_vtx_0() {
	int i = 0;
	int count = 58;
	int width = 128 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(ccm_dl_Circle_007_mesh_vtx_0);

	deltaY = (int)(0.10000000149011612 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_ccm_dl_Plane_036_mesh_layer_5_vtx_1() {
	int i = 0;
	int count = 42;
	int width = 128 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(ccm_dl_Plane_036_mesh_layer_5_vtx_1);

	deltaY = (int)(0.10000000149011612 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_ccm_dl_Lava_mesh_layer_1_vtx_0() {
	int i = 0;
	int count = 259;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(ccm_dl_Lava_mesh_layer_1_vtx_0);

	deltaX = (int)(0.10000000149011612 * 0x20) % width;
	deltaY = (int)(0.05000000074505806 * 0x20) % height;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}
	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
		vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void scroll_ccm_dl_Plane_039_mesh_layer_5_vtx_0() {
	int i = 0;
	int count = 260;
	int width = 128 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(ccm_dl_Plane_039_mesh_layer_5_vtx_0);

	deltaY = (int)(0.10000000149011612 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_ccm() {
	scroll_sts_mat_ccm_dl_ZOOWATER_layer5();
	scroll_ccm_dl_waterstill_mesh_layer_5_vtx_1();
	scroll_ccm_dl_ZZZZWATERFALL_mesh_layer_5_vtx_0();
	scroll_ccm_dl_ZZZZWATERFALL_mesh_layer_5_vtx_1();
	scroll_ccm_dl_ZZZZWATERFALL_mesh_layer_6_vtx_0();
	scroll_ccm_dl_Circle_007_mesh_vtx_0();
	scroll_ccm_dl_Plane_036_mesh_layer_5_vtx_1();
	scroll_ccm_dl_Lava_mesh_layer_1_vtx_0();
	scroll_ccm_dl_Plane_039_mesh_layer_5_vtx_0();
}
