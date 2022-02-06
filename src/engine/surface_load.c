#include <PR/ultratypes.h>

#include "prevent_bss_reordering.h"

#include "sm64.h"
#include "game/ingame_menu.h"
#include "graph_node.h"
#include "behavior_script.h"
#include "behavior_data.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/macro_special_objects.h"
#include "surface_collision.h"
#include "game/mario.h"
#include "game/object_list_processor.h"
#include "surface_load.h"
#include "engine/math_util.h"
#define EPSILON 0.0001f
#define VEC3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define VEC3_COPY(out, a)                                                                              \
    {                                                                                                  \
        (out)[0] = (a)[0];                                                                             \
        (out)[1] = (a)[1];                                                                             \
        (out)[2] = (a)[2];                                                                             \
    }
#define VEC3_SCALE(out, a, scale)                                                                      \
    {                                                                                                  \
        (out)[0] = (a)[0] * (scale);                                                                   \
        (out)[1] = (a)[1] * (scale);                                                                   \
        (out)[2] = (a)[2] * (scale);                                                                   \
    }
#define VEC3_ADD(out, a, b)                                                                            \
    {                                                                                                  \
        (out)[0] = (a)[0] + (b)[0];                                                                    \
        (out)[1] = (a)[1] + (b)[1];                                                                    \
        (out)[2] = (a)[2] + (b)[2];                                                                    \
    }
#define VEC3_DIFF(out, a, b)                                                                           \
    {                                                                                                  \
        (out)[0] = (a)[0] - (b)[0];                                                                    \
        (out)[1] = (a)[1] - (b)[1];                                                                    \
        (out)[2] = (a)[2] - (b)[2];                                                                    \
    }
#define ABSI(x) ((x) > 0 ? (x) : -(x))
#define NORMAL_SCALE 1024

/**
 * Partitions for course and object surfaces. The arrays represent
 * the 16x16 cells that each level is split into.
 */
SpatialPartitionCell gStaticSurfacePartition[CELLCOUNT][CELLCOUNT];
SpatialPartitionCell gDynamicSurfacePartition[CELLCOUNT][CELLCOUNT];

/**
 * Pools of data to contain either surface nodes or surfaces.
 */
struct SurfaceNode *sSurfaceNodePool;
struct Surface *sSurfacePool;

/**
 * The size of the surface pool (2300).
 */
s16 sSurfacePoolSize;

/**
 * Allocate the part of the surface node pool to contain a surface node.
 */
inline struct SurfaceNode *alloc_surface_node(void) {
    struct SurfaceNode *node = &sSurfaceNodePool[gSurfaceNodesAllocated];
    gSurfaceNodesAllocated++;

    node->next = NULL;

    return node;
}
/**
 * Iterates through the entire partition, clearing the surfaces.
 */
static void clear_spatial_partition(SpatialPartitionCell *cells) {
    register s32 i = CELLCOUNT * CELLCOUNT;

    while (i--) {
        (*cells)[SPATIAL_PARTITION_FLOORS].next = NULL;
        (*cells)[SPATIAL_PARTITION_CEILS].next = NULL;
        (*cells)[SPATIAL_PARTITION_WALLS].next = NULL;

        cells++;
    }
}

/**
 * Clears the (level) surface partitions for new use.
 */
inline void clear_static_surfaces(void) {
    clear_spatial_partition(&gStaticSurfacePartition[0][0]);
}

/**
 * Add a surface to the correct cell list of surfaces.
 * @param dynamic Determines whether the surface is or dynamic
 * @param cellX The X position of the cell in which the surface resides
 * @param cellZ The Z position of the cell in which the surface resides
 * @param surface The surface to add
 */
