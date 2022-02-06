#include <ultra64.h>

#include "sm64.h"
#include "moving_texture.h"
#include "area.h"
#include "camera.h"
#include "rendering_graph_node.h"
#include "engine/math_util.h"
#include "memory.h"
#include "save_file.h"
#include "segment2.h"
#include "engine/surface_collision.h"
#include "geo_misc.h"
#include "rendering_graph_node.h"
#include "object_list_processor.h"
#include "OPT_FOR_SIZE.h"

/**
 * This file contains functions for generating display lists with moving textures
 * (abbreviated movtex). This is used for water, sand, haze, mist and treadmills.
 * Each mesh using this system has the animated vertices stored as an array of shorts.
 * The first entry is the texture movement speed. After that the vertices are stored
 * in one of two layouts: one without per-vertex color attributes and one with.
 * [speed, v0(x,y,z, s,t)      , v1(x,y,z, s,t)      , ...]
 * [speed, v0(x,y,z, r,g,b s,t), v1(x,y,z, r,g,b s,t), ...]
 * x, y, z = vertex position as integers
 * s, t = texture coordinates as 6.10 fixed point number. That means coordinates in
 * range [0, 1024] are a unique part of the image, after that it repeats the image.
 *
 * The first vertex 'v0' is special because all subsequent vertices inherit its
 * texture offset. So to animate e.g. a treadmill, the speed component arr[0] is
 * simply added to the s component arr[7] every frame and the texture scrolls
 * horizontally over the entire mesh without changing the rest of the array.
 * Note that while the system allows many kinds of vertex animations, in
 * practice the only animation used is horizontally scrolling textures.
 *
 * After updating the base mesh, the vertices are converted to the format the RSP
 * understands and a display list is generated. The RSP can buffer 16 vertices at
 * a time, and this code assumes everything fits in one buffer, so every moving
 * texture mesh must have at most 16 vertices. As a result some meshes are split
 * up into multiple parts, like the sand pathway inside the pyramid which has 3
 * parts. The water stream in the Cavern of the Metal Cap fits in one mesh.
 *
 * Apart from this general system, there is also a simpler system for flat
 * quads with a rotating texture. This is often used for water, but also
 * for mist, toxic haze and lava inside the volcano. One quad is described
 * by the struct MovtexQuad, and multiple MovtexQuads form a MovtexQuadCollection.
 * A geo node has an id that corresponds to the id of a certain MovtexQuadCollection,
 * which will then be matched with the id of entries in gEnvironmentRegions to get the
 * y-position. The x and z coordinates are stored in the MovtexQuads themself,
 * so the water rectangle is separate from the actually drawn rectangle.
 */

// First entry in array is texture movement speed for both layouts
#define MOVTEX_ATTR_SPEED 0

// Different layouts for vertices
#define MOVTEX_LAYOUT_NOCOLOR 0
#define MOVTEX_LAYOUT_COLORED 1

// Attributes for movtex vertices
#define MOVTEX_ATTR_X 1
#define MOVTEX_ATTR_Y 2
#define MOVTEX_ATTR_Z 3

// For MOVTEX_LAYOUT_NOCOLOR only
#define MOVTEX_ATTR_NOCOLOR_S 4
#define MOVTEX_ATTR_NOCOLOR_T 5

// For MOVTEX_LAYOUT_COLORED only
#define MOVTEX_ATTR_COLORED_R 4
#define MOVTEX_ATTR_COLORED_G 5
#define MOVTEX_ATTR_COLORED_B 6
#define MOVTEX_ATTR_COLORED_S 7
#define MOVTEX_ATTR_COLORED_T 8

/**
 * An object containing all info for a mesh with moving textures.
 * Contains the vertices that are animated, but also the display list which
 * determines the connectivity, as well as the texture, texture blend color
 * and drawing layer.
 */
