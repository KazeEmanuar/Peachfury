Lights1 dorrieeyes_eyeclosed_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Lights1 dorrieeyes_eyehalf_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Gfx dorrieeyes_eyeframenessie2_ci4_aligner[] = {gsSPEndDisplayList()};
u8 dorrieeyes_eyeframenessie2_ci4[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 
	0x11, 0x11, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x11, 0x22, 0x33, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x43, 0x32, 
	0x22, 0x21, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0x11, 0x22, 0x34, 0x56, 0x67, 0x89, 
	0xaa, 0xbc, 0xcc, 0xdd, 0xdd, 0xdd, 0xdc, 0xcc, 
	0xba, 0x98, 0x76, 0x54, 0x33, 0x32, 0x21, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x11, 0x22, 0x34, 0x56, 0x79, 0xab, 0xbc, 0xcd, 
	0xde, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xdd, 0xcb, 0xa9, 0x86, 0x54, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 
	0x23, 0x45, 0x68, 0x9a, 0xbc, 0xcd, 0xdd, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xed, 0xca, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 
	0x46, 0x89, 0xab, 0xcc, 0xdd, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x46, 
	0x89, 0xab, 0xcc, 0xdd, 0xde, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x35, 0x89, 
	0xab, 0xbc, 0xcd, 0xdd, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x24, 0x69, 0x9a, 
	0xbb, 0xcc, 0xcd, 0xdd, 0xde, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x00, 0x13, 0x47, 0x9a, 0xab, 
	0xbb, 0xbc, 0xcd, 0xdd, 0xde, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x00, 0x25, 0x89, 0x9a, 0xab, 
	0xbb, 0xcc, 0xcd, 0xdd, 0xde, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x01, 0x48, 0x99, 0xaa, 0xab, 
	0xbb, 0xcc, 0xcd, 0xdd, 0xdd, 0xde, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x03, 0x78, 0x9a, 0xaa, 0xbb, 
	0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x15, 0x89, 0xaa, 0xaa, 0xab, 
	0xbb, 0xbc, 0xcc, 0xcd, 0xdd, 0xdd, 0xed, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x26, 0x89, 0xaa, 0xaa, 0xab, 
	0xbb, 0xbc, 0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 0xde, 
	0xee, 0xee, 0xed, 0xed, 0xdd, 0xdd, 0xdd, 0xde, 
	0xde, 0xde, 0xde, 0xee, 0xee, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x47, 0x9a, 0xaa, 0xaa, 0xbb, 
	0xbb, 0xbc, 0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xed, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xed, 0xee, 0xee, 0xee, 
	0x00, 0x00, 0x00, 0x57, 0x9a, 0xaa, 0xaa, 0xbb, 
	0xbb, 0xbc, 0xcc, 0xcc, 0xcd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xee, 0xee, 0xed, 
	0x00, 0x00, 0x01, 0x57, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xbb, 0xbb, 0xcc, 0xcc, 0xcd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xde, 0xdd, 
	0x00, 0x00, 0x01, 0x58, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xbb, 0xbb, 0xbc, 0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x01, 0x58, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xab, 0xbb, 0xbc, 0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x01, 0x58, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xbb, 0xbb, 0xbb, 0xcc, 0xcc, 0xcd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x01, 0x58, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xab, 0xbb, 0xbb, 0xcc, 0xcc, 0xcc, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x01, 0x58, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xab, 0xbb, 0xbb, 0xbc, 0xcc, 0xcc, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x01, 0x48, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xaa, 0xbb, 0xbb, 0xbc, 0xcc, 0xcc, 0xcd, 0xcd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x37, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xaa, 0xab, 0xbb, 0xbb, 0xcc, 0xcc, 0xcc, 0xcc, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x37, 0x99, 0x9a, 0xaa, 0xaa, 
	0xaa, 0xab, 0xbb, 0xbb, 0xbc, 0xcc, 0xcc, 0xcc, 
	0xdc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x27, 0x99, 0x99, 0x9a, 0xaa, 
	0xaa, 0xaa, 0xbb, 0xbb, 0xbb, 0xcc, 0xcc, 0xcc, 
	0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x16, 0x99, 0x99, 0x99, 0x9a, 
	0xaa, 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xcc, 
	0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x15, 0x99, 0x99, 0x99, 0x9a, 
	0xaa, 0xaa, 0xaa, 0xab, 0xbb, 0xbb, 0xcc, 0xcc, 
	0xcc, 0xcc, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x04, 0x89, 0x99, 0x99, 0x99, 
	0x9a, 0xaa, 0xaa, 0xab, 0xbb, 0xbb, 0xbb, 0xcc, 
	0xcc, 0xcc, 0xcc, 0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x03, 0x88, 0x99, 0x99, 0x99, 
	0x99, 0xaa, 0xaa, 0xaa, 0xab, 0xbb, 0xbb, 0xbb, 
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd, 0xcd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x03, 0x68, 0x89, 0x99, 0x99, 
	0x99, 0x9a, 0xaa, 0xaa, 0xaa, 0xbb, 0xbb, 0xbb, 
	0xbc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd, 
	0xcc, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdc, 
	0x00, 0x00, 0x00, 0x02, 0x57, 0x88, 0x88, 0x99, 
	0x99, 0x99, 0x9a, 0xaa, 0xaa, 0xaa, 0xbb, 0xbb, 
	0xbb, 0xbb, 0xcb, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xdc, 0xcc, 
	0x00, 0x00, 0x00, 0x0f, 0x46, 0x78, 0x88, 0x88, 
	0x99, 0x99, 0x99, 0xaa, 0xaa, 0xaa, 0xab, 0xbb, 
	0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbc, 0xcc, 0xcc, 
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 
	0x00, 0x00, 0x00, 0x01, 0x35, 0x67, 0x88, 0x88, 
	0x89, 0x99, 0x99, 0x9a, 0xaa, 0xaa, 0xaa, 0xab, 
	0xbb, 0xab, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbc, 
	0xbb, 0xbb, 0xcb, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 
	0x00, 0x00, 0x00, 0x01, 0x34, 0x67, 0x78, 0x88, 
	0x88, 0x99, 0x99, 0x99, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xaa, 0xaa, 0xab, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 
	0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 
	0x00, 0x00, 0x00, 0x01, 0x24, 0x56, 0x77, 0x78, 
	0x88, 0x88, 0x99, 0x99, 0x99, 0x9a, 0xaa, 0xaa, 
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xbb, 0xbb, 
	0xbb, 0xba, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 
	0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 0x66, 0x77, 
	0x78, 0x88, 0x89, 0x99, 0x99, 0x99, 0x99, 0xaa, 
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
	0x00, 0x00, 0x00, 0x01, 0x23, 0x44, 0x56, 0x67, 
	0x77, 0x88, 0x88, 0x89, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0xa9, 0xaa, 0x9a, 0xaa, 0xaa, 0xaa, 
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa9, 
	0x00, 0x00, 0x00, 0x01, 0xf2, 0x34, 0x55, 0x66, 
	0x77, 0x78, 0x88, 0x88, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0xa9, 0x99, 
	0x00, 0x00, 0x00, 0x01, 0x12, 0x33, 0x44, 0x56, 
	0x67, 0x77, 0x88, 0x88, 0x88, 0x88, 0x89, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x01, 0x1f, 0x23, 0x34, 0x55, 
	0x66, 0x77, 0x78, 0x88, 0x88, 0x88, 0x89, 0x89, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x22, 0x33, 0x45, 
	0x56, 0x67, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x89, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x11, 0xf2, 0x23, 0x34, 
	0x55, 0x66, 0x77, 0x77, 0x78, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0xf2, 0x23, 0x34, 
	0x45, 0x56, 0x67, 0x77, 0x77, 0x77, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0x22, 0x33, 
	0x44, 0x55, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0x22, 0x23, 
	0x34, 0x45, 0x56, 0x67, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xf2, 0x22, 
	0x33, 0x44, 0x55, 0x66, 0x67, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x87, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x87, 0x88, 0x88, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xff, 0x22, 
	0x23, 0x34, 0x45, 0x56, 0x66, 0x66, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x1f, 0xf2, 
	0x22, 0x33, 0x44, 0x55, 0x56, 0x66, 0x66, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0xff, 
	0x22, 0x23, 0x34, 0x44, 0x55, 0x56, 0x66, 0x66, 
	0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0xff, 
	0xf2, 0x22, 0x23, 0x34, 0x44, 0x55, 0x55, 0x56, 
	0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x1f, 
	0xff, 0x22, 0x22, 0x23, 0x33, 0x44, 0x44, 0x55, 
	0x55, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x67, 0x76, 0x67, 0x77, 0x77, 0x77, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 
	0xff, 0xff, 0x22, 0x22, 0x33, 0x33, 0x33, 0x44, 
	0x45, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 
	0xff, 0xff, 0xf2, 0x22, 0x22, 0x22, 0x23, 0x33, 
	0x34, 0x44, 0x45, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x55, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 
	0xff, 0xff, 0xff, 0xff, 0x2f, 0xff, 0xf2, 0x22, 
	0x23, 0x33, 0x34, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x44, 0x44, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x22, 0x22, 0x22, 0x23, 0x33, 0x33, 0x33, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 
	0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x22, 0x22, 0x22, 0x22, 0x22, 
	0x22, 0x22, 0x22, 0x22, 0xff, 0xff, 0xf2, 0x2f, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x11, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x11, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x11, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x11, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x2f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	
};