static void add_surface_to_cell(s16 dynamic, s16 cellX, s16 cellZ, struct Surface *surface) {
    struct SurfaceNode *newNode = alloc_surface_node();
    struct SurfaceNode *list;
    s16 surfacePriority;
    s16 priority;
    s16 sortDir;
    s16 listIndex;

    if (surface->normal.y > WALLMAXNORMAL) {
        listIndex = SPATIAL_PARTITION_FLOORS;
        sortDir = 1; // highest to lowest, then insertion order
    } else if (surface->normal.y < -WALLMAXNORMAL) {
        listIndex = SPATIAL_PARTITION_CEILS;
        sortDir = -1; // lowest to highest, then insertion order
    } else {
        listIndex = SPATIAL_PARTITION_WALLS;
        sortDir = 0; // insertion order
    }

    //! (Surface Cucking) Surfaces are sorted by the height of their first
    //  vertex. Since vertices aren't ordered by height, this causes many
    //  lower triangles to be sorted higher. This worsens surface cucking since
    //  many functions only use the first triangle in surface order that fits,
    //  missing higher surfaces.
    //  upperY would be a better sort method.
    surfacePriority = surface->vertex1[1] * sortDir;

    newNode->surface = surface;

    if (dynamic) {
        list = &gDynamicSurfacePartition[cellZ][cellX][listIndex];
    } else {
        list = &gStaticSurfacePartition[cellZ][cellX][listIndex];
    }

    // Loop until we find the appropriate place for the surface in the list.
    while (list->next != NULL) {
        priority = list->next->surface->vertex1[1] * sortDir;

        if (surfacePriority > priority) {
            break;
        }

        list = list->next;
    }

    newNode->next = list->next;
    list->next = newNode;
}

/**
 * Returns the lowest of three values.
 */
inline s16 min_3(s16 a0, s16 a1, s16 a2) {
    if (a1 < a0) {
        a0 = a1;
    } 
    if (a2 < a0) {
        a0 = a2;
    }

    return a0;
}

/**
 * Returns the highest of three values.
 */
inline s16 max_3(s16 a0, s16 a1, s16 a2) {
    if (a1 > a0) {
        a0 = a1;
    } 
    if (a2 > a0) {
        a0 = a2;
    }

    return a0;
}

/**
 * Every level is split into 16 * 16 cells of surfaces (to limit computing
 * time). This function determines the lower cell for a given x/z position.
 * @param coord The coordinate to test
 */
static s16 lower_cell_index(s32 coord) {
    s16 index;

    // Move from range [-0x2000, 0x2000) to [0, 0x4000)
    coord += LEVEL_BOUNDARY_MAX;
 /*   if (coord < 0) {
        coord = 0;
    }*/

    // [0, 16)
    index = coord / CELL_SIZE;

    // Include extra cell if close to boundary
    //! Some wall checks are larger than the buffer, meaning wall checks can
    //  miss walls that are near a cell border.
 /*   if (coord % CELL_SIZE < 50) {
        index -= 1;
    }

    if (index < 0) {
        index = 0;
    }*/

    // Potentially > 15, but since the upper index is <= 15, not exploitable
    return index;
}

/**
 * Every level is split into 16 * 16 cells of surfaces (to limit computing
 * time). This function determines the upper cell for a given x/z position.
 * @param coord The coordinate to test
 */
static s16 upper_cell_index(s32 coord) {
    s16 index;

    // Move from range [-0x2000, 0x2000) to [0, 0x4000)
    coord += LEVEL_BOUNDARY_MAX;
 /*   if (coord < 0) {
        coord = 0;
    }*/

    // [0, 16)
    index = coord / CELL_SIZE;

    // Include extra cell if close to boundary
    //! Some wall checks are larger than the buffer, meaning wall checks can
    //  miss walls that are near a cell border.
 /*   if (coord % CELL_SIZE > CELL_SIZE - 50) {
        index += 1;
    }

    if (index > (CELLCOUNT - 1)) {
        index = (CELLCOUNT - 1);
    }*/

    // Potentially < 0, but since lower index is >= 0, not exploitable
    return index;
}

