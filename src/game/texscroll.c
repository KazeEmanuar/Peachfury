#include "types.h"
#include "include/segment_symbols.h"
#include "memory.h"
#include "engine/math_util.h"
#include "src/engine/behavior_script.h"
#include "tile_scroll.h"
#include "texscroll.h"
#include "OPT_FOR_SIZE.h"

#define SCROLL_CONDITION(condition) condition

#include "src/game/texscroll/castle_grounds_texscroll.inc.c"
#include "src/game/texscroll/group0_texscroll.inc.c"
#include "src/game/texscroll/bob_texscroll.inc.c"
#include "src/game/texscroll/wf_texscroll.inc.c"
#include "src/game/texscroll/group13_texscroll.inc.c"
#include "src/game/texscroll/jrb_texscroll.inc.c"
#include "src/game/texscroll/common1_texscroll.inc.c"
#include "src/game/texscroll/common0_texscroll.inc.c"
#include "src/game/texscroll/group15_texscroll.inc.c"
#include "src/game/texscroll/ccm_texscroll.inc.c"
#include "src/game/texscroll/bbh_texscroll.inc.c"
#include "src/game/texscroll/bitdw_texscroll.inc.c"
#include "src/game/texscroll/hmc_texscroll.inc.c"
void scroll_textures() {
	if(SCROLL_CONDITION(sSegmentROMTable[0x7] == (uintptr_t)_hmc_segment_7SegmentRomStart)) {
		scroll_textures_hmc();
	}

}