struct MovtexObject {
    /// number that geo nodes have as parameter to refer to this mesh
    u32 geoId;
    /// which texture to use for this mesh, index into gMovtexIdToTexture
    s32 textureId;
    /// amount of moving vertices
    s32 vtx_count;
    /// segmented address to movtex mesh with vertices
    void *movtexVerts;
    /// display list inserted before moving triangles
    Gfx *beginDl;
    /// display list inserted after moving triangles
    Gfx *endDl;
    /// display list with the actual moving texture triangles.
    /// Assumes the animated vertices are buffered and correct texture is set
    Gfx *triDl;
    // if the list does not have per-vertex colors, all vertices have these colors
    u8 r;      /// red
    u8 g;      /// green
    u8 b;      /// blue
    u8 a;      /// alpha
    s32 layer; /// the drawing layer for this mesh
};

/// Counters to make textures move iff the game is not paused.
s16 gMovtexCounter = 1;
s16 gMovtexCounterPrev = 0;

// Vertex colors for rectangles. Used to give mist a tint
#define MOVTEX_VTX_COLOR_DEFAULT 0 // no tint (white vertex colors)
#define MOVTEX_VTX_COLOR_YELLOW 1  // used for Hazy Maze Cave toxic haze
#define MOVTEX_VTX_COLOR_RED 2     // used for Shifting Sand Land around the Tox box maze

s8 gMovtexVtxColor = MOVTEX_VTX_COLOR_DEFAULT;

/// The height at which Mario entered the last painting. Used for Wet-Dry World only.
float gPaintingMarioYEntry = 0.0f;


extern u8 ssl_quicksand[];
extern u8 ssl_pyramid_sand[];
extern u8 ttc_yellow_triangle[];

/**
 * An array for converting a movtex texture id to a pointer that can
 * be passed to gDPSetTextureImage.
 */
u8 *gMovtexIdToTexture[] = { texture_waterbox_water,     texture_waterbox_mist,
                             texture_waterbox_jrb_water, texture_waterbox_unknown_water,
                             texture_waterbox_lava,      ssl_quicksand,
                             ssl_pyramid_sand,           ttc_yellow_triangle };

extern Gfx castle_grounds_dl_waterfall[];
extern s16 castle_grounds_movtex_tris_waterfall[];
extern s16 ssl_movtex_tris_pyramid_sand_pathway_front[];
extern Gfx ssl_dl_pyramid_sand_pathway_begin[];
extern Gfx ssl_dl_pyramid_sand_pathway_end[];
extern Gfx ssl_dl_pyramid_sand_pathway_front_end[];
extern s16 ssl_movtex_tris_pyramid_sand_pathway_floor[];
extern Gfx ssl_dl_pyramid_sand_pathway_floor_begin[];
extern Gfx ssl_dl_pyramid_sand_pathway_floor_end[];
extern s16 ssl_movtex_tris_pyramid_sand_pathway_side[];
extern Gfx ssl_dl_pyramid_sand_pathway_side_end[];
extern s16 bitfs_movtex_tris_lava_first_section[];
extern Gfx bitfs_dl_lava_sections[];
extern s16 bitfs_movtex_tris_lava_second_section[];
extern s16 bitfs_movtex_tris_lava_floor[];
extern Gfx bitfs_dl_lava_floor[];
extern s16 lll_movtex_tris_lava_floor[];
extern Gfx lll_dl_lava_floor[];
extern s16 lll_movtex_tris_lavafall_volcano[];
extern Gfx lll_dl_lavafall_volcano[];
extern s16 cotmc_movtex_tris_water[];
extern Gfx cotmc_dl_water_begin[];
extern Gfx cotmc_dl_water_end[];
extern Gfx cotmc_dl_water[];
extern s16 ttm_movtex_tris_begin_waterfall[];
extern Gfx ttm_dl_waterfall[];
extern s16 ttm_movtex_tris_end_waterfall[];
extern s16 ttm_movtex_tris_begin_puddle_waterfall[];
extern Gfx ttm_dl_bottom_waterfall[];
extern s16 ttm_movtex_tris_end_puddle_waterfall[];
extern s16 ttm_movtex_tris_puddle_waterfall[];
extern Gfx ttm_dl_puddle_waterfall[];
extern s16 ssl_movtex_tris_pyramid_quicksand[];
extern Gfx ssl_dl_quicksand_begin[];
extern Gfx ssl_dl_quicksand_end[];
extern Gfx ssl_dl_pyramid_quicksand[];
extern s16 ssl_movtex_tris_pyramid_corners_quicksand[];
extern Gfx ssl_dl_pyramid_corners_quicksand[];
extern s16 ssl_movtex_tris_sides_quicksand[];
extern Gfx ssl_dl_sides_quicksand[];
extern s16 ttc_movtex_tris_big_surface_treadmill[];
extern Gfx ttc_dl_surface_treadmill_begin[];
extern Gfx ttc_dl_surface_treadmill_end[];
extern Gfx ttc_dl_surface_treadmill[];
extern s16 ttc_movtex_tris_small_surface_treadmill[];
extern s16 ssl_movtex_tris_quicksand_pit[];
extern Gfx ssl_dl_quicksand_pit_begin[];
extern Gfx ssl_dl_quicksand_pit_end[];
extern Gfx ssl_dl_quicksand_pit[];
extern s16 ssl_movtex_tris_pyramid_quicksand_pit[];
extern Gfx ssl_dl_pyramid_quicksand_pit_begin[];
extern Gfx ssl_dl_pyramid_quicksand_pit_end[];