/**
 * Every level is split into 16x16 cells, this takes a surface, finds
 * the appropriate cells (with a buffer), and adds the surface to those
 * cells.
 * @param surface The surface to check
 * @param dynamic Boolean determining whether the surface is or dynamic
 */

inline void add_surface(struct Surface *surface, s32 dynamic) {
    s16 minX, minZ, maxX, maxZ;

    s16 minCellX, minCellZ, maxCellX, maxCellZ;

    s16 cellZ, cellX;

    minX = min_3(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0]);
    minZ = min_3(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2]);
    maxX = max_3(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0]);
    maxZ = max_3(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2]);

    minCellX = lower_cell_index(minX);
    maxCellX = upper_cell_index(maxX);
    minCellZ = lower_cell_index(minZ);
    maxCellZ = upper_cell_index(maxZ);

    for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
        for (cellX = minCellX; cellX <= maxCellX; cellX++) {
            add_surface_to_cell(dynamic, cellX, cellZ, surface);
        }
    }
}
/**
 * Initializes a Surface struct using the given vertex data
 * @param vertexData The raw data containing vertex positions
 * @param vertexIndices Helper which tells positions in vertexData to start reading vertices
 */

static f32 getBarry(f32 *one, f32 *two) {
    register f32 mag = 0;
    register s32 x1;
    for (x1 = 0; x1 < 3; x1++) {
        mag += one[x1] * two[x1];
    }
    return mag;
}
static struct Surface *read_surface_data(s16 *vertexData, s16 **vertexIndices) {
    struct Surface *surface;
    register s32 x1, y1, z1;
    register s32 x2, y2, z2;
    register s32 x3, y3, z3;
    f32 nx, ny, nz;
    f32 mag;
    s32 offset1, offset2, offset3;

    offset1 = 3 * (*vertexIndices)[0];
    offset2 = 3 * (*vertexIndices)[1];
    offset3 = 3 * (*vertexIndices)[2];
    x1 = (vertexData + offset1)[0];
    y1 = (vertexData + offset1)[1];
    z1 = (vertexData + offset1)[2];
    x2 = (vertexData + offset2)[0];
    y2 = (vertexData + offset2)[1];
    z2 = (vertexData + offset2)[2];
    x3 = (vertexData + offset3)[0];
    y3 = (vertexData + offset3)[1];
    z3 = (vertexData + offset3)[2];

    surface = &sSurfacePool[gSurfacesAllocated];
    gSurfacesAllocated++;
    surface->object = NULL;
    surface->flags = 0;

    surface->vertex1[0] = x1;
    surface->vertex2[0] = x2;
    surface->vertex3[0] = x3;

    surface->vertex1[1] = y1;
    surface->vertex2[1] = y2;
    surface->vertex3[1] = y3;

    surface->vertex1[2] = z1;
    surface->vertex2[2] = z2;
    surface->vertex3[2] = z3;

    // (v2 - v1) x (v3 - v2)
    nx = (y2 - y1) * (z3 - z2) - (z2 - z1) * (y3 - y2);
    ny = (z2 - z1) * (x3 - x2) - (x2 - x1) * (z3 - z2);
    nz = (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);
    mag = sqrtf(nx * nx + ny * ny + nz * nz);

    // Checking to make sure no DIV/0
    /*if (mag < 0.0001) { DANGER
        return NULL;
    }*/
    mag = (f32) (1.0 / mag);
    nx *= mag;
    ny *= mag;
    nz *= mag;

    surface->normal.x = nx;
    surface->normal.y = ny;
    surface->normal.z = nz;

    surface->originOffset = -(nx * x1 + ny * y1 + nz * z1);

    surface->lowerY = min_3(y1, y2, y3) - 5;
    surface->upperY = max_3(y1, y2, y3) + 5;

    for (x1 = 0; x1 < 3; x1++) {
        surface->Baryu[x1] = surface->vertex2[x1] - surface->vertex1[x1];
        surface->Baryv[x1] = surface->vertex3[x1] - surface->vertex1[x1];
    }
    surface->BaryProducts[0] = getBarry(surface->Baryu, surface->Baryu);
    surface->BaryProducts[1] = getBarry(surface->Baryu, surface->Baryv);
    surface->BaryProducts[2] = getBarry(surface->Baryv, surface->Baryv);

