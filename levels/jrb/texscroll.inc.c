void scroll_jrb_dl_cullthislight_mesh_vtx_0() {
	int i = 0;
	int count = 96;
	int width = 128 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(jrb_dl_cullthislight_mesh_vtx_0);

	deltaY = (int)(0.10000000149011612 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_jrb_dl_Circle_020_mesh_vtx_0() {
	int i = 0;
	int count = 12;
	int width = 128 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(jrb_dl_Circle_020_mesh_vtx_0);

	deltaY = (int)(0.10000000149011612 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_jrb_dl_Circle_042_mesh_vtx_0() {
	int i = 0;
	int count = 12;
	int width = 128 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(jrb_dl_Circle_042_mesh_vtx_0);

	deltaY = (int)(0.10000000149011612 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_jrb() {
	scroll_jrb_dl_cullthislight_mesh_vtx_0();
	scroll_jrb_dl_Circle_020_mesh_vtx_0();
	scroll_jrb_dl_Circle_042_mesh_vtx_0();
}