/**
 * MovtexObjects that have no color attributes per vertex (though the mesh
 * as a whole can have a blend color).
 */
struct MovtexObject gMovtexNonColored[] = {
    // Inside the pyramid there is a sand pathway with the 5 secrets on it.
    // pathway_front is the highest 'sand fall', pathway_floor is the horizontal
    // sand stream and pathway_side is the lower 'sand fall'.
    { MOVTEX_PYRAMID_SAND_PATHWAY_FRONT, TEX_PYRAMID_SAND_SSL, 8,
      ssl_movtex_tris_pyramid_sand_pathway_front, ssl_dl_pyramid_sand_pathway_begin,
      ssl_dl_pyramid_sand_pathway_end, ssl_dl_pyramid_sand_pathway_front_end, 0xff, 0xff, 0xff, 0xff,
      LAYER_TRANSPARENT_INTER },
    { MOVTEX_PYRAMID_SAND_PATHWAY_FLOOR, TEX_PYRAMID_SAND_SSL, 8,
      ssl_movtex_tris_pyramid_sand_pathway_floor, ssl_dl_pyramid_sand_pathway_floor_begin,
      ssl_dl_pyramid_sand_pathway_floor_end, ssl_dl_pyramid_sand_pathway_front_end, 0xff, 0xff, 0xff,
      0xff, LAYER_OPAQUE_INTER },
    { MOVTEX_PYRAMID_SAND_PATHWAY_SIDE, TEX_PYRAMID_SAND_SSL, 6,
      ssl_movtex_tris_pyramid_sand_pathway_side, ssl_dl_pyramid_sand_pathway_begin,
      ssl_dl_pyramid_sand_pathway_end, ssl_dl_pyramid_sand_pathway_side_end, 0xff, 0xff, 0xff, 0xff,
      LAYER_TRANSPARENT_INTER },

    // The waterfall outside the castle
    { MOVTEX_CASTLE_WATERFALL, TEXTURE_WATER, 15, 0,
      dl_waterbox_rgba16_begin, dl_waterbox_end, 0, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT_INTER },

    // Bowser in the Fire Sea has lava at 3 heights, lava_floor is the lowest
    // and lava_second_section is the highest
    { MOVTEX_BITFS_LAVA_FIRST, TEXTURE_LAVA, 4, bitfs_movtex_tris_lava_first_section,
      dl_waterbox_rgba16_begin, dl_waterbox_end, bitfs_dl_lava_sections, 0xff, 0xff, 0xff, 0xff,
      LAYER_OPAQUE },
    { MOVTEX_BITFS_LAVA_SECOND, TEXTURE_LAVA, 4, bitfs_movtex_tris_lava_second_section,
      dl_waterbox_rgba16_begin, dl_waterbox_end, bitfs_dl_lava_sections, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT },
    { MOVTEX_BITFS_LAVA_FLOOR, TEXTURE_LAVA, 9, bitfs_movtex_tris_lava_floor, dl_waterbox_rgba16_begin,
      dl_waterbox_end, bitfs_dl_lava_floor, 0xff, 0xff, 0xff, 0xb4, LAYER_TRANSPARENT },

