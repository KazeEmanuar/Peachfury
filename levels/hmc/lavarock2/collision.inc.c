const Collision lavarock2_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(8),
	COL_VERTEX(-324, -23, -311),
	COL_VERTEX(-232, 151, -257),
	COL_VERTEX(252, 151, -254),
	COL_VERTEX(353, -23, -308),
	COL_VERTEX(-200, 151, 233),
	COL_VERTEX(-279, -23, 312),
	COL_VERTEX(279, -23, 312),
	COL_VERTEX(200, 151, 233),
	COL_TRI_INIT(TERRAIN(3,0,0,0,SPECFLAG_CLIMBABLE,0x0015), 10),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI(4, 5, 6),
	COL_TRI(4, 6, 7),
	COL_TRI(2, 1, 4),
	COL_TRI(2, 4, 7),
	COL_TRI(7, 6, 3),
	COL_TRI(7, 3, 2),
	COL_TRI(1, 0, 5),
	COL_TRI(1, 5, 4),
	COL_TRI_STOP(),
	COL_END()
};