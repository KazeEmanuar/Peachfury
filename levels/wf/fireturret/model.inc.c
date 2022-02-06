Lights1 fireturret_f3d_material_006_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Lights1 fireturret_f3d_material_005_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFE, 0xFE, 0xFE, 0x28, 0x28, 0x28);

Gfx fireturret_Texture0565_RGB_ci8_aligner[] = {gsSPEndDisplayList()};
u8 fireturret_Texture0565_RGB_ci8[] = {
	0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 
	0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 
	0x00, 0x08, 0x09, 0x03, 0x05, 0x0a, 0x0b, 0x0c, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x0d, 0x0e, 0x0f, 0x10, 0x02, 0x11, 
	0x04, 0x04, 0x05, 0x12, 0x07, 0x07, 0x07, 0x07, 
	0x00, 0x09, 0x00, 0x13, 0x06, 0x07, 0x0c, 0x14, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x15, 0x00, 0x04, 0x16, 0x0b, 0x11, 0x17, 
	0x04, 0x05, 0x12, 0x12, 0x07, 0x07, 0x07, 0x18, 
	0x00, 0x09, 0x03, 0x06, 0x07, 0x0c, 0x19, 0x1a, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0e, 0x04, 0x12, 0x1b, 0x11, 0x1c, 0x1d, 
	0x05, 0x05, 0x12, 0x12, 0x07, 0x07, 0x18, 0x0c, 
	0x00, 0x1e, 0x05, 0x07, 0x0c, 0x19, 0x17, 0x1f, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x1c, 
	0x20, 0x1e, 0x12, 0x0a, 0x21, 0x22, 0x23, 0x24, 
	0x12, 0x12, 0x12, 0x0a, 0x0a, 0x0a, 0x0a, 0x1b, 
	0x00, 0x25, 0x0a, 0x0c, 0x19, 0x17, 0x1d, 0x26, 
	0x01, 0x27, 0x28, 0x28, 0x29, 0x26, 0x1f, 0x17, 
	0x13, 0x25, 0x0a, 0x0b, 0x19, 0x2a, 0x24, 0x26, 
	0x07, 0x07, 0x07, 0x07, 0x0a, 0x0a, 0x1b, 0x0b, 
	0x00, 0x10, 0x0b, 0x19, 0x17, 0x1d, 0x2b, 0x28, 
	0x01, 0x28, 0x2c, 0x29, 0x2d, 0x1d, 0x17, 0x1c, 
	0x13, 0x2e, 0x0b, 0x2f, 0x2a, 0x24, 0x30, 0x28, 
	0x07, 0x07, 0x07, 0x18, 0x0a, 0x1b, 0x0b, 0x0b, 
	0x00, 0x0b, 0x19, 0x1a, 0x1f, 0x26, 0x28, 0x27, 
	0x01, 0x28, 0x29, 0x2b, 0x31, 0x17, 0x19, 0x22, 
	0x13, 0x0b, 0x19, 0x17, 0x26, 0x26, 0x28, 0x27, 
	0x07, 0x07, 0x18, 0x32, 0x1b, 0x0b, 0x0b, 0x0b, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x29, 0x2d, 0x31, 0x33, 0x2f, 0x21, 0x00, 
	0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 
	0x07, 0x18, 0x0c, 0x0c, 0x0b, 0x0b, 0x0b, 0x34, 
	0x00, 0x28, 0x2c, 0x29, 0x30, 0x24, 0x2a, 0x1c, 
	0x00, 0x2b, 0x31, 0x33, 0x2f, 0x21, 0x0b, 0x35, 
	0x04, 0x36, 0x25, 0x35, 0x37, 0x38, 0x1c, 0x23, 
	0x39, 0x2d, 0x3a, 0x3b, 0x31, 0x3c, 0x3d, 0x3e, 
	0x00, 0x2c, 0x3a, 0x2d, 0x39, 0x3f, 0x1c, 0x04, 
	0x00, 0x1d, 0x33, 0x2f, 0x21, 0x0b, 0x0a, 0x40, 
	0x04, 0x0f, 0x12, 0x0a, 0x0c, 0x19, 0x41, 0x17, 
	0x1d, 0x24, 0x30, 0x29, 0x41, 0x42, 0x1d, 0x14, 
	0x00, 0x29, 0x2d, 0x31, 0x3f, 0x22, 0x11, 0x02, 
	0x00, 0x33, 0x43, 0x44, 0x02, 0x35, 0x40, 0x36, 
	0x05, 0x40, 0x0a, 0x0b, 0x3b, 0x19, 0x17, 0x2a, 
	0x24, 0x30, 0x29, 0x45, 0x41, 0x1d, 0x3e, 0x46, 
	0x00, 0x24, 0x31, 0x33, 0x2f, 0x47, 0x0c, 0x0b, 
	0x20, 0x13, 0x13, 0x13, 0x23, 0x04, 0x04, 0x05, 
	0x05, 0x10, 0x0a, 0x0b, 0x19, 0x19, 0x17, 0x1d, 
	0x24, 0x30, 0x29, 0x2c, 0x42, 0x1d, 0x3e, 0x46, 
	0x00, 0x2a, 0x1c, 0x11, 0x0b, 0x0c, 0x07, 0x0a, 
	0x13, 0x48, 0x05, 0x12, 0x0a, 0x40, 0x10, 0x1b, 
	0x12, 0x35, 0x0b, 0x21, 0x38, 0x1c, 0x23, 0x39, 
	0x2d, 0x3a, 0x49, 0x41, 0x42, 0x3e, 0x30, 0x46, 
	0x00, 0x17, 0x11, 0x0b, 0x0a, 0x07, 0x06, 0x12, 
	0x13, 0x05, 0x23, 0x06, 0x07, 0x0a, 0x0c, 0x47, 
	0x07, 0x0b, 0x0c, 0x19, 0x41, 0x17, 0x31, 0x07, 
	0x4a, 0x3a, 0x41, 0x42, 0x1d, 0x3e, 0x45, 0x1f, 
	0x00, 0x3b, 0x02, 0x2e, 0x4b, 0x0a, 0x12, 0x48, 
	0x13, 0x12, 0x06, 0x07, 0x4a, 0x0b, 0x3b, 0x14, 
	0x07, 0x0c, 0x4c, 0x1a, 0x1a, 0x1f, 0x24, 0x30, 
	0x29, 0x45, 0x2c, 0x4d, 0x4e, 0x46, 0x46, 0x4f, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x13, 0x13, 
	0x23, 0x0a, 0x07, 0x0c, 0x0c, 0x11, 0x19, 0x1a, 
	0x07, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0b, 0x21, 
	0x11, 0x11, 0x11, 0x22, 0x22, 0x1c, 0x1c, 0x33, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x13, 0x13, 0x23, 
	0x23, 0x0b, 0x0c, 0x3b, 0x19, 0x1c, 0x2a, 0x1f, 
	0x18, 0x27, 0x50, 0x50, 0x50, 0x50, 0x4e, 0x50, 
	0x50, 0x50, 0x50, 0x51, 0x51, 0x51, 0x28, 0x41, 
	0x00, 0x00, 0x00, 0x02, 0x02, 0x13, 0x23, 0x23, 
	0x23, 0x11, 0x3b, 0x19, 0x17, 0x23, 0x24, 0x26, 
	0x0c, 0x50, 0x46, 0x4e, 0x4e, 0x4e, 0x1d, 0x4d, 
	0x4d, 0x4d, 0x52, 0x52, 0x52, 0x2c, 0x2c, 0x3c, 
	0x00, 0x00, 0x02, 0x02, 0x03, 0x23, 0x23, 0x23, 
	0x53, 0x1c, 0x19, 0x17, 0x1d, 0x39, 0x30, 0x26, 
	0x0c, 0x50, 0x4e, 0x4e, 0x4e, 0x4d, 0x1d, 0x4d, 
	0x4d, 0x52, 0x52, 0x52, 0x2c, 0x2c, 0x2c, 0x3c, 
	0x00, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 
	0x12, 0x12, 0x3f, 0x31, 0x07, 0x4a, 0x29, 0x28, 
	0x0c, 0x50, 0x4e, 0x4e, 0x4d, 0x4d, 0x42, 0x4d, 
	0x52, 0x52, 0x52, 0x2c, 0x2c, 0x2c, 0x2c, 0x39, 
	0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x12, 
	0x12, 0x54, 0x39, 0x2d, 0x4a, 0x3a, 0x2c, 0x51, 
	0x0c, 0x50, 0x4e, 0x4d, 0x4d, 0x08, 0x0d, 0x0e, 
	0x0f, 0x10, 0x02, 0x11, 0x2c, 0x2c, 0x2c, 0x39, 
	0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x12, 
	0x12, 0x21, 0x4a, 0x3a, 0x3b, 0x41, 0x42, 0x4e, 
	0x0b, 0x4e, 0x1d, 0x1d, 0x42, 0x15, 0x00, 0x04, 
	0x16, 0x0b, 0x11, 0x17, 0x2c, 0x2c, 0x2c, 0x39, 
	0x03, 0x03, 0x04, 0x23, 0x53, 0x06, 0x07, 0x12, 
	0x0a, 0x4a, 0x29, 0x2c, 0x2c, 0x4d, 0x4e, 0x50, 
	0x21, 0x4d, 0x1d, 0x42, 0x42, 0x0e, 0x04, 0x12, 
	0x1b, 0x11, 0x1c, 0x1d, 0x2c, 0x2c, 0x2c, 0x31, 
	0x03, 0x04, 0x04, 0x53, 0x06, 0x07, 0x07, 0x0a, 
	0x0a, 0x0a, 0x0a, 0x1b, 0x0b, 0x0b, 0x0b, 0x34, 
	0x11, 0x4d, 0x42, 0x42, 0x42, 0x1e, 0x12, 0x0a, 
	0x21, 0x22, 0x23, 0x24, 0x41, 0x41, 0x49, 0x31, 
	0x04, 0x50, 0x4e, 0x51, 0x28, 0x28, 0x28, 0x29, 
	0x30, 0x2b, 0x24, 0x24, 0x1d, 0x2a, 0x17, 0x1c, 
	0x11, 0x4d, 0x42, 0x17, 0x17, 0x25, 0x0a, 0x0b, 
	0x19, 0x2a, 0x24, 0x26, 0x41, 0x49, 0x49, 0x31, 
	0x04, 0x4e, 0x1d, 0x52, 0x2c, 0x2c, 0x29, 0x3a, 
	0x4a, 0x2d, 0x39, 0x31, 0x23, 0x3f, 0x3f, 0x16, 
	0x11, 0x52, 0x17, 0x17, 0x41, 0x2e, 0x0b, 0x2f, 
	0x2a, 0x24, 0x30, 0x28, 0x2c, 0x49, 0x49, 0x31, 
	0x05, 0x4e, 0x42, 0x17, 0x41, 0x3b, 0x3a, 0x4a, 
	0x4a, 0x2d, 0x39, 0x31, 0x23, 0x3f, 0x3f, 0x16, 
	0x2f, 0x52, 0x17, 0x41, 0x41, 0x0b, 0x19, 0x17, 
	0x26, 0x26, 0x28, 0x27, 0x2c, 0x49, 0x49, 0x31, 
	0x05, 0x4e, 0x42, 0x41, 0x49, 0x3b, 0x3a, 0x4a, 
	0x2d, 0x2d, 0x39, 0x31, 0x23, 0x3f, 0x33, 0x16, 
	0x22, 0x52, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x49, 0x2c, 0x2c, 0x2c, 0x2c, 0x49, 0x49, 0x31, 
	0x12, 0x4d, 0x42, 0x41, 0x49, 0x29, 0x29, 0x30, 
	0x2b, 0x39, 0x39, 0x31, 0x23, 0x3f, 0x33, 0x16, 
	0x1c, 0x2c, 0x41, 0x41, 0x41, 0x41, 0x41, 0x49, 
	0x49, 0x2c, 0x2c, 0x2c, 0x2c, 0x49, 0x49, 0x31, 
	0x12, 0x4d, 0x17, 0x2c, 0x2c, 0x29, 0x30, 0x30, 
	0x2b, 0x24, 0x24, 0x31, 0x23, 0x3f, 0x33, 0x16, 
	0x1c, 0x2c, 0x41, 0x41, 0x41, 0x41, 0x49, 0x49, 
	0x49, 0x49, 0x3b, 0x49, 0x49, 0x49, 0x49, 0x31, 
	0x0a, 0x42, 0x1d, 0x17, 0x41, 0x49, 0x3b, 0x3a, 
	0x4a, 0x4a, 0x2d, 0x07, 0x54, 0x0a, 0x0a, 0x55, 
	0x33, 0x41, 0x39, 0x31, 0x31, 0x31, 0x31, 0x31, 
	0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x56, 
	
};

