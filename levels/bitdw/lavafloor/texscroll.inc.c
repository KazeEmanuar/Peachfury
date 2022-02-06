void scroll_lavafloor_LAVA_mesh_layer_1_vtx_0() {
	int i = 0;
	int count = 64;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(lavafloor_LAVA_mesh_layer_1_vtx_0);

	deltaY = (int)(0.05000000074505806 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_bitdw_level_geo_lavafloor() {
	scroll_lavafloor_LAVA_mesh_layer_1_vtx_0();
}
