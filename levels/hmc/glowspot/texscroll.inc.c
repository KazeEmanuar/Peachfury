void scroll_sts_mat_glowspot_f3d_material_007_layer6() {
	Gfx *mat = segmented_to_virtual(mat_glowspot_f3d_material_007_layer6);
	shift_t(mat, 13, PACK_TILESIZE(0, 1));
	shift_t_down(mat, 21, PACK_TILESIZE(0, 1));
};

void scroll_hmc_level_geo_glowspot() {
	scroll_sts_mat_glowspot_f3d_material_007_layer6();
}
