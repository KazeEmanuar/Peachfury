#ifndef SURFACE_LOAD_H
#define SURFACE_LOAD_H

#include <PR/ultratypes.h>

#include "types.h"

#define WALLMAXNORMAL 0.03f

struct SurfaceNode
{
    struct SurfaceNode *next;
    struct Surface *surface;
};

enum
{
    SPATIAL_PARTITION_FLOORS,
    SPATIAL_PARTITION_CEILS,
    SPATIAL_PARTITION_WALLS
};

typedef struct SurfaceNode SpatialPartitionCell[3];


#define CELLCOUNT 64
extern SpatialPartitionCell gStaticSurfacePartition[CELLCOUNT][CELLCOUNT];
extern SpatialPartitionCell gDynamicSurfacePartition[CELLCOUNT][CELLCOUNT];
extern struct SurfaceNode *sSurfaceNodePool;
extern struct Surface *sSurfacePool;
extern s16 sSurfacePoolSize;

void alloc_surface_pools(void);
#ifdef NO_SEGMENTED_MEMORY
u32 get_area_terrain_size(s16 *data);
#endif
void load_area_terrain(s16 index, s16 *data, s8 *surfaceRooms, s16 *macroObjects);
void clear_dynamic_surfaces(void);
void load_object_collision_model(void);

#endif // SURFACE_LOAD_H
