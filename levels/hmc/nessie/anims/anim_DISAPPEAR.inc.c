static const s16 nessie_anim_DISAPPEAR_values[] = {
	0x0000, 0x0000, 0x0002, 0x0007, 0x000F, 0x0019, 0x0025, 0x0032, 0x003F, 
	0x004C, 0x0059, 0x0065, 0x006F, 0x0077, 0x007C, 0x007E, 0x007C, 0x0077, 
	0x006E, 0x0063, 0x0055, 0x0044, 0x0031, 0x001B, 0x0004, 0xFFEC, 0xFFD1, 
	0xFFB6, 0xFF99, 0xFF7C, 0xFF5E, 0xFF40, 0xFF22, 0xFF04, 0xFEE6, 0xFEC9, 
	0xFEAD, 0xFE91, 0xFE77, 0xFE5E, 0xFE47, 0xFE32, 0xFE1F, 0xFE0E, 0xFE00, 
	0xFDF4, 0xFDEC, 0xFDE6, 0xFDE5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0xFFCA, 0xFF30, 0xFE3C, 0xFCF7, 0xFB6A, 0xF99D, 0xF79B, 0xF56F, 0xF323, 
	0xF0C3, 0xEE5B, 0xEBF8, 0xE9A6, 0xE771, 0xE567, 0xE391, 0xE1FD, 0xE0B5, 
	0xDFA0, 0xDEA0, 0xDDB3, 0xDCD9, 0xDC10, 0xDB58, 0xDAB0, 0xDA17, 0xD98C, 
	0xD90F, 0xD89F, 0xD83B, 0xD7E2, 0xD794, 0xD74F, 0xD714, 0xD6E2, 0xD6B7, 
	0xD693, 0xD676, 0xD65F, 0xD64D, 0xD63F, 0xD635, 0xD62F, 0xD62B, 0xD629, 
	0xD628, 0x0000, 0x0000, 0x0000, 0x00DD, 0x0327, 0x0669, 0x0A27, 0x0DE6, 
	0x1127, 0x1372, 0x144F, 0x144F, 0x144F, 0x144F, 0x144F, 0x144F, 0x144F, 
	0x144F, 0x144F, 0x144F, 0x144F, 0x144F, 0x144F, 0x1328, 0x100A, 0x0B76, 
	0x05F8, 0x0029, 0xFAA9, 0xF615, 0xF2F7, 0xF1D0, 0x0000, 0x0000, 
};

static const u16 nessie_anim_DISAPPEAR_indices[] = {
	0x0001, 0x0000, 0x0030, 0x0001, 0x0001, 0x0031, 0x0001, 0x0032, 0x0001, 
	0x0033, 0x0001, 0x0034, 0x002F, 0x0035, 0x0001, 0x0064, 0x0001, 0x0065, 
	0x001E, 0x0066, 0x0001, 0x0084, 0x0001, 0x0085, 
};

static const struct Animation nessie_anim_DISAPPEAR = {
	1,
	0,
	0,
	1,
	47,
	ANIMINDEX_NUMPARTS(nessie_anim_DISAPPEAR_indices),
	nessie_anim_DISAPPEAR_values,
	nessie_anim_DISAPPEAR_indices,
	0,
};