    surface->BaryProducts[3] = 1.f
                               / (surface->BaryProducts[1] * surface->BaryProducts[1]
                                  - surface->BaryProducts[0] * surface->BaryProducts[2]);
    return surface;
}

/**
 * Returns whether a surface has exertion/moves Mario
 * based on the surface type.
 */
static s32 surface_has_force(s32 surfaceType) {
    s32 hasForce = FALSE;

    switch (surfaceType & 0xFF) {
        case SURFACE_FLOWING_WATER:
        case SURFACE_DEEP_MOVING_QUICKSAND:
        case SURFACE_SHALLOW_MOVING_QUICKSAND:
        case SURFACE_MOVING_QUICKSAND:
        case SURFACE_HORIZONTAL_WIND:
        case SURFACE_INSTANT_MOVING_QUICKSAND:
            hasForce = TRUE;
            break;

        default:
            break;
    }
    return hasForce;
}

/**
 * Load in the surfaces for a given surface type. This includes setting the flags,
 * exertion, and room.
 */
static void load_static_surfaces(s16 **data, s16 *vertexData, s32 surfaceType, s8 **surfaceRooms) {
    s32 i;
    s32 numSurfaces;
    struct Surface *surface;
    s8 room = 0;
    s16 hasForce = surface_has_force(surfaceType);
    s16 flags = 0;

    numSurfaces = *(*data);
    *data += 1;

    for (i = 0; i < numSurfaces; i++) {
        if (*surfaceRooms != NULL) {
            room = *(*surfaceRooms);
            *surfaceRooms += 1;
        }

        surface = read_surface_data(vertexData, data);
        if (surface != NULL) {
            surface->room = room;
            surface->type = surfaceType;
            surface->flags = (s8) flags;

            if (hasForce) {
                surface->force = *(*data + 3);
            } else {
                surface->force = 0;
            }

            add_surface(surface, FALSE);
        }

        *data += 3;
        if (hasForce) {
            *data += 1;
        }
    }
}

/**
 * Read the data for vertices for reference by triangles.
 */
static s16 *read_vertex_data(s16 **data) {
    s32 numVertices;
    UNUSED s16 unused1[3];
    UNUSED s16 unused2[3];
    s16 *vertexData;

    numVertices = *(*data);
    (*data)++;

    vertexData = *data;
    *data += 3 * numVertices;

    return vertexData;
}

/**
 * Loads in special environmental regions, such as water, poison gas, and JRB fog.
 */
static void load_environmental_regions(s16 **data) {
    s32 numRegions;
    s32 i;

    gEnvironmentRegions = *data;
    numRegions = *(*data)++;

    if (numRegions > 20) {
    }

    for (i = 0; i < numRegions; i++) {
        UNUSED s16 val, loX, loZ, hiX, hiZ;
        s16 height;

        val = *(*data)++;

        loX = *(*data)++;
        hiX = *(*data)++;
        loZ = *(*data)++;
        hiZ = *(*data)++;

        height = *(*data)++;

        gEnvironmentLevels[i] = height;
    }
}

/**
 * Allocate some of the main pool for surfaces (2300 surf) and for surface nodes (7000 nodes).
 */
void alloc_surface_pools(void) {
    sSurfacePoolSize = 4000;
    sSurfaceNodePool = main_pool_alloc(21000 * sizeof(struct SurfaceNode), MEMORY_POOL_LEFT);
    sSurfacePool = main_pool_alloc(sSurfacePoolSize * sizeof(struct Surface), MEMORY_POOL_LEFT);

    gRedCoinsCollected = 0;
}
#define getTerrainType(pointer, store)                                                                 \
    store = (((u16 *) pointer)[0] << 16) | (((u16 *) pointer)[1]);                                     \
    pointer += 2;
