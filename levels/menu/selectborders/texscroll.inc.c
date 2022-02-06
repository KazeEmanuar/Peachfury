void scroll_selectborders_Cube_mesh_vtx_3() {
	int i = 0;
	int count = 9;
	int width = 64 * 0x20;
	int height = 64 * 0x20;

	static int currentX = 0;
	int deltaX;
	Vtx *vertices = segmented_to_virtual(selectborders_Cube_mesh_vtx_3);

	deltaX = (int)(0.10000000149011612 * 0x20) % width;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
	}
	currentX += deltaX;

}
void scroll_bob_level_dl_selectborders() {
	scroll_selectborders_Cube_mesh_vtx_3();

}
