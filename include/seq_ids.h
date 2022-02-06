#ifndef SEQ_IDS_H
#define SEQ_IDS_H

#define SEQ_VARIATION 0x80

enum SeqId {
    SEQ_SOUND_PLAYER,                 // 0x00
    SEQ_EVENT_CUTSCENE_COLLECT_STAR,  // 0x01
    SEQ_MENU_TITLE_SCREEN,            // 0x02
    SEQ_LEVEL_GRASS,                  // 0x03
    SEQ_LEVEL_INSIDE_CASTLE,          // 0x04
    SEQ_LEVEL_WATER,                  // 0x05
    SEQ_LEVEL_GHOSTSHIP,              //0x06
    SEQ_LEVEL_BOSS_KOOPA,             // 0x07
    SEQ_LEVEL_SNOW,                   // 0x08
    SEQ_LEVEL_SLIDE,                  // 0x09
    SEQ_LEVEL_SPOOKY,                 // 0x0A
    SEQ_EVENT_PIRANHA_PLANT,          // 0x0B
    SEQ_LEVEL_UNDERGROUND,            // 0x0C
    SEQ_MENU_STAR_SELECT,             // 0x0D
    SEQ_EVENT_POWERUP,                // 0x0E
    SEQ_EVENT_METAL_CAP,              // 0x0F
    SEQ_EVENT_KOOPA_MESSAGE,          // 0x10
    SEQ_LEVEL_KOOPA_ROAD,             // 0x11
    SEQ_EVENT_HIGH_SCORE,             // 0x12
    SEQ_EVENT_MERRY_GO_ROUND,         // 0x13
    SEQ_EVENT_RACE,                   // 0x14
    SEQ_EVENT_CUTSCENE_STAR_SPAWN,    // 0x15
    SEQ_EVENT_BOSS,                   // 0x16
    SEQ_EVENT_CUTSCENE_COLLECT_KEY,   // 0x17
    SEQ_EVENT_ENDLESS_STAIRS,         // 0x18
    SEQ_LEVEL_BOSS_KOOPA_FINAL,       // 0x19
    SEQ_EVENT_CUTSCENE_CREDITS,       // 0x1A
    SEQ_EVENT_SOLVE_PUZZLE,           // 0x1B
    SEQ_EVENT_TOAD_MESSAGE,           // 0x1C
    SEQ_EVENT_PEACH_MESSAGE,          // 0x1D
    SEQ_EVENT_CUTSCENE_INTRO,         // 0x1E
    SEQ_EVENT_CUTSCENE_VICTORY,       // 0x1F
    SEQ_EVENT_CUTSCENE_ENDING,        // 0x20
    SEQ_MENU_FILE_SELECT,             // 0x21
    SEQ_EVENT_CUTSCENE_LAKITU,        // 0x22 (not in JP)
    SEQ_LEVEL_YARD_FOREST,        // 0x23
    SEQ_LEVEL_FLY_ME_TO_THE_MOON,        // 0x24
    SEQ_LEVEL_SMB3_UNDERWORLD,        // 0x25
    SEQ_LEVEL_HAPPYVILLAGE,        // 0x26
    SEQ_LEVEL_DEEPWATER,            //0x27
    SEQ_LEVEL_BUNNY,            //0x28
    SEQ_LEVEL_BUBBLEDOME,            //0x29
    SEQ_LEVEL_BOOPATHIC,            //0x2A
    SEQ_LEVEL_NIPPERBOSS,            //0x2B
    SEQ_LEVEL_W11,            //0x2C
    SEQ_LEVEL_SLIDER,            //0x2D
    SEQ_LEVEL_RAINBOWGROTTO,            //0x2E
    SEQ_LEVEL_MARIOSHOUSE,            //0x2F
    SEQ_LEVEL_INTRO,            //0x30
    SEQ_EVENT_YOSHIBEAT,            //0x31
    SEQ_EVEN_STARSELECT,            //0x32
    SEQ_LEVEL_GROTTO,            //0x33
    SEQ_LEVEL_FLYING_SHIPS,            //0x34
    SEQ_LEVEL_ICECAVE,            //0x35
    SEQ_LEVEL_SMW,              //0x36
    SEQ_BOSS_PIRANHA,              //0x37
    SEQ_LEVEL_THEMEPARK,              //0x38
    SEQ_LEVEL_JAIL,              //0x39
    SEQ_LEVEL_WARIOCASTLE,              //0x3A
    SEQ_EVENT_EELJOKE,              //0x3B
    SEQ_LEVEL_EELCLUB,              //0x3C
    SEQ_LEVEL_INSHIP,               //0x3D
    SEQ_LEVEL_OWNEW,               //0x3E
    SEQ_EVENT_SHYGUYBOSS,               //0x3F
    SEQ_EVENT_ICECAP,               //0x40
    SEQ_LEVEL_ICEBOWL,               //0x41
    SEQ_LEVEL_CARROT,               //0x42
    SEQ_LEVEL_DINODINO,               //0x43
    SEQ_COUNT
};

#endif // SEQ_IDS_H
