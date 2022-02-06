#include <PR/ultratypes.h>

#include "sm64.h"
#include "game/debug.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/object_list_processor.h"
#include "surface_collision.h"
#include "surface_load.h"

/**************************************************
 *                      WALLS                     *
 **************************************************/

/**
 * Iterate through the list of walls until all walls are checked and
 * have given their wall push.
 */
static s32 find_wall_collisions_from_list(struct SurfaceNode *surfaceNode,
                                          struct WallCollisionData *data) {
    const f32 corner_threshold = -0.9f;

    register struct Surface *surf;
    register f32 offset;
    register f32 radius = data->radius;
    register f32 x = data->x;
    register f32 y = data->y + data->offsetY;
    register f32 z = data->z;
    register f32 v0x, v0y, v0z;
    register f32 v1x, v1y, v1z;
    register f32 v2x, v2y, v2z;
    register f32 d00, d01, d11, d20, d21;
    register f32 invDenom;
    register f32 v, w;
    register f32 margin_radius = radius - 1.0f;

    s32 numCols = 0;

    // Max collision radius = 200
    /*if (radius > 200.0f) {
        radius = 200.0f;
    }*/

    // Stay in this loop until out of walls.
    while (surfaceNode != NULL) {
        surf = surfaceNode->surface;
        surfaceNode = surfaceNode->next;
        if ((SURFACETYPE(surf) == 0x00AD) && !(gMarioState->flags & MARIO_VANISH_CAP)) {
            continue;
        }
        // Exclude a large number of walls immediately to optimize.
        if (y < surf->lowerY || y > surf->upperY) {
            continue;
        }

        offset = surf->normal.x * x + surf->normal.y * y + surf->normal.z * z + surf->originOffset;

        if (offset < 0 || offset > radius) {
            continue;
        }

        v0x = (f32) (surf->vertex2[0] - surf->vertex1[0]);
        v0y = (f32) (surf->vertex2[1] - surf->vertex1[1]);
        v0z = (f32) (surf->vertex2[2] - surf->vertex1[2]);

        v1x = (f32) (surf->vertex3[0] - surf->vertex1[0]);
        v1y = (f32) (surf->vertex3[1] - surf->vertex1[1]);
        v1z = (f32) (surf->vertex3[2] - surf->vertex1[2]);

        v2x = x - (f32) surf->vertex1[0];
        v2y = y - (f32) surf->vertex1[1];
        v2z = z - (f32) surf->vertex1[2];

        d00 = v0x * v0x + v0y * v0y + v0z * v0z;
        d01 = v0x * v1x + v0y * v1y + v0z * v1z;
        d11 = v1x * v1x + v1y * v1y + v1z * v1z;
        d20 = v2x * v0x + v2y * v0y + v2z * v0z;
        d21 = v2x * v1x + v2y * v1y + v2z * v1z;
        invDenom = 1.0f / (d00 * d11 - d01 * d01);
        v = (d11 * d20 - d01 * d21) * invDenom;
        if (v < 0.0f || v > 1.0f)
            goto edge_1_2;

        w = (d00 * d21 - d01 * d20) * invDenom;
        if (w < 0.0f || w > 1.0f || v + w > 1.0f)
            goto edge_1_2;

        x += surf->normal.x * (radius - offset);
        z += surf->normal.z * (radius - offset);
        goto hasCollision;

    edge_1_2:
        if (offset < 0)
            continue;
        // Edge 1-2
        if (v0y != 0.0f) {
            v = (v2y / v0y);
            if (v < 0.0f || v > 1.0f)
                goto edge_1_3;
            d00 = v0x * v - v2x;
            d01 = v0z * v - v2z;
            invDenom = sqrtf(d00 * d00 + d01 * d01);
            offset = invDenom - margin_radius;
            if (offset > 0.0f)
                goto edge_1_3;
            invDenom = offset / invDenom;
            x += (d00 *= invDenom);
            z += (d01 *= invDenom);
            margin_radius += 0.01f;

            if (d00 * surf->normal.x + d01 * surf->normal.z < corner_threshold * offset)
                continue;
            else
                goto hasCollision;
        }

    edge_1_3:
        // Edge 1-3
        if (v1y != 0.0f) {
            v = (v2y / v1y);
            if (v < 0.0f || v > 1.0f)
                goto edge_2_3;
            d00 = v1x * v - v2x;
            d01 = v1z * v - v2z;
            invDenom = sqrtf(d00 * d00 + d01 * d01);
            offset = invDenom - margin_radius;
            if (offset > 0.0f)
                goto edge_2_3;
            invDenom = offset / invDenom;
            x += (d00 *= invDenom);
            z += (d01 *= invDenom);
            margin_radius += 0.01f;

            if (d00 * surf->normal.x + d01 * surf->normal.z < corner_threshold * offset)
                continue;
            else
                goto hasCollision;
        }

    edge_2_3:
        // Edge 2-3
        v1x = (f32) (surf->vertex3[0] - surf->vertex2[0]);
        v1y = (f32) (surf->vertex3[1] - surf->vertex2[1]);
        v1z = (f32) (surf->vertex3[2] - surf->vertex2[2]);

        v2x = x - (f32) surf->vertex2[0];
        v2y = y - (f32) surf->vertex2[1];
        v2z = z - (f32) surf->vertex2[2];

        if (v1y != 0.0f) {
            v = (v2y / v1y);
            if (v < 0.0f || v > 1.0f)
                continue;
            d00 = v1x * v - v2x;
            d01 = v1z * v - v2z;
            invDenom = sqrtf(d00 * d00 + d01 * d01);
            offset = invDenom - margin_radius;
            if (offset > 0.0f)
                continue;
            invDenom = offset / invDenom;
            x += (d00 *= invDenom);
            z += (d01 *= invDenom);
            margin_radius += 0.01f;
            if (d00 * surf->normal.x + d01 * surf->normal.z < corner_threshold * offset)
                continue;
            else
                goto hasCollision;
        } else
            continue;

    hasCollision:
        //! (Unreferenced Walls) Since this only returns the first four walls,
        //  this can lead to wall interaction being missed. Typically unreferenced walls
        //  come from only using one wall, however.
        if (data->numWalls < 4) {
            data->walls[data->numWalls++] = surf;
        }

        numCols++;
    }
    data->x = x;
    data->z = z;

    return numCols;
}

