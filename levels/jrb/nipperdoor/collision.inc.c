const Collision nipperdoor_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(4),
	COL_VERTEX(2, -1, -230),
	COL_VERTEX(2, 453, -230),
	COL_VERTEX(-2, 453, 234),
	COL_VERTEX(-2, -1, 234),
	COL_TRI_INIT(SURFACE_DEFAULT, 2),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI_STOP(),
	COL_END()
};
