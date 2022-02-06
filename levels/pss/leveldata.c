#include <ultra64.h>
#include "sm64.h"
#include "surface_terrains.h"
#include "moving_texture_macros.h"
#include "level_misc_macros.h"
#include "macro_preset_names.h"
#include "special_preset_names.h"
#include "textures.h"
#include "dialog_ids.h"

#include "make_const_nonconst.h"
#include "levels/pss/areas/1/1/model.inc.c"
#include "levels/pss/areas/1/2/model.inc.c"
#include "levels/pss/areas/1/3/model.inc.c"
#include "levels/pss/areas/1/4/model.inc.c"
#include "levels/pss/areas/1/5/model.inc.c"
#include "levels/pss/areas/1/6/model.inc.c"
#include "levels/pss/areas/1/7/model.inc.c"
#include "levels/pss/areas/1/collision.inc.c"
#include "levels/pss/areas/1/macro.inc.c"
#include "levels/pss/leveldata.inc.c"

#include "levels/pss/icecube/model.inc.c"
#include "levels/pss/icecube/collision.inc.c"
#include "levels/pss/whale/model.inc.c"
#include "levels/pss/whale/anims/data.inc.c"
#include "levels/pss/whale/anims/table.inc.c"
#include "levels/pss/whale/collision.inc.c"
#include "levels/pss/whalespout/model.inc.c"
#include "levels/pss/whalespout/collision.inc.c"
#include "levels/pss/tweesternew/model.inc.c"
#include "levels/scripts.h"
#include "level_commands.h"

#include "game/level_update.h"
const LevelScript plsdontdelete[] = {
    LOAD_MODEL_FROM_DL( MODEL_DL_WHIRLPOOL,       tweesternew_sm64_mesh_001_mesh, LAYER_TRANSPARENT),
    RETURN(),
};



const LevelScript pooly[] = { //has to be in area
	WHIRLPOOL( 0, 0, -0, -1184, 0, 80), 
    RETURN(),
};
#include "levels/pss/lungeFish/model.inc.c"
#include "levels/pss/lungeFish/anims/data.inc.c"
#include "levels/pss/lungeFish/anims/table.inc.c"
#include "levels/pss/lungeFishAttack/model.inc.c"
#include "levels/pss/end/model.inc.c"