void scroll_sts_mat_hmc_dl_Watermat_layer0() {
	Gfx *mat = segmented_to_virtual(mat_hmc_dl_Watermat_layer0);
	shift_s(mat, 13, PACK_TILESIZE(0, 1));
	shift_t(mat, 13, PACK_TILESIZE(0, 1));
	shift_s_down(mat, 21, PACK_TILESIZE(0, 1));
	shift_t(mat, 21, PACK_TILESIZE(0, 1));
};

void scroll_sts_mat_hmc_dl_f3d_material_006_layer5() {
	Gfx *mat = segmented_to_virtual(mat_hmc_dl_f3d_material_006_layer5);
	shift_s(mat, 20, PACK_TILESIZE(0, 1));
	shift_t(mat, 20, PACK_TILESIZE(0, 2));
	shift_s(mat, 35, PACK_TILESIZE(0, 2));
	shift_t_down(mat, 35, PACK_TILESIZE(0, 1));
};

void scroll_sts_mat_hmc_dl_f3d_material_020_layer1() {
	Gfx *mat = segmented_to_virtual(mat_hmc_dl_f3d_material_020_layer1);
	shift_s(mat, 20, PACK_TILESIZE(0, 1));
	shift_t_down(mat, 20, PACK_TILESIZE(0, 1));
	shift_s(mat, 35, PACK_TILESIZE(0, 1));
	shift_t(mat, 35, PACK_TILESIZE(0, 1));
};

void scroll_hmc() {
	scroll_sts_mat_hmc_dl_Watermat_layer0();
	scroll_sts_mat_hmc_dl_f3d_material_006_layer5();
	scroll_sts_mat_hmc_dl_f3d_material_020_layer1();
}
