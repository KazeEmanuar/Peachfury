void scroll_sts_mat_wf_dl_SCROLLWATER_layer1_area1() {
	Gfx *mat = segmented_to_virtual(mat_wf_dl_SCROLLWATER_layer1_area1);
	shift_s(mat, 16, PACK_TILESIZE(0, 1));
};

void scroll_sts_mat_wf_dl_f3d_material_026() {
	Gfx *mat = segmented_to_virtual(mat_wf_dl_f3d_material_026);
	shift_t(mat, 11, PACK_TILESIZE(0, 3));
	shift_s_down(mat, -1, PACK_TILESIZE(0, 1));
	shift_t(mat, -1, PACK_TILESIZE(0, 1));
};

void scroll_wf_dl_Plane_001_mesh_vtx_1() {
	int i = 0;
	int count = 36;
	int width = 16 * 0x20;
	int height = 16 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(wf_dl_Plane_001_mesh_vtx_1);

	deltaY = (int)(0.20000000298023224 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_wf_dl_Water_mesh_layer_5_vtx_0() {
	int i = 0;
	int count = 10;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(wf_dl_Water_mesh_layer_5_vtx_0);

	deltaY = (int)(0.05000000074505806 * 0x20) % height;

	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[1] += deltaY;
	}
	currentY += deltaY;
}

void scroll_wf() {
	scroll_sts_mat_wf_dl_SCROLLWATER_layer1_area1();
	scroll_sts_mat_wf_dl_f3d_material_026();
	scroll_wf_dl_Plane_001_mesh_vtx_1();
	scroll_wf_dl_Water_mesh_layer_5_vtx_0();
}