/**
 * Process the level file, loading in vertices, surfaces, some objects, and environmental
 * boxes (water, gas, JRB fog).
 */
void load_area_terrain(s16 index, s16 *data, s8 *surfaceRooms, s16 *macroObjects) {
    s32 terrainLoadType;
    s16 *vertexData;

    gEnvironmentRegions = NULL;
    gSurfaceNodesAllocated = 0;
    gSurfacesAllocated = 0;

    clear_static_surfaces();

    while (TRUE) {
        getTerrainType(data, terrainLoadType)

            if (TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(terrainLoadType)) {
            load_static_surfaces(&data, vertexData, terrainLoadType, &surfaceRooms);
        }
        else if (terrainLoadType == 0x40) {
            vertexData = read_vertex_data(&data);
        }
        else if (terrainLoadType == 0x43) {
            spawn_special_objects(index, &data);
        }
        else if (terrainLoadType == 0x44) {
            load_environmental_regions(&data);
        }
        else if (terrainLoadType == 0x41) {
            continue;
        }
        else if (terrainLoadType == 0x42) {
            break;
        }
        else if (TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(terrainLoadType)) {
            load_static_surfaces(&data, vertexData, terrainLoadType, &surfaceRooms);
            continue;
        }
    }

    gNumStaticSurfaceNodes = gSurfaceNodesAllocated;
    gNumStaticSurfaces = gSurfacesAllocated;
}

/**
 * If not in time stop, clear the surface partitions.
 */
void clear_dynamic_surfaces(void) {
    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        gSurfacesAllocated = gNumStaticSurfaces;
        gSurfaceNodesAllocated = gNumStaticSurfaceNodes;

        clear_spatial_partition(&gDynamicSurfacePartition[0][0]);
    }
}
/**
 * Applies an object's transformation to the object's vertices.
 */
inline void transform_object_vertices(s16 **data, s16 *vertexData) {
    register s16 *vertices;
    register f32 vx, vy, vz;
    register s32 numVertices;

    Mat4 *objectTransform;
    Mat4 m;

    objectTransform = &gCurrentObject->transform;

    numVertices = *(*data);
    (*data)++;

    vertices = *data;

    obj_build_transform_from_pos_and_angle(gCurrentObject, O_POS_INDEX, O_FACE_ANGLE_INDEX);

    // obj_apply_scale_to_matrix(gCurrentObject, m, *objectTransform);
    mtxf_scale_vec3f(m, *objectTransform, &gCurrentObject->header.gfx.scale[0]);
    // Go through all vertices, rotating and translating them to transform the object.
    while (numVertices--) {
        vx = *(vertices++);
        vy = *(vertices++);
        vz = *(vertices++);

        //! No bounds check on vertex data
        *vertexData++ = (s16) (vx * m[0][0] + vy * m[1][0] + vz * m[2][0] + m[3][0]);
        *vertexData++ = (s16) (vx * m[0][1] + vy * m[1][1] + vz * m[2][1] + m[3][1]);
        *vertexData++ = (s16) (vx * m[0][2] + vy * m[1][2] + vz * m[2][2] + m[3][2]);
    }

    *data = vertices;
}

/**
 * Load in the surfaces for the gCurrentObject. This includes setting the flags, exertion, and room.
 */
