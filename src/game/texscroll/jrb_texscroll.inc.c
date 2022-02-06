#include "levels/jrb/header.h"
#include "levels/jrb/minewall/texscroll.inc.c"
#include "levels/jrb/bridgepound/texscroll.inc.c"
#include "levels/jrb/boonard/texscroll.inc.c"
#include "levels/jrb/nipperdoor/texscroll.inc.c"
#include "levels/jrb/nipperkey/texscroll.inc.c"
#include "levels/jrb/NavalPiranha/texscroll.inc.c"
#include "levels/jrb/piranhavines/texscroll.inc.c"
#include "levels/jrb/piranhavineswithint/texscroll.inc.c"
#include "levels/jrb/TNT/texscroll.inc.c"
#include "levels/jrb/highlight/texscroll.inc.c"
#include "levels/jrb/texscroll.inc.c"
void scroll_textures_jrb() {
		scroll_jrb_level_geo_minewall();

	scroll_jrb_level_geo_bridgepound();

	scroll_jrb_level_geo_boonard();

	scroll_jrb_level_geo_nipperdoor();

	scroll_jrb_level_geo_nipperkey();

	scroll_jrb_level_geo_NavalPiranha();

	scroll_jrb_level_geo_piranhavines();

	scroll_jrb_level_geo_piranhavineswithint();

	scroll_jrb_level_geo_TNT();

	scroll_jrb_level_geo_highlight();

	scroll_jrb();

}
