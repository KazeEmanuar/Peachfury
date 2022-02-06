#include <ultra64.h>
#include "sm64.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "levels/intro/header.h"

#include "make_const_nonconst.h"

#define TEST_STAGE LEVEL_HMC

extern const LevelScript level_main_scripts_entry[];
const LevelScript level_script_entry[] = {
    INIT_LEVEL(),
    SLEEP(/*frames*/ 2),
    BLACKOUT(/*active*/ FALSE),
    #ifdef TEST_STAGE
    SET_REG(TEST_STAGE),
    EXECUTE(/*seg*/ 0x15, _scriptsSegmentRomStart, _scriptsSegmentRomEnd, level_main_scripts_entry),
    #else
    SET_REG(0),
    EXECUTE(/*seg*/ 0x14, /*script*/ _introSegmentRomStart, /*scriptEnd*/ _introSegmentRomEnd, /*entry*/ level_intro_splash_screen),
    #endif
    JUMP(/*target*/ level_script_entry),
};
const LevelScript level_script_entry_error_screen[] = {
    INIT_LEVEL(),
    SLEEP(/*frames*/ 2),
    BLACKOUT(/*active*/ FALSE),
    SET_REG(/*value*/ 0),
    EXECUTE(/*seg*/ 0x14, /*script*/ _introSegmentRomStart, /*scriptEnd*/ _introSegmentRomEnd, /*entry*/ level_intro_entry_error_screen),
    JUMP(/*target*/ level_script_entry_error_screen),
};