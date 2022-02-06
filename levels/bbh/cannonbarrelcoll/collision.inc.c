const Collision cannonbarrelcoll_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(16),
	COL_VERTEX(58, 236, -57),
	COL_VERTEX(-57, 236, -57),
	COL_VERTEX(-57, 236, 58),
	COL_VERTEX(82, 236, 0),
	COL_VERTEX(58, 236, 58),
	COL_VERTEX(-81, 236, 0),
	COL_VERTEX(0, 236, -81),
	COL_VERTEX(0, 236, 82),
	COL_VERTEX(58, -76, 58),
	COL_VERTEX(0, -76, 82),
	COL_VERTEX(82, -76, 0),
	COL_VERTEX(-57, -76, 58),
	COL_VERTEX(58, -76, -57),
	COL_VERTEX(0, -76, -81),
	COL_VERTEX(-57, -76, -57),
	COL_VERTEX(-81, -76, 0),
	COL_TRI_INIT(SURFACE_NOT_SLIPPERY, 6),
	COL_TRI(0, 1, 2),
	COL_TRI(3, 0, 4),
	COL_TRI(0, 2, 4),
	COL_TRI(1, 5, 2),
	COL_TRI(0, 6, 1),
	COL_TRI(2, 7, 4),
	COL_TRI_INIT(SURFACE_DEFAULT, 16),
	COL_TRI(8, 4, 7),
	COL_TRI(8, 7, 9),
	COL_TRI(10, 4, 8),
	COL_TRI(10, 3, 4),
	COL_TRI(9, 7, 2),
	COL_TRI(9, 2, 11),
	COL_TRI(12, 3, 10),
	COL_TRI(12, 0, 3),
	COL_TRI(13, 0, 12),
	COL_TRI(13, 6, 0),
	COL_TRI(14, 6, 13),
	COL_TRI(11, 2, 5),
	COL_TRI(14, 1, 6),
	COL_TRI(15, 1, 14),
	COL_TRI(15, 5, 1),
	COL_TRI(11, 5, 15),
	COL_TRI_STOP(),
	COL_END()
};