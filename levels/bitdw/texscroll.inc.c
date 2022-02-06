void scroll_bitdw_dl_Layer1_003_mesh_layer_1_vtx_1() {
	int i = 0;
	int count = 83;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_003_mesh_layer_1_vtx_1);

	deltaY = (int)(1.0399999618530273 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_dl_Layer1_008_mesh_layer_1_vtx_3() {
	int i = 0;
	int count = 11;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_008_mesh_layer_1_vtx_3);

	deltaY = (int)(1.0399999618530273 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_dl_Layer1_011_mesh_layer_1_vtx_0() {
	int i = 0;
	int count = 24;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_011_mesh_layer_1_vtx_0);

	deltaY = (int)(1.0399999618530273 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_dl_Layer1_012_mesh_layer_1_vtx_0() {
	int i = 0;
	int count = 24;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_012_mesh_layer_1_vtx_0);

	deltaY = (int)(1.0399999618530273 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_dl_Layer1_013_mesh_layer_1_vtx_1() {
	int i = 0;
	int count = 8;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_013_mesh_layer_1_vtx_1);

	deltaX = (int)(0.25 * 0x20) % width;
	deltaY = (int)(0.10000000149011612 * 0x20) % height;

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

void scroll_bitdw_dl_Layer1_013_mesh_layer_1_vtx_3() {
	int i = 0;
	int count = 24;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_013_mesh_layer_1_vtx_3);

	deltaY = (int)(1.0399999618530273 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_dl_Layer6_mesh_layer_6_vtx_1() {
	int i = 0;
	int count = 8;
	int width = 32 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer6_mesh_layer_6_vtx_1);

	deltaY = (int)(-1.7000000476837158 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_dl_Layer1_026_mesh_layer_1_vtx_2() {
	int i = 0;
	int count = 12;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(bitdw_dl_Layer1_026_mesh_layer_1_vtx_2);

	deltaY = (int)(0.05000000074505806 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw() {
	scroll_bitdw_dl_Layer1_003_mesh_layer_1_vtx_1();
	scroll_bitdw_dl_Layer1_008_mesh_layer_1_vtx_3();
	scroll_bitdw_dl_Layer1_011_mesh_layer_1_vtx_0();
	scroll_bitdw_dl_Layer1_012_mesh_layer_1_vtx_0();
	scroll_bitdw_dl_Layer1_013_mesh_layer_1_vtx_1();
	scroll_bitdw_dl_Layer1_013_mesh_layer_1_vtx_3();
	scroll_bitdw_dl_Layer6_mesh_layer_6_vtx_1();
	scroll_bitdw_dl_Layer1_026_mesh_layer_1_vtx_2();
}