inline void load_object_surfaces(s16 **data, s16 *vertexData) {
    s32 surfaceType;
    s32 i;
    s32 numSurfaces;
    s16 hasForce;
    s16 flags;
    s16 room;

    getTerrainType((*data), surfaceType);

    numSurfaces = *(*data);
    (*data)++;

    hasForce = surface_has_force(surfaceType);

    flags |= SURFACE_FLAG_DYNAMIC;

    for (i = 0; i < numSurfaces; i++) {
        struct Surface *surface = read_surface_data(vertexData, data);

        surface->object = gCurrentObject;
        surface->type = surfaceType;

        if (hasForce) {
            surface->force = *(*data + 3);
        } else {
            surface->force = 0;
        }

        surface->flags |= flags;
        surface->room = 0;
        add_surface(surface, TRUE);

        if (hasForce) {
            *data += 4;
        } else {
            *data += 3;
        }
    }
}
#define o gCurrentObject
inline void get_optimal_coll_dist(struct Object *this) {
    f32 maxDist = 0.f;
    f32 x, y, z;
    f32 thisVertDist;
    u32 vertsLeft;
    s16 *collisionData = gCurrentObject->collisionData;
    this->oFlags |= OBJ_FLAG_DONT_CALC_COLL_DIST;
    collisionData += 2;
    vertsLeft = *(collisionData);
    collisionData++;

    // vertices = *data;
    while (vertsLeft) {
        x = *(collisionData);
        y = *(collisionData + 1);
        z = *(collisionData + 2);
        x = o->header.gfx.scale[0] * x;
        y = o->header.gfx.scale[1] * y;
        z = o->header.gfx.scale[2] * z;

        thisVertDist = (x * x + y * y + z * z);
        if (thisVertDist > maxDist) {
            maxDist = thisVertDist;
        }
        collisionData += 3;
        vertsLeft--;
    }

    this->oCollisionDistance = sqrtf(maxDist) + 100.f;
}
/**
 * Transform an object's vertices, reload them, and render the object.
 */
