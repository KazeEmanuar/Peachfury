const Collision underwaterdoor_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(8),
	COL_VERTEX(45, 0, -376),
	COL_VERTEX(45, 803, -376),
	COL_VERTEX(45, 803, 376),
	COL_VERTEX(45, 0, 376),
	COL_VERTEX(-45, 0, -376),
	COL_VERTEX(-45, 0, 376),
	COL_VERTEX(-45, 803, 376),
	COL_VERTEX(-45, 803, -376),
	COL_TRI_INIT(SURFACE_DEFAULT, 6),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI(4, 5, 6),
	COL_TRI(4, 6, 7),
	COL_TRI(3, 5, 4),
	COL_TRI(3, 4, 0),
	COL_TRI_STOP(),
	COL_END()
};