    // Lava floor in Lethal Lava Land and the lava fall in the volcano
    //! Note that the lava floor in the volcano is actually a quad.
    // The quad collection index LLL_MOVTEX_VOLCANO_FLOOR_LAVA is actually
    // 2 | MOVTEX_AREA_LLL, suggesting that the lava floor of LLL used to be a
    // quad too, with index 1.
    // It was probably too large however, resulting in overflowing texture
    // coordinates or other artifacts, so they converted it to a movtex
    // mesh with 9 vertices, subdividing the rectangle into 4 smaller ones.
    { MOVTEX_LLL_LAVA_FLOOR, TEXTURE_LAVA, 9, lll_movtex_tris_lava_floor, dl_waterbox_rgba16_begin,
      dl_waterbox_end, lll_dl_lava_floor, 0xff, 0xff, 0xff, 0xc8, LAYER_TRANSPARENT },
    { MOVTEX_VOLCANO_LAVA_FALL, TEXTURE_LAVA, 16, lll_movtex_tris_lavafall_volcano,
      dl_waterbox_rgba16_begin, dl_waterbox_end, lll_dl_lavafall_volcano, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT_INTER },

    // Cavern of the metal Cap has a waterfall source above the switch platform,
    // the stream, around the switch, and the waterfall that's the same as the one
    // outside the castle. They are all part of the same mesh.
    { MOVTEX_COTMC_WATER, TEXTURE_WATER, 14, cotmc_movtex_tris_water, cotmc_dl_water_begin,
      cotmc_dl_water_end, cotmc_dl_water, 0xff, 0xff, 0xff, 0xb4, LAYER_TRANSPARENT_INTER },

    // Tall Tall mountain has water going from the top to the bottom of the mountain.
    { MOVTEX_TTM_BEGIN_WATERFALL, TEXTURE_WATER, 6, ttm_movtex_tris_begin_waterfall,
      dl_waterbox_rgba16_begin, dl_waterbox_end, ttm_dl_waterfall, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT },
    { MOVTEX_TTM_END_WATERFALL, TEXTURE_WATER, 6, ttm_movtex_tris_end_waterfall,
      dl_waterbox_rgba16_begin, dl_waterbox_end, ttm_dl_waterfall, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT },
    { MOVTEX_TTM_BEGIN_PUDDLE_WATERFALL, TEXTURE_WATER, 4, ttm_movtex_tris_begin_puddle_waterfall,
      dl_waterbox_rgba16_begin, dl_waterbox_end, ttm_dl_bottom_waterfall, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT_INTER },
    { MOVTEX_TTM_END_PUDDLE_WATERFALL, TEXTURE_WATER, 4, ttm_movtex_tris_end_puddle_waterfall,
      dl_waterbox_rgba16_begin, dl_waterbox_end, ttm_dl_bottom_waterfall, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT_INTER },
    { MOVTEX_TTM_PUDDLE_WATERFALL, TEXTURE_WATER, 8, ttm_movtex_tris_puddle_waterfall,
      dl_waterbox_rgba16_begin, dl_waterbox_end, ttm_dl_puddle_waterfall, 0xff, 0xff, 0xff, 0xb4,
      LAYER_TRANSPARENT_INTER },
    { 0x00000000, 0x00000000, 0, NULL, NULL, NULL, NULL, 0x00, 0x00, 0x00, 0x00, 0x00000000 },
};

/**
 * MovtexObjects that have color attributes per vertex.
 */