extern struct CutsceneJump *currentScene;
void load_object_collision_model(void) {
    s16 vertexData[600];

    s16 *collisionData = gCurrentObject->collisionData;
    f32 marioDist = gCurrentObject->oDistanceToMario;
    f32 tangibleDist;
    if (!(gCurrentObject->oFlags & OBJ_FLAG_DONT_CALC_COLL_DIST)) {
        get_optimal_coll_dist(gCurrentObject);
    }
    tangibleDist = gCurrentObject->oCollisionDistance;

    // On an object's first frame, the distance is set to 19000.0f.
    // If the distance hasn't been updated, update it now.
    if (!(gCurrentObject->oFlags & OBJ_FLAG_COMPUTE_DIST_TO_MARIO)) {
        marioDist = dist_between_objects(gCurrentObject, gMarioObject);
    }

    // Update if no Time Stop, in range, and in the current room.
    if (!(gTimeStopState & TIME_STOP_ACTIVE) && (marioDist < tangibleDist)
        && !(gCurrentObject->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        collisionData += 2;
        transform_object_vertices(&collisionData, vertexData);

        // TERRAIN_LOAD_CONTINUE acts as an "end" to the terrain data.
        while (collisionData[1] != 0x41) {
            load_object_surfaces(&collisionData, vertexData);
        }
    }
}

/**
 * Raycast functions
 */
void vec3f_dif(Vec3f a, Vec3f b, Vec3f c) {
    a[0] = b[0] - c[0];
    a[1] = b[1] - c[1];
    a[2] = b[2] - c[2];
}

void vec3f_mul(Vec3f add_dir, f32 length) {
    add_dir[0] *= length;
    add_dir[1] *= length;
    add_dir[2] *= length;
}

inline f32 ray_surf_intersect(Vec3f rayStart, Vec3f rayDir, f32 rayDist, Vec3f intersectionOut,
                              struct Surface *surf) {
    f32 denom;
    f32 distOnRay;
    f32 w[3];   // Intersection point - vertex1
    f32 wu, wv; // Dot products
    f32 s;      // Barycentric coordinates
    s32 i;
    // Intersect the ray with the plane that the surface lies in
    // a = -(originOffset + normal dot rayStart) / (normal dot rayDir)

    denom = vec3f_dot(&surf->normal.x, rayDir);
    // Prevent division by zero and throw out collision with backfaces (negative denominator)
    /*if (denom != 0 && denom > -EPSILON) {
        return 0;
    }*/
    if (denom > -EPSILON) {
        return 0;
    }
    distOnRay = -((surf->originOffset) + vec3f_dot(&surf->normal.x, rayStart)) / denom;
    if (distOnRay > rayDist) {
        return 0;
    }
    if (distOnRay < 0) {
        return 0;
    }

    for (i = 0; i < 3; i++) {
        intersectionOut[i] = rayDir[i] * distOnRay + rayStart[i];
        w[i] = intersectionOut[i] - surf->vertex1[i];
    }

    wu = vec3f_dot(w, surf->Baryu);
    w[0] = vec3f_dot(w, surf->Baryv);

    w[1] = (surf->BaryProducts[1] * w[0] - surf->BaryProducts[2] * wu) * surf->BaryProducts[3];
    if (w[1] < -EPSILON || w[1] > (1 + EPSILON)) {
        return 0;
    }
    wu = (surf->BaryProducts[1] * wu - surf->BaryProducts[0] * w[0]) * surf->BaryProducts[3];
    if (wu < -EPSILON || (w[1] + wu) > (1 + EPSILON)) {
        return 0;
    }
    return distOnRay;
}
#include "game/level_update.h"

// SURFACE_PAINTING_WOBBLE_AD
static void find_surface_on_ray_list(struct SurfaceNode *list, Vec3f orig, Vec3f dir, f32 dir_length,
                                     struct Surface **hit_surface, Vec3f hit_pos, f32 *max_length) {
    s32 hit;
    f32 length;
    Vec3f chk_hit_pos;
    f32 top, bottom;

    // Get upper and lower bounds of ray
    if (dir[1] >= 0.0f) {
        top = orig[1] + dir[1] * dir_length;
        bottom = orig[1];
    } else {
        top = orig[1];
        bottom = orig[1] + dir[1] * dir_length;
    }

    // Iterate through every surface of the list
    for (; list != NULL; list = list->next) {
        // Reject surface if out of vertical bounds
        if (list->surface->lowerY > top || list->surface->upperY < bottom)
            continue;

        // Check intersection between the ray and this surface/*
        /*if ((hit = ray_surface_intersect(orig, dir, dir_length, list->surface, chk_hit_pos, &length))
            != 0) {
            if (length <= *max_length) {
                *hit_surface = list->surface;
                vec3f_copy(hit_pos, chk_hit_pos);
                *max_length = length;
            }
        }*/
        if (((SURFACETYPE(list->surface) != 0x00AD) || (gMarioState->flags & MARIO_VANISH_CAP))) {
            if ((length = ray_surf_intersect(orig, dir, dir_length, chk_hit_pos, list->surface) > 0)) {
                if (length <= *max_length) {
                    *hit_surface = list->surface;
                    vec3f_copy(hit_pos, chk_hit_pos);
                    *max_length = length;
                }
            }
        }
    }
}

static void find_surface_on_ray_cell(s16 cellX, s16 cellZ, Vec3f orig, Vec3f normalized_dir,
                                     f32 dir_length, struct Surface **hit_surface, Vec3f hit_pos,
                                     f32 *max_length, u8 cellFlags) {
    // Skip if OOB
    if (cellX >= 0 && cellX <= 0x3F && cellZ >= 0 && cellZ <= 0x3F) {
        // Iterate through each surface in this partition
        if (cellFlags & 4) {
            if (normalized_dir[1] > -0.99f) {
                find_surface_on_ray_list(
                    gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_CEILS].next, orig,
                    normalized_dir, dir_length, hit_surface, hit_pos, max_length);
                find_surface_on_ray_list(
                    gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_CEILS].next, orig,
                    normalized_dir, dir_length, hit_surface, hit_pos, max_length);
            }
        }
        if (cellFlags & 1) {
            if (normalized_dir[1] < 0.99f) {
                find_surface_on_ray_list(
                    gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next, orig,
                    normalized_dir, dir_length, hit_surface, hit_pos, max_length);
                find_surface_on_ray_list(
                    gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next, orig,
                    normalized_dir, dir_length, hit_surface, hit_pos, max_length);
            }
        }
        if (cellFlags & 2) {
            find_surface_on_ray_list(
                gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS].next, orig,
                normalized_dir, dir_length, hit_surface, hit_pos, max_length);
            find_surface_on_ray_list(
                gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS].next, orig,
                normalized_dir, dir_length, hit_surface, hit_pos, max_length);
        }
    }
}