Gfx fireturret_Texture0565_RGB_ci8_pal_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 fireturret_Texture0565_RGB_ci8_pal_rgba16[] = {
	0xac, 0xa5, 0x83, 0x59, 0xa4, 0xa5, 0xa4, 0xa3, 
	0x9c, 0xa3, 0x9c, 0x61, 0x72, 0xd5, 0x6a, 0xd5, 
	0xf7, 0xbb, 0xde, 0x2f, 0x94, 0x1f, 0x8b, 0xdd, 
	0x62, 0x93, 0xd6, 0x2d, 0xd5, 0xed, 0xc5, 0x69, 
	0xb5, 0x27, 0x83, 0x9b, 0x94, 0x61, 0x7b, 0x17, 
	0x31, 0x09, 0xd5, 0xef, 0x94, 0x21, 0x52, 0x11, 
	0x6a, 0x95, 0x5a, 0x51, 0x31, 0x07, 0x93, 0xdf, 
	0x7b, 0x59, 0x4a, 0x0f, 0xcd, 0xab, 0x29, 0x07, 
	0x7b, 0x19, 0x83, 0x9d, 0x7b, 0x5b, 0x73, 0x17, 
	0x49, 0xcf, 0xbd, 0x69, 0x28, 0xc7, 0x08, 0x01, 
	0x20, 0xc5, 0x41, 0x8d, 0x52, 0x0f, 0x49, 0xcd, 
	0x39, 0x8b, 0x62, 0xd5, 0xac, 0xe5, 0x7b, 0x9b, 
	0x41, 0xcd, 0x6b, 0x17, 0x6a, 0x93, 0x73, 0x59, 
	0x8b, 0x9d, 0xb4, 0xe7, 0xde, 0xf5, 0x8b, 0x9b, 
	0x83, 0x5b, 0x6a, 0xd7, 0x5a, 0x93, 0x5a, 0x53, 
	0x62, 0xd7, 0x5a, 0x95, 0x41, 0xcf, 0x73, 0x19, 
	0xbd, 0x29, 0x52, 0x51, 0x4a, 0x11, 0x94, 0x63, 
	0x9c, 0xa5, 0x39, 0x8d, 0x29, 0x09, 0x62, 0x53, 
	0xcd, 0xed, 0x52, 0x53, 0x62, 0x95, 0xbd, 0x27, 
	0x39, 0x09, 0x31, 0x4b, 0x31, 0x49, 0x10, 0x83, 
	0x18, 0x85, 0x20, 0x85, 0x39, 0x4b, 0x72, 0xd7, 
	0x8b, 0xdf, 0xa5, 0x27, 0x7b, 0xdd, 
};

