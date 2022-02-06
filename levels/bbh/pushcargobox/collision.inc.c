const Collision pushcargobox_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(8),
	COL_VERTEX(200, 400, -200),
	COL_VERTEX(200, 400, 200),
	COL_VERTEX(200, 0, 200),
	COL_VERTEX(200, 0, -200),
	COL_VERTEX(-200, 400, 200),
	COL_VERTEX(-200, 0, 200),
	COL_VERTEX(-200, 400, -200),
	COL_VERTEX(-200, 0, -200),
	COL_TRI_INIT(SURFACE_DEFAULT, 10),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI(1, 4, 5),
	COL_TRI(1, 5, 2),
	COL_TRI(4, 6, 7),
	COL_TRI(4, 7, 5),
	COL_TRI(0, 6, 4),
	COL_TRI(0, 4, 1),
	COL_TRI(6, 0, 3),
	COL_TRI(6, 3, 7),
	COL_TRI_STOP(),
	COL_END()
};