static void killWeirdFloats(void *addr) {
    u32 bits;
    s32 exponent;
    bits = *(u32 *) addr;
    exponent = ((bits & 0x7f800000U) >> 0x17) - 0x7f;
    if ((exponent >= -0x7e && exponent <= 0x7f) || bits == 0) {
        return;
    } else {
        *(u32 *) addr = 0.f;
    }
}
void find_surface_on_ray(Vec3f orig, Vec3f dir, struct Surface **hit_surface, Vec3f hit_pos,
                         u8 cellFlags) {
    f32 max_length;
    s16 cellZ, cellX;
    s16 LastcellZ, LastcellX;
    f32 fCellZ, fCellX;
    f32 dir_length;
    Vec3f normalized_dir;
    f32 step, dx, dz;
    u32 i;

    // Set that no surface has been hit
    *hit_surface = NULL;
    vec3f_sum(hit_pos, orig, dir);

    // Get normalized direction
    dir_length = vec3f_length(dir);
    max_length = dir_length;
    vec3f_copy(normalized_dir, dir);
    vec3f_normalize(normalized_dir);

    // Get our cell coordinate
    fCellX = (orig[0] + LEVEL_BOUNDARY_MAX) / CELL_SIZE;
    fCellZ = (orig[2] + LEVEL_BOUNDARY_MAX) / CELL_SIZE;
    cellX = (s16) fCellX;
    cellZ = (s16) fCellZ;

    // Get cells we cross using DDA
    killWeirdFloats(&dir[0]);
    killWeirdFloats(&dir[2]);
    if (absf(dir[0]) >= absf(dir[2])) {
        step = absf(dir[0]) / CELL_SIZE;
    } else {
        step = absf(dir[2]) / CELL_SIZE;
    }
    killWeirdFloats(&step);
    if (step < EPSILON) {
        return;
    }
    // step = sqrtf(absf(dir[0])*absf(dir[0]) + absf(dir[2])*absf(dir[2]) ) / CELL_SIZE;
    dx = dir[0] / step / CELL_SIZE;
    dz = dir[2] / step / CELL_SIZE;

    for (i = 0; i < (step) && *hit_surface == 0; i++) {
        find_surface_on_ray_cell(cellX, cellZ, orig, normalized_dir, dir_length, hit_surface, hit_pos,
                                 &max_length, cellFlags);

        // Move cell coordinate
        fCellX += dx;
        fCellZ += dz;
        LastcellX = cellX;
        LastcellZ = cellZ;
        cellX = (s16) fCellX;
        cellZ = (s16) fCellZ;
        if ((LastcellX != cellX) && (LastcellZ != cellZ)) {
            f32 incline = dx / dz;
            f32 movement = fCellZ - LastcellZ;
            s16 newCellX = (s16) (fCellX + incline * movement);
            if (newCellX == LastcellX) {
                find_surface_on_ray_cell(LastcellX, cellZ, orig, normalized_dir, dir_length,
                                         hit_surface, hit_pos, &max_length, cellFlags);
            } else {
                find_surface_on_ray_cell(cellX, LastcellZ, orig, normalized_dir, dir_length,
                                         hit_surface, hit_pos, &max_length, cellFlags);
            }
            /* find_surface_on_ray_cell(cellX, LastcellZ, orig, normalized_dir, dir_length,
               hit_surface, hit_pos, &max_length,cellFlags); find_surface_on_ray_cell(LastcellX, cellZ,
               orig, normalized_dir, dir_length, hit_surface, hit_pos, &max_length,cellFlags);*/
        }
    }
}