Gfx fireturret_Texture0564_RGB_ci8_aligner[] = {gsSPEndDisplayList()};
u8 fireturret_Texture0564_RGB_ci8[] = {
	0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 
	0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 
	0x00, 0x08, 0x09, 0x03, 0x05, 0x0a, 0x0b, 0x0c, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x0d, 0x0e, 0x0f, 0x10, 0x02, 0x11, 
	0x04, 0x04, 0x05, 0x12, 0x07, 0x07, 0x07, 0x07, 
	0x00, 0x09, 0x00, 0x13, 0x06, 0x07, 0x0c, 0x14, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x15, 0x00, 0x04, 0x16, 0x0b, 0x11, 0x17, 
	0x04, 0x05, 0x12, 0x12, 0x07, 0x07, 0x07, 0x18, 
	0x00, 0x09, 0x03, 0x06, 0x07, 0x0c, 0x19, 0x1a, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x1b, 0x01, 0x01, 
	0x01, 0x1c, 0x1b, 0x12, 0x1d, 0x11, 0x1e, 0x1f, 
	0x05, 0x05, 0x12, 0x12, 0x07, 0x07, 0x18, 0x0c, 
	0x00, 0x20, 0x05, 0x07, 0x0c, 0x19, 0x17, 0x21, 
	0x01, 0x01, 0x01, 0x22, 0x01, 0x22, 0x23, 0x23, 
	0x23, 0x23, 0x1b, 0x1b, 0x1b, 0x1b, 0x24, 0x25, 
	0x12, 0x12, 0x12, 0x0a, 0x0a, 0x0a, 0x0a, 0x1d, 
	0x00, 0x26, 0x0a, 0x0c, 0x19, 0x17, 0x1f, 0x27, 
	0x01, 0x28, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x01, 0x01, 0x22, 
	0x07, 0x07, 0x07, 0x07, 0x0a, 0x0a, 0x1d, 0x0b, 
	0x00, 0x10, 0x0b, 0x19, 0x17, 0x1f, 0x29, 0x1a, 
	0x22, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x22, 
	0x01, 0x07, 0x07, 0x18, 0x0a, 0x1d, 0x0b, 0x0b, 
	0x00, 0x0b, 0x19, 0x1a, 0x21, 0x21, 0x1a, 0x21, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x01, 0x01, 0x18, 0x2a, 0x1d, 0x0b, 0x0b, 0x0b, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x01, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x01, 0x01, 0x0c, 0x0b, 0x0b, 0x0b, 0x2b, 
	0x00, 0x2c, 0x2d, 0x28, 0x2e, 0x22, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x1c, 0x2f, 0x30, 0x31, 0x32, 0x33, 
	0x00, 0x2d, 0x34, 0x35, 0x1b, 0x01, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x01, 0x01, 0x36, 0x37, 0x1f, 0x14, 
	0x00, 0x28, 0x35, 0x30, 0x01, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x01, 0x36, 0x1f, 0x33, 0x38, 
	0x00, 0x25, 0x30, 0x39, 0x01, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x01, 0x37, 0x1f, 0x33, 0x38, 
	0x00, 0x29, 0x1e, 0x1b, 0x01, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x1b, 0x1b, 0x33, 0x2e, 0x38, 
	0x00, 0x17, 0x11, 0x01, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x1b, 0x33, 0x3a, 0x21, 
	0x00, 0x2f, 0x02, 0x1c, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x01, 0x38, 0x38, 0x3b, 
	0x00, 0x00, 0x00, 0x01, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x1b, 0x1e, 0x1e, 0x39, 
	0x00, 0x00, 0x00, 0x01, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x22, 0x3c, 0x2c, 0x36, 
	0x00, 0x00, 0x00, 0x1b, 0x1b, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x01, 0x01, 0x2d, 0x2d, 0x31, 
	0x00, 0x00, 0x02, 0x02, 0x1b, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x01, 0x2d, 0x2d, 0x2d, 0x31, 
	0x00, 0x02, 0x02, 0x03, 0x1b, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x01, 0x2d, 0x2d, 0x2d, 0x3d, 
	0x02, 0x02, 0x03, 0x03, 0x1b, 0x1b, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x1c, 0x01, 0x2d, 0x2d, 0x2d, 0x3d, 
	0x03, 0x03, 0x03, 0x04, 0x04, 0x1b, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x01, 0x17, 0x2d, 0x2d, 0x2d, 0x3d, 
	0x03, 0x03, 0x04, 0x24, 0x3e, 0x01, 0x01, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x01, 0x01, 0x1f, 0x2d, 0x2d, 0x2d, 0x30, 
	0x03, 0x04, 0x04, 0x3e, 0x06, 0x07, 0x01, 0x09, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x1b, 0x1b, 0x24, 0x25, 0x36, 0x36, 0x3f, 0x30, 
	0x04, 0x40, 0x41, 0x3c, 0x2c, 0x2c, 0x2c, 0x01, 
	0x09, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x09, 
	0x1b, 0x29, 0x25, 0x27, 0x36, 0x3f, 0x3f, 0x30, 
	0x04, 0x41, 0x1f, 0x42, 0x2d, 0x2d, 0x28, 0x34, 
	0x1b, 0x09, 0x09, 0x23, 0x23, 0x23, 0x23, 0x23, 
	0x23, 0x23, 0x23, 0x23, 0x23, 0x09, 0x09, 0x1b, 
	0x29, 0x25, 0x2e, 0x2c, 0x2d, 0x3f, 0x3f, 0x30, 
	0x05, 0x41, 0x37, 0x17, 0x36, 0x2f, 0x34, 0x43, 
	0x43, 0x35, 0x1b, 0x08, 0x08, 0x08, 0x23, 0x23, 
	0x23, 0x23, 0x08, 0x08, 0x08, 0x1b, 0x19, 0x17, 
	0x27, 0x27, 0x2c, 0x44, 0x2d, 0x3f, 0x3f, 0x30, 
	0x05, 0x41, 0x37, 0x36, 0x3f, 0x2f, 0x34, 0x43, 
	0x35, 0x35, 0x3d, 0x30, 0x24, 0x1b, 0x08, 0x08, 
	0x08, 0x08, 0x1b, 0x36, 0x36, 0x36, 0x36, 0x36, 
	0x3f, 0x2d, 0x2d, 0x2d, 0x2d, 0x3f, 0x3f, 0x30, 
	0x12, 0x45, 0x37, 0x36, 0x3f, 0x28, 0x28, 0x2e, 
	0x46, 0x3d, 0x3d, 0x30, 0x24, 0x47, 0x39, 0x16, 
	0x1e, 0x2d, 0x36, 0x36, 0x36, 0x36, 0x36, 0x3f, 
	0x3f, 0x2d, 0x2d, 0x2d, 0x2d, 0x3f, 0x3f, 0x30, 
	0x12, 0x45, 0x17, 0x2d, 0x2d, 0x28, 0x2e, 0x2e, 
	0x46, 0x25, 0x25, 0x30, 0x24, 0x47, 0x39, 0x16, 
	0x1e, 0x2d, 0x36, 0x36, 0x36, 0x36, 0x3f, 0x3f, 
	0x3f, 0x3f, 0x2f, 0x3f, 0x3f, 0x3f, 0x3f, 0x30, 
	0x0a, 0x37, 0x1f, 0x17, 0x36, 0x3f, 0x2f, 0x34, 
	0x43, 0x43, 0x35, 0x07, 0x48, 0x0a, 0x0a, 0x49, 
	0x39, 0x36, 0x3d, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x4a, 
	
};