/**
 * Formats the position and wall search for find_wall_collisions.
 */
s32 f32_find_wall_collision(f32 *xPtr, f32 *yPtr, f32 *zPtr, f32 offsetY, f32 radius) {
    struct WallCollisionData collision;
    s32 numCollisions = 0;

    collision.offsetY = offsetY;
    collision.radius = radius;

    collision.x = *xPtr;
    collision.y = *yPtr;
    collision.z = *zPtr;

    collision.numWalls = 0;

    numCollisions = find_wall_collisions(&collision);

    *xPtr = collision.x;
    *yPtr = collision.y;
    *zPtr = collision.z;

    return numCollisions;
}

/**
 * Find wall collisions and receive their push.
 */
s32 find_wall_collisions(struct WallCollisionData *colData) {
    struct SurfaceNode *node;
    s16 cellX, cellZ;
    s32 numCollisions = 0;
    s32 xmin, xmax;
    s32 zmin, zmax;
    s16 cellXmin, cellXmax;
    s16 cellZmin, cellZmax;
    xmin = colData->x - colData->radius;
    xmax = colData->x + colData->radius;
    zmin = colData->z - colData->radius;
    zmax = colData->z + colData->radius;
    cellXmin = (((xmin) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    cellXmax = (((xmax) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    cellZmin = (((zmin) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    cellZmax = (((zmax) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    colData->numWalls = 0;

    for (cellX = cellXmin; cellX <= cellXmax; cellX++) {
        for (cellZ = cellZmin; cellZ <= cellZmax; cellZ++) {
            node = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS].next;
            numCollisions += find_wall_collisions_from_list(node, colData);
            node = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS].next;
            numCollisions += find_wall_collisions_from_list(node, colData);
        }
    }
    return numCollisions;
}

/**************************************************
 *                     CEILINGS                   *
 **************************************************/

/**
 * Iterate through the list of ceilings and find the first ceiling over a given point.
 */
static struct Surface *find_ceil_from_list(struct SurfaceNode *surfaceNode, s32 x, s32 y, s32 z,
                                           f32 *pheight) {
    register struct Surface *surf;
    register s32 x1, z1, x2, z2, x3, z3;
    struct Surface *ceil = NULL;

    ceil = NULL;

    // Stay in this loop until out of ceilings.
    while (surfaceNode != NULL) {
        surf = surfaceNode->surface;
        surfaceNode = surfaceNode->next;

        x1 = surf->vertex1[0];
        z1 = surf->vertex1[2];
        z2 = surf->vertex2[2];
        x2 = surf->vertex2[0];

        // Checking if point is in bounds of the triangle laterally.
        if ((z1 - z) * (x2 - x1) - (x1 - x) * (z2 - z1) > 0) {
            continue;
        }

        // Slight optimization by checking these later.
        x3 = surf->vertex3[0];
        z3 = surf->vertex3[2];
        if ((z2 - z) * (x3 - x2) - (x2 - x) * (z3 - z2) > 0) {
            continue;
        }
        if ((z3 - z) * (x1 - x3) - (x3 - x) * (z1 - z3) > 0) {
            continue;
        }

        {
            f32 nx = surf->normal.x;
            f32 ny = surf->normal.y;
            f32 nz = surf->normal.z;
            f32 oo = surf->originOffset;
            f32 height;

            // If a wall, ignore it. Likely a remnant, should never occur.
            if (ny == 0.0f) {
                continue;
            }

            // Find the ceil height at the specific point.
            height = -(x * nx + nz * z + oo) / ny;

            // Checks for ceiling interaction with a 78 unit buffer.
            //! (Exposed Ceilings) Because any point above a ceiling counts
            //  as interacting with a ceiling, ceilings far below can cause
            // "invisible walls" that are really just exposed ceilings.
            if (y - (height - -78.0f) > 0.0f) {
                continue;
            }

            *pheight = height;
            ceil = surf;
            break;
        }
    }

    //! (Surface Cucking) Since only the first ceil is returned and not the lowest,
    //  lower ceilings can be "cucked" by higher ceilings.
    return ceil;
}

/**
 * Find the lowest ceiling above a given position and return the height.
 */
f32 find_ceil(f32 posX, f32 posY, f32 posZ, struct Surface **pceil) {
    s16 cellZ, cellX;
    struct Surface *ceil, *dynamicCeil;
    struct SurfaceNode *surfaceList;
    f32 height = 20000.0f;
    f32 dynamicHeight = 20000.0f;
    s16 x, y, z;

    //! (Parallel Universes) Because position is casted to an s16, reaching higher
    // float locations  can return ceilings despite them not existing there.
    //(Dynamic ceilings will unload due to the range.)
    x = (s16) posX;
    y = (s16) posY;
    z = (s16) posZ;
    *pceil = NULL;

    // Each level is split into cells to limit load, find the appropriate cell.
    cellX = (((x) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    cellZ = (((z) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;

    // Check for surfaces belonging to objects.
    surfaceList = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_CEILS].next;
    dynamicCeil = find_ceil_from_list(surfaceList, x, y, z, &dynamicHeight);

    // Check for surfaces that are a part of level geometry.
    surfaceList = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_CEILS].next;
    ceil = find_ceil_from_list(surfaceList, x, y, z, &height);

    if (dynamicHeight < height) {
        ceil = dynamicCeil;
        height = dynamicHeight;
    }

    *pceil = ceil;

    return height;
}

/**************************************************
 *                     FLOORS                     *
 **************************************************/

/**
 * Find the height of the highest floor below an object.
 */
f32 obj_find_floor_height(struct Object *obj) {
    struct Surface *floor;
    f32 floorHeight = find_floor(obj->oPosX, obj->oPosY, obj->oPosZ, &floor);
    return floorHeight;
}

/**
 * Basically a local variable that passes through floor geo info.
 */
struct FloorGeometry sFloorGeo;

/**
 * Return the floor height underneath (xPos, yPos, zPos) and populate `floorGeo`
 * with data about the floor's normal vector and origin offset. Also update
 * sFloorGeo.
 */
f32 find_floor_height_and_data(f32 xPos, f32 yPos, f32 zPos, struct FloorGeometry **floorGeo) {
    struct Surface *floor;
    f32 floorHeight = find_floor(xPos, yPos, zPos, &floor);

    *floorGeo = NULL;

    if (floor != NULL) {
        sFloorGeo.normalX = floor->normal.x;
        sFloorGeo.normalY = floor->normal.y;
        sFloorGeo.normalZ = floor->normal.z;
        sFloorGeo.originOffset = floor->originOffset;

        *floorGeo = &sFloorGeo;
    }
    return floorHeight;
}

/**
 * Iterate through the list of floors and find the first floor under a given point.
 */
struct Surface *find_floor_from_list(struct SurfaceNode *surfaceNode, s32 x, s32 y, s32 z,
                                     f32 *pheight) {
    register struct Surface *surf;
    register s32 x1, z1, x2, z2, x3, z3;
    f32 nx, ny, nz;
    f32 oo;
    f32 height;
    struct Surface *floor = NULL;
    *pheight = -20000.f;

    // Iterate through the list of floors until there are no more floors.
    while (surfaceNode != NULL) {
        surf = surfaceNode->surface;
        surfaceNode = surfaceNode->next;
        if ((SURFACETYPE(surf) == 0x00AD) && !(gMarioState->flags & MARIO_VANISH_CAP)) {
            continue;
        }
        x1 = surf->vertex1[0];
        z1 = surf->vertex1[2];
        x2 = surf->vertex2[0];
        z2 = surf->vertex2[2];

        // Check that the point is within the triangle bounds.
        if ((z1 - z) * (x2 - x1) - (x1 - x) * (z2 - z1) < 0) {
            continue;
        }

        // To slightly save on computation time, set this later.
        x3 = surf->vertex3[0];
        z3 = surf->vertex3[2];

        if ((z2 - z) * (x3 - x2) - (x2 - x) * (z3 - z2) < 0) {
            continue;
        }
        if ((z3 - z) * (x1 - x3) - (x3 - x) * (z1 - z3) < 0) {
            continue;
        }

        nx = surf->normal.x;
        ny = surf->normal.y;
        nz = surf->normal.z;
        oo = surf->originOffset;

        // Find the height of the floor at a given location.
        height = -(x * nx + nz * z + oo) / ny;
        // Checks for floor interaction with a 78 unit buffer.
        if (y < (height + -78.0f)) {
            continue;
        }
        if (*pheight < height) {
            *pheight = height;
            floor = surf;
        }
        //   break;
    }

    //! (Surface Cucking) Since only the first floor is returned and not the highest,
    //  higher floors can be "cucked" by lower floors.
    return floor;
}

/**
 * Find the height of the highest floor below a point.
 */
f32 find_floor_height(f32 x, f32 y, f32 z) {
    struct Surface *floor;

    return find_floor(x, y, z, &floor);
}
f32 find_floor_marioair(f32 xPos, f32 yPos, f32 zPos, struct Surface **pfloor) {
    s16 cellZ, cellX;

    struct Surface *floor, *dynamicFloor;
    struct SurfaceNode *surfaceList;

    f32 height = -11000.0f;
    f32 dynamicHeight = -11000.0f;

    //! (Parallel Universes) Because position is casted to an s16, reaching higher
    // float locations  can return floors despite them not existing there.
    //(Dynamic floors will unload due to the range.)
    s16 x = (s16) xPos;
    s16 y = (s16) yPos;
    s16 z = (s16) zPos;

    // Each level is split into cells to limit load, find the appropriate cell.
    cellX = (((x) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    cellZ = (((z) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;

    // Check for surfaces belonging to objects.
    surfaceList = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next;
    if (gMarioState->vel[1] < 10.f) {
        dynamicFloor = find_floor_from_list(surfaceList, x, y, z, &dynamicHeight);
    } else {
        dynamicFloor = 0;
    }

    // Check for surfaces that are a part of level geometry.
    surfaceList = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next;
    floor = find_floor_from_list(surfaceList, x, y, z, &height);

    if (dynamicHeight > height) {
        floor = dynamicFloor;
        height = dynamicHeight;
    }

    *pfloor = floor;

    return height;
}
/**
 * Find the highest floor under a given position and return the height.
 */
f32 find_floor(f32 xPos, f32 yPos, f32 zPos, struct Surface **pfloor) {
    s16 cellZ, cellX;

    struct Surface *floor, *dynamicFloor;
    struct SurfaceNode *surfaceList;

    f32 height = -20000.0f;
    f32 dynamicHeight = -20000.0f;

    //! (Parallel Universes) Because position is casted to an s16, reaching higher
    // float locations  can return floors despite them not existing there.
    //(Dynamic floors will unload due to the range.)
    s16 x = (s16) xPos;
    s16 y = (s16) yPos;
    s16 z = (s16) zPos;

    *pfloor = NULL;

    // Each level is split into cells to limit load, find the appropriate cell.
    cellX = (((x) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;
    cellZ = (((z) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & 0x3F;

    // Check for surfaces belonging to objects.
    surfaceList = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next;
    dynamicFloor = find_floor_from_list(surfaceList, x, y, z, &dynamicHeight);

    // Check for surfaces that are a part of level geometry.
    surfaceList = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next;
    floor = find_floor_from_list(surfaceList, x, y, z, &height);

    if (dynamicHeight > height) {
        floor = dynamicFloor;
        height = dynamicHeight;
    }

    *pfloor = floor;

    return height;
}

/**************************************************
 *               ENVIRONMENTAL BOXES              *
 **************************************************/

/**
 * Finds the height of water at a given location.
 */
extern void doMarioWaterColl(f32 *waterLevel, struct MarioState *m);
f32 find_water_level(f32 x, f32 z) {
    s32 i;
    s32 numRegions;
    s16 val;
    f32 loX, hiX, loZ, hiZ;
    f32 waterLevel = -11000.0f;
    s16 *p = gEnvironmentRegions;

    if (p != NULL) {
        numRegions = *p++;

        for (i = 0; i < numRegions; i++) {
            val = *p++;
            loX = *p++;
            loZ = *p++;
            hiX = *p++;
            hiZ = *p++;

            // If the location is within a water box and it is a water box.
            // Water is less than 50 val only, while above is gas and such.
            if (loX < x && x < hiX && loZ < z && z < hiZ && val < 50) {
                // Set the water height. Since this breaks, only return the first height.
                waterLevel = *p;
                break;
            }
            p++;
        }
    }
    if (gCurrentObject == gMarioState->marioObj) {
        doMarioWaterColl(&waterLevel, gMarioState);
    }
    return waterLevel;
}

/**
 * Finds the height of the poison gas (used only in HMC) at a given location.
 */
f32 find_poison_gas_level(f32 x, f32 z) {
    s32 i;
    s32 numRegions;
    s16 val;
    f32 loX, hiX, loZ, hiZ;
    f32 gasLevel = -11000.0f;
    s16 *p = gEnvironmentRegions;

    if (p != NULL) {
        numRegions = *p++;

        for (i = 0; i < numRegions; i++) {
            val = *p;

            if (val >= 50) {
                loX = *(p + 1);
                loZ = *(p + 2);
                hiX = *(p + 3);
                hiZ = *(p + 4);

                // If the location is within a gas's box and it is a gas box.
                // Gas has a value of 50, 60, etc.
                if (loX < x && x < hiX && loZ < z && z < hiZ && val % 10 == 0) {
                    // Set the gas height. Since this breaks, only return the first height.
                    gasLevel = *(p + 5);
                    break;
                }
            }

            p += 6;
        }
    }

    return gasLevel;
}
