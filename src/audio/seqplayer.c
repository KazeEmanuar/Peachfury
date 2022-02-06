#include <PR/ultratypes.h>

#include "data.h"
#include "effects.h"
#include "external.h"
#include "heap.h"
#include "load.h"
#include "seqplayer.h"
#include "game/OPT_FOR_SIZE.h"

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode & 0x80)
#define PORTAMENTO_MODE(x) ((x).mode & ~0x80)
#define PORTAMENTO_MODE_1 1
#define PORTAMENTO_MODE_2 2
#define PORTAMENTO_MODE_3 3
#define PORTAMENTO_MODE_4 4
#define PORTAMENTO_MODE_5 5