Gfx fireturret_Texture0564_RGB_ci8_pal_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 fireturret_Texture0564_RGB_ci8_pal_rgba16[] = {
	0xac, 0xa5, 0x83, 0x59, 0xa4, 0xa5, 0xa4, 0xa3, 
	0x9c, 0xa3, 0x9c, 0x61, 0x72, 0xd5, 0x6a, 0xd5, 
	0xf7, 0xbb, 0xde, 0x2f, 0x94, 0x1f, 0x8b, 0xdd, 
	0x62, 0x93, 0xd6, 0x2d, 0xd5, 0xed, 0xc5, 0x69, 
	0xb5, 0x27, 0x83, 0x9b, 0x94, 0x61, 0x7b, 0x17, 
	0x31, 0x09, 0xd5, 0xef, 0x94, 0x21, 0x52, 0x11, 
	0x6a, 0x95, 0x5a, 0x51, 0x31, 0x07, 0xb4, 0xa5, 
	0xac, 0xa3, 0x93, 0xdf, 0x7b, 0x59, 0x4a, 0x0f, 
	0xcd, 0xab, 0x29, 0x07, 0x51, 0x8b, 0x00, 0x01, 
	0x73, 0x17, 0x49, 0xcf, 0xbd, 0x69, 0x28, 0xc7, 
	0x41, 0x8d, 0x52, 0x0f, 0x6a, 0x93, 0x8b, 0x9d, 
	0x20, 0xc5, 0x39, 0x8b, 0x41, 0xcd, 0x5a, 0x53, 
	0x6b, 0x17, 0x62, 0xd7, 0x5a, 0x95, 0x41, 0xcf, 
	0x5a, 0x93, 0x62, 0xd5, 0x52, 0x51, 0x4a, 0x11, 
	0x29, 0x09, 0x73, 0x59, 0x39, 0x8d, 0x10, 0x83, 
	0x20, 0x85, 0x6a, 0xd7, 0x72, 0xd7, 0x52, 0x53, 
	0x18, 0x85, 0x31, 0x49, 0x39, 0x4b, 0x62, 0x95, 
	0x08, 0x01, 0x31, 0x4b, 0x49, 0xcd, 0x73, 0x19, 
	0x8b, 0xdf, 0xa5, 0x27, 0x7b, 0xdd, 
};