Gfx dorrieeyes_eyeframenessie2_ci4_pal_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 dorrieeyes_eyeframenessie2_ci4_pal_rgba16[] = {
	0x00, 0x01, 0x08, 0x03, 0x10, 0x45, 0x18, 0x09, 
	0x20, 0x0d, 0x28, 0x13, 0x38, 0x17, 0x40, 0x1b, 
	0x48, 0x1d, 0x50, 0x21, 0x58, 0x25, 0x60, 0x29, 
	0x70, 0x2b, 0x78, 0x2f, 0x78, 0x31, 0x08, 0x43, 
	
};

Gfx dorrieeyes_unkno2wn_ci4_aligner[] = {gsSPEndDisplayList()};
u8 dorrieeyes_unkno2wn_ci4[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 
	0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x12, 0x22, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x22, 0x21, 
	0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x11, 0x23, 0x34, 0x45, 0x55, 
	0x66, 0x77, 0x78, 0x88, 0x88, 0x88, 0x88, 0x77, 
	0x76, 0x55, 0x44, 0x33, 0x22, 0x21, 0x11, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x11, 0x23, 0x34, 0x45, 0x66, 0x77, 0x88, 
	0x89, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x88, 0x77, 0x65, 0x54, 0x32, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x12, 0x34, 0x45, 0x56, 0x77, 0x88, 0x88, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x98, 0x86, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x12, 
	0x34, 0x55, 0x66, 0x77, 0x88, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 
	0x55, 0x66, 0x77, 0x88, 0x89, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x24, 0x55, 
	0x66, 0x77, 0x78, 0x88, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x45, 0x66, 
	0x67, 0x77, 0x78, 0x88, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x34, 0x56, 0x66, 
	0x77, 0x77, 0x88, 0x88, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x00, 0x13, 0x55, 0x66, 0x66, 
	0x77, 0x77, 0x78, 0x88, 0x89, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x00, 0x35, 0x56, 0x66, 0x66, 
	0x77, 0x77, 0x78, 0x88, 0x88, 0x89, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x02, 0x45, 0x56, 0x66, 0x66, 
	0x77, 0x77, 0x78, 0x88, 0x88, 0x88, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x04, 0x55, 0x66, 0x66, 0x66, 
	0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x24, 0x55, 0x66, 0x66, 0x66, 
	0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x89, 
	0x99, 0x99, 0x99, 0x98, 0x88, 0x88, 0x89, 0x89, 
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x34, 0x56, 0x66, 0x66, 0x66, 
	0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 
	0x98, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x98, 0x99, 0x99, 0x99, 
	0x00, 0x00, 0x00, 0x34, 0x56, 0x66, 0x66, 0x66, 
	0x67, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x99, 0x99, 0x98, 
	0x00, 0x00, 0x00, 0x35, 0x56, 0x66, 0x66, 0x66, 
	0x67, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x89, 0x89, 0x88, 
	0x00, 0x00, 0x00, 0x45, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x77, 0x77, 0x77, 0x78, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x89, 0x88, 
	0x00, 0x00, 0x00, 0x45, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x77, 0x77, 0x77, 0x78, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x45, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x67, 0x77, 0x77, 0x78, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x01, 0x45, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x67, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x45, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x67, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x35, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x25, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x67, 0x77, 0x77, 0x77, 0x78, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x24, 0x56, 0x56, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x14, 0x55, 0x56, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x14, 0x55, 0x55, 0x55, 0x66, 
	0x66, 0x66, 0x66, 0x67, 0x77, 0x77, 0x77, 0x77, 
	0x78, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x03, 0x55, 0x55, 0x55, 0x56, 
	0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x78, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x03, 0x55, 0x55, 0x55, 0x55, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x78, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x02, 0x55, 0x55, 0x55, 0x55, 
	0x56, 0x66, 0x66, 0x66, 0x66, 0x67, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x66, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x01, 0x45, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x56, 0x66, 0x66, 0x66, 0x66, 0x67, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 
	0x88, 0x78, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x00, 0x00, 0x00, 0x01, 0x34, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x56, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x67, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x78, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x23, 0x45, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x76, 
	0x00, 0x00, 0x00, 0x00, 0x11, 0x23, 0x45, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x65, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x77, 0x76, 0x66, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x12, 0x34, 0x45, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x56, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x22, 0x34, 
	0x45, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x66, 0x55, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 0x22, 
	0x34, 0x44, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 
	0x22, 0x22, 0x33, 0x34, 0x44, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x44, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x12, 0x22, 0x23, 0x33, 0x33, 
	0x34, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x33, 0x32, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x12, 0x22, 0x22, 0x22, 0x22, 
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x11, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcc, 
	0xcc, 0xcb, 0xbb, 0xbb, 0xaa, 0xaa, 0xa1, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1a, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0xcd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdc, 0x00, 
	0x0a, 0xee, 0xff, 0xb1, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xef, 0xee, 0xea, 0x00, 0x0c, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xcc, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdc, 0x00, 
	0x0a, 0xef, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0a, 0xff, 0xfe, 0xba, 0x00, 0x0c, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xbc, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0x00, 
	0x00, 0xef, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0b, 0xff, 0xfe, 0xb0, 0x00, 0x0d, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 
	0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0x00, 
	0x00, 0xef, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xaf, 0xff, 0xfe, 0xa0, 0x00, 0x0d, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 
	0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xa0, 
	0x00, 0xaf, 0xff, 0xff, 0xb0, 0x00, 0x00, 0x00, 
	0x0a, 0xef, 0xff, 0xfe, 0x00, 0x00, 0xcd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 
	0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xc0, 
	0x00, 0x0e, 0xff, 0xef, 0xfe, 0xa0, 0x00, 0x00, 
	0xbf, 0xff, 0xef, 0xfb, 0x00, 0x0a, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 
	0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xda, 
	0x00, 0x0b, 0xff, 0xee, 0xff, 0xfe, 0xee, 0xef, 
	0xff, 0xfe, 0xff, 0xe0, 0x00, 0x0c, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 
	0xbc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdc, 
	0x00, 0x00, 0xef, 0xfe, 0xef, 0xff, 0xff, 0xff, 
	0xfe, 0xef, 0xff, 0xa0, 0x00, 0x0d, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xac, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xa0, 0x00, 0x0e, 0xff, 0xfe, 0xee, 0xee, 0xfe, 
	0xee, 0xff, 0xfb, 0x00, 0x00, 0xcd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xab, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xc0, 0x00, 0x00, 0xef, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xb0, 0x00, 0x0b, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0a, 0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdc, 0x00, 0x00, 0x0b, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xea, 0x00, 0x00, 0x0d, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0a, 0xbc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xc0, 0x00, 0x00, 0x0a, 0xee, 0xee, 0xee, 
	0xba, 0x00, 0x00, 0x00, 0xcd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xac, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0c, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xab, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xca, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0b, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0a, 0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xca, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0c, 0xcd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0a, 0xbc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdc, 0xba, 0x0a, 
	0x0a, 0xcc, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 
	
};