struct MovtexObject gMovtexColored[] = {
    { MOVTEX_SSL_PYRAMID_SIDE, TEX_QUICKSAND_SSL, 12, ssl_movtex_tris_pyramid_quicksand,
      ssl_dl_quicksand_begin, ssl_dl_quicksand_end, ssl_dl_pyramid_quicksand, 0xff, 0xff, 0xff, 0xff,
      LAYER_OPAQUE },
    { MOVTEX_SSL_PYRAMID_CORNER, TEX_QUICKSAND_SSL, 16, ssl_movtex_tris_pyramid_corners_quicksand,
      ssl_dl_quicksand_begin, ssl_dl_quicksand_end, ssl_dl_pyramid_corners_quicksand, 0xff, 0xff, 0xff,
      0xff, LAYER_OPAQUE },
    { MOVTEX_SSL_COURSE_EDGE, TEX_QUICKSAND_SSL, 15, ssl_movtex_tris_sides_quicksand,
      ssl_dl_quicksand_begin, ssl_dl_quicksand_end, ssl_dl_sides_quicksand, 0xff, 0xff, 0xff, 0xff,
      LAYER_OPAQUE },
    { MOVTEX_TREADMILL_BIG, TEX_YELLOW_TRI_TTC, 12, ttc_movtex_tris_big_surface_treadmill,
      ttc_dl_surface_treadmill_begin, ttc_dl_surface_treadmill_end, ttc_dl_surface_treadmill, 0xff,
      0xff, 0xff, 0xff, LAYER_OPAQUE },
    { MOVTEX_TREADMILL_SMALL, TEX_YELLOW_TRI_TTC, 12, ttc_movtex_tris_small_surface_treadmill,
      ttc_dl_surface_treadmill_begin, ttc_dl_surface_treadmill_end, ttc_dl_surface_treadmill, 0xff,
      0xff, 0xff, 0xff, LAYER_OPAQUE },
    { 0x00000000, 0x00000000, 0, NULL, NULL, NULL, NULL, 0x00, 0x00, 0x00, 0x00, 0x00000000 },
};

/**
 * Treated identically to gMovtexColored.
 */
struct MovtexObject gMovtexColored2[] = {
    { MOVTEX_SSL_SAND_PIT_OUTSIDE, TEX_QUICKSAND_SSL, 8, ssl_movtex_tris_quicksand_pit,
      ssl_dl_quicksand_pit_begin, ssl_dl_quicksand_pit_end, ssl_dl_quicksand_pit, 0xff, 0xff, 0xff,
      0xff, LAYER_OPAQUE },
    { MOVTEX_SSL_SAND_PIT_PYRAMID, TEX_PYRAMID_SAND_SSL, 8, ssl_movtex_tris_pyramid_quicksand_pit,
      ssl_dl_pyramid_quicksand_pit_begin, ssl_dl_pyramid_quicksand_pit_end, ssl_dl_quicksand_pit, 0xff,
      0xff, 0xff, 0xff, LAYER_OPAQUE },
    { 0x00000000, 0x00000000, 0, NULL, NULL, NULL, NULL, 0x00, 0x00, 0x00, 0x00, 0x00000000 },
};

/**
 * Update moving texture counters that determine when to update the coordinates.
 * Textures update when gMovtexCounterPrev != gMovtexCounter.
 * This ensures water / sand flow stops when the game pauses.
 */
Gfx *geo_movtex_pause_control(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx) {
    return NULL;
}

/**
 * Represents a single flat quad with a rotating texture
 * Stores x and z for 4 vertices, though it is often just a rectangle.
 * Does not store the y-position, since that can be dynamic for water levels.
 */
struct MovtexQuad {
    /// the current texture rotation in this quad
    s16 rot;
    /// gets added to rot every frame
    s16 rotspeed;
    /// the amount of times the texture repeats. 1 = no repeat.
    s16 scale;
    /// Coordinates of vertices
    s16 x1;
    s16 z1;
    s16 x2;
    s16 z2;
    s16 x3;
    s16 z3;
    s16 x4;
    s16 z4;
    s16 rotDir;    /// if 1, it rotates counter-clockwise
    s16 alpha;     /// opacity, 255 = fully opaque
    s16 textureId; /// texture id
};

/// Variable for a little optimization: only set the texture when it differs from the previous texture
s16 gMovetexLastTextureId;
/**
 * Geo script responsible for drawing quads with a moving texture at the height
 * of the corresponding water region. The node's parameter determines which quad
 * collection is drawn, see moving_texture.h.
 */
Gfx *geo_movtex_draw_water_regions(s32 callContext, struct GraphNode *node, UNUSED Mat4 mtx) {
    return 0;
}