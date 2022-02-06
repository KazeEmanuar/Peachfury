const Collision floatboat_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(15),
	COL_VERTEX(61, -67, -79),
	COL_VERTEX(19, 33, -189),
	COL_VERTEX(178, 33, -68),
	COL_VERTEX(-209, 26, 108),
	COL_VERTEX(-50, 26, 230),
	COL_VERTEX(-109, -67, 142),
	COL_VERTEX(-206, 33, 268),
	COL_VERTEX(23, 29, -178),
	COL_VERTEX(23, 134, -178),
	COL_VERTEX(166, 134, -68),
	COL_VERTEX(166, 29, -68),
	COL_VERTEX(56, 134, 75),
	COL_VERTEX(56, 29, 75),
	COL_VERTEX(-87, 134, -35),
	COL_VERTEX(-87, 29, -35),
	COL_TRI_INIT(SURFACE_DEFAULT, 20),
	COL_TRI(0, 1, 2),
	COL_TRI(3, 4, 2),
	COL_TRI(3, 2, 1),
	COL_TRI(0, 5, 3),
	COL_TRI(0, 3, 1),
	COL_TRI(6, 4, 3),
	COL_TRI(4, 6, 5),
	COL_TRI(5, 6, 3),
	COL_TRI(7, 8, 9),
	COL_TRI(7, 9, 10),
	COL_TRI(10, 11, 12),
	COL_TRI(12, 11, 13),
	COL_TRI(12, 13, 14),
	COL_TRI(13, 7, 14),
	COL_TRI(11, 9, 8),
	COL_TRI(11, 8, 13),
	COL_TRI(10, 9, 11),
	COL_TRI(13, 8, 7),
	COL_TRI(0, 2, 4),
	COL_TRI(0, 4, 5),
	COL_TRI_STOP(),
	COL_END()
};