Gfx dorrieeyes_unkno2wn_ci4_pal_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 dorrieeyes_unkno2wn_ci4_pal_rgba16[] = {
	0x00, 0x01, 0x08, 0x43, 0x18, 0x09, 0x20, 0x0f, 
	0x38, 0x17, 0x50, 0x21, 0x58, 0x25, 0x68, 0x2b, 
	0x78, 0x2f, 0x78, 0x31, 0x21, 0x09, 0x3a, 0x53, 
	0x84, 0x21, 0xf7, 0xbd, 0x34, 0x5f, 0x4d, 0xe9, 
	
};

Vtx dorrieeyes_Plane_mesh_vtx_cull[8] = {
	{{{-459, 0, -459},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-459, 0, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-459, 0, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-459, 0, -459},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{459, 0, -459},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{459, 0, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{459, 0, 0},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{459, 0, -459},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
};

Vtx dorrieeyes_Plane_mesh_vtx_0[3] = {
	{{{459, 0, 0},0, {-14, -12},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-459, 0, -459},0, {2028, 2030},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-459, 0, 0},0, {-14, 2030},{0xFF, 0xFF, 0xFF, 0xFF}}},
};

Gfx dorrieeyes_Plane_mesh_tri_0[] = {
	gsSPVertex(dorrieeyes_Plane_mesh_vtx_0 + 0, 3, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSPEndDisplayList(),
};

Vtx dorrieeyes_Plane_mesh_vtx_1[3] = {
	{{{459, 0, 0},0, {2032, 1008},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{459, 0, -459},0, {2032, -16},{0xFF, 0xFF, 0xFF, 0xFF}}},
	{{{-459, 0, -459},0, {-16, -16},{0xFF, 0xFF, 0xFF, 0xFF}}},
};

Gfx dorrieeyes_Plane_mesh_tri_1[] = {
	gsSPVertex(dorrieeyes_Plane_mesh_vtx_1 + 0, 3, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSPEndDisplayList(),
};

Gfx mat_dorrieeyes_eyeclosed[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, TEXEL0, 0, SHADE, 0, 0, 0, 0, 1),
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrieeyes_eyeframenessie2_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 15),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_16b, 1, dorrieeyes_eyeframenessie2_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPLoadSync(),
	gsDPLoadBlock(7, 0, 0, 1023, 512),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPSetTileSize(0, 0, 0, 252, 252),
	gsSPSetLights1(dorrieeyes_eyeclosed_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_dorrieeyes_eyeclosed[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

Gfx mat_dorrieeyes_eyehalf[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, TEXEL0, 0, SHADE, 0, 0, 0, 0, 1),
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrieeyes_unkno2wn_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 15),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_16b, 1, dorrieeyes_unkno2wn_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPLoadSync(),
	gsDPLoadBlock(7, 0, 0, 1023, 512),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPSetTileSize(0, 0, 0, 252, 252),
	gsSPSetLights1(dorrieeyes_eyehalf_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_dorrieeyes_eyehalf[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

Gfx dorrieeyes_Plane_mesh[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(dorrieeyes_Plane_mesh_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_dorrieeyes_eyeclosed),
	gsSPDisplayList(dorrieeyes_Plane_mesh_tri_0),
	gsSPDisplayList(mat_revert_dorrieeyes_eyeclosed),
	gsSPDisplayList(mat_dorrieeyes_eyehalf),
	gsSPDisplayList(dorrieeyes_Plane_mesh_tri_1),
	gsSPDisplayList(mat_revert_dorrieeyes_eyehalf),
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsSPEndDisplayList(),
};

