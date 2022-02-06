#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "geo_misc.h"
#include "levels/castle_inside/header.h"
#include "levels/hmc/header.h"
#include "levels/ttm/header.h"
#include "mario.h"
#include "memory.h"
#include "moving_texture.h"
#include "object_list_processor.h"
#include "paintings.h"
#include "save_file.h"
#include "segment2.h"
#include "OPT_FOR_SIZE.h"


/**
 * Render and update the painting whose id and group matches the values in the GraphNode's parameter.
 * Use PAINTING_ID(id, group) to set the right parameter in a level's geo layout.
 */
Gfx *geo_painting_draw(s32 callContext, struct GraphNode *node, UNUSED void *context) {
}

/**
 * Update the painting system's local copy of Mario's current floor and position.
 */
Gfx *geo_painting_update(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 c) {
}
