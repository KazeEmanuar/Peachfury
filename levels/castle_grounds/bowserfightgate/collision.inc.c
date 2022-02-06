const Collision bowserfightgate_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(4),
	COL_VERTEX(-281, 710, -183),
	COL_VERTEX(330, 710, 64),
	COL_VERTEX(330, 0, 64),
	COL_VERTEX(-281, 0, -183),
	COL_TRI_INIT(SURFACE_DEFAULT, 2),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI_STOP(),
	COL_END()
};