Vtx fireturret_Circle_001_mesh_layer_1_vtx_cull[8] = {
	{{{-143, 25, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-143, 25, 121},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-143, 178, 121},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{-143, 178, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{143, 25, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{143, 25, 121},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{143, 178, 121},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	{{{143, 178, -150},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
};

Vtx fireturret_Circle_001_mesh_layer_1_vtx_0[20] = {
	{{{-88, 25, 121},0, {1007, -15},{0x97, 0x81, 0x75, 0xFF}}},
	{{{-93, 160, 121},0, {-14, -15},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{-143, 25, -46},0, {1007, 1006},{0x97, 0x81, 0x75, 0xFF}}},
	{{{143, 25, -46},0, {1007, 1006},{0x97, 0x81, 0x75, 0xFF}}},
	{{{93, 160, 121},0, {-14, -15},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{88, 25, 121},0, {1007, -15},{0x97, 0x81, 0x75, 0xFF}}},
	{{{-143, 25, -46},0, {1006, 1007},{0x97, 0x81, 0x75, 0xFF}}},
	{{{-93, 160, 121},0, {-15, -14},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{-65, 160, -70},0, {-15, 1007},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{143, 25, -46},0, {1006, 1007},{0x97, 0x81, 0x75, 0xFF}}},
	{{{65, 160, -70},0, {-15, 1007},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{93, 160, 121},0, {-15, -14},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{0, 25, -150},0, {496, 1008},{0x97, 0x81, 0x75, 0xFF}}},
	{{{65, 160, -70},0, {1008, -16},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{143, 25, -46},0, {1008, 1008},{0x97, 0x81, 0x75, 0xFF}}},
	{{{-65, 160, -70},0, {-16, -16},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{-143, 25, -46},0, {-16, 1008},{0x97, 0x81, 0x75, 0xFF}}},
	{{{0, 178, 121},0, {496, 1008},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{93, 160, 121},0, {1008, 1008},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{-93, 160, 121},0, {-16, 1008},{0xE5, 0xA8, 0xAB, 0xFF}}},
};

Gfx fireturret_Circle_001_mesh_layer_1_tri_0[] = {
	gsSPVertex(fireturret_Circle_001_mesh_layer_1_vtx_0 + 0, 20, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
	gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
	gsSP2Triangles(12, 13, 14, 0, 12, 15, 13, 0),
	gsSP2Triangles(16, 15, 12, 0, 17, 13, 15, 0),
	gsSP2Triangles(17, 18, 13, 0, 15, 19, 17, 0),
	gsSPEndDisplayList(),
};

Vtx fireturret_Circle_001_mesh_layer_1_vtx_1[5] = {
	{{{-88, 25, 121},0, {9, 1008},{0x97, 0x81, 0x75, 0xFF}}},
	{{{0, 178, 121},0, {496, -16},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{-93, 160, 121},0, {-16, -16},{0xE5, 0xA8, 0xAB, 0xFF}}},
	{{{88, 25, 121},0, {983, 1008},{0x97, 0x81, 0x75, 0xFF}}},
	{{{93, 160, 121},0, {1008, -16},{0xE5, 0xA8, 0xAB, 0xFF}}},
};

Gfx fireturret_Circle_001_mesh_layer_1_tri_1[] = {
	gsSPVertex(fireturret_Circle_001_mesh_layer_1_vtx_1 + 0, 5, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 1, 0, 0),
	gsSP1Triangle(3, 4, 1, 0),
	gsSPEndDisplayList(),
};


Gfx mat_fireturret_f3d_material_006[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 1, TEXEL0, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 1),
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fireturret_Texture0565_RGB_ci8_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 86),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 32, fireturret_Texture0565_RGB_ci8),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 4, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 124, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPSetLights1(fireturret_f3d_material_006_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_fireturret_f3d_material_006[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

Gfx mat_fireturret_f3d_material_005[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 1, TEXEL0, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 1),
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fireturret_Texture0564_RGB_ci8_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 74),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 32, fireturret_Texture0564_RGB_ci8),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 4, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 124, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPSetLights1(fireturret_f3d_material_005_lights),
	gsSPEndDisplayList(),
};

Gfx mat_revert_fireturret_f3d_material_005[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

Gfx fireturret_Circle_001_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(fireturret_Circle_001_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_fireturret_f3d_material_006),
	gsSPDisplayList(fireturret_Circle_001_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_fireturret_f3d_material_006),
	gsSPDisplayList(mat_fireturret_f3d_material_005),
	gsSPDisplayList(fireturret_Circle_001_mesh_layer_1_tri_1),
	gsSPDisplayList(mat_revert_fireturret_f3d_material_005),
	gsSPEndDisplayList(),
};

Gfx fireturret_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

