const Collision shyguyblock_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(8),
	COL_VERTEX(-175, 0, 175),
	COL_VERTEX(-175, 175, 175),
	COL_VERTEX(-175, 175, -175),
	COL_VERTEX(-175, 0, -175),
	COL_VERTEX(175, 175, -175),
	COL_VERTEX(175, 0, -175),
	COL_VERTEX(175, 175, 175),
	COL_VERTEX(175, 0, 175),
	COL_TRI_INIT(TERRAIN(9,0,0,0,SPECFLAG_CLIMBABLE,0x0015), 10),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI(3, 2, 4),
	COL_TRI(3, 4, 5),
	COL_TRI(5, 4, 6),
	COL_TRI(5, 6, 7),
	COL_TRI(7, 6, 1),
	COL_TRI(7, 1, 0),
	COL_TRI(6, 4, 2),
	COL_TRI(6, 2, 1),
	COL_TRI_STOP(),
	COL_END()
};
