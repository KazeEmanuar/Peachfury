#include <ultra64.h>

#include "sm64.h"
#include "engine/graph_node.h"
#include "math_util.h"
#include "surface_collision.h"

#include "trig_tables.inc.c"

// Variables for a spline curve animation (used for the flight path in the grand star cutscene)
Vec4s *gSplineKeyframe;
float gSplineKeyframeFraction;
int gSplineState;

// These functions have bogus return values.
// Disable the compiler warning.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
#pragma intrinsic(sqrtf);

/// Copy vector 'src' to 'dest'
void *vec3f_copy(Vec3f dest, Vec3f src) {
    register x = ((u32 *) src)[0];
    register y = ((u32 *) src)[1];
    register z = ((u32 *) src)[2];
    ((u32 *) dest)[0] = x;
    ((u32 *) dest)[1] = y;
    ((u32 *) dest)[2] = z;
}
void *vec3f_mult(Vec3f dest, f32 a) {
    register f32 *temp = dest;
    while (temp < dest + 3) {
        *temp *= a;
        temp++;
    }
}
/// Add vector 'a' to 'dest'
void *vec3f_add(Vec3f dest, Vec3f a) {
    register f32 *temp = dest;
    register f32 sum, sum2;
    while (temp < dest + 3) {
        sum = *a;
        a++;
        sum2 = *temp;
        temp++;
        temp[-1] = sum + sum2;
    }
}

/// Make 'dest' the sum of vectors a and b.
void *vec3f_sum(Vec3f dest, Vec3f a, Vec3f b) {
    register f32 *temp = dest;
    register f32 x, y;
    while (temp < dest + 3) {
        x = *a;
        a++;
        y = *b;
        b++;
        *temp = x + y;
        temp++;
    }
}

/// Copy vector src to dest
void *vec3s_copy(Vec3s dest, Vec3s src) { // TODO: use LW add alignment to types.h
    register s16 x = src[0];
    register s16 y = src[1];
    register s16 z = src[2];

    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
}

/// Add vector a to 'dest'
void *vec3s_add(Vec3s dest, Vec3s a) {
    dest[0] += a[0];
    dest[1] += a[1];
    dest[2] += a[2];
}

/// Make 'dest' the sum of vectors a and b.
void *vec3s_sum(Vec3s dest, Vec3s a, Vec3s b) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
}

/// Subtract vector a from 'dest'
void *vec3s_sub(Vec3s dest, Vec3s a) {
    dest[0] -= a[0];
    dest[1] -= a[1];
    dest[2] -= a[2];
}

/// Convert short vector a to float vector 'dest'
void *vec3s_to_vec3f(Vec3f dest, Vec3s a) {
    dest[0] = a[0];
    dest[1] = a[1];
    dest[2] = a[2];
}

/**
 * Convert float vector a to a short vector 'dest' by rounding the components
 * to the nearest integer.
 */
void *vec3f_to_vec3s(Vec3s dest, Vec3f a) {
    // add/subtract 0.5 in order to round to the nearest s32 instead of truncating
    dest[0] = a[0] + ((a[0] > 0) ? 0.5f : -0.5f);
    dest[1] = a[1] + ((a[1] > 0) ? 0.5f : -0.5f);
    dest[2] = a[2] + ((a[2] > 0) ? 0.5f : -0.5f);
}

/**
 * Set 'dest' the normal vector of a triangle with vertices a, b and c.
 * It is similar to vec3f_cross, but it calculates the vectors (c-b) and (b-a)
 * at the same time.
 */
void *find_vector_perpendicular_to_plane(Vec3f dest, Vec3f a, Vec3f b, Vec3f c) {
    dest[0] = (b[1] - a[1]) * (c[2] - b[2]) - (c[1] - b[1]) * (b[2] - a[2]);
    dest[1] = (b[2] - a[2]) * (c[0] - b[0]) - (c[2] - b[2]) * (b[0] - a[0]);
    dest[2] = (b[0] - a[0]) * (c[1] - b[1]) - (c[0] - b[0]) * (b[1] - a[1]);
}

/// Make vector 'dest' the cross product of vectors a and b.
void *vec3f_cross(Vec3f dest, Vec3f a, Vec3f b) {
    dest[0] = a[1] * b[2] - b[1] * a[2];
    dest[1] = a[2] * b[0] - b[2] * a[0];
    dest[2] = a[0] * b[1] - b[0] * a[1];
}

/// Scale vector 'dest' so it has length 1
void *vec3f_normalize(Vec3f dest) {
    f32 size = sqrtf(dest[0] * dest[0] + dest[1] * dest[1] + dest[2] * dest[2]);
    register f32 invsqrt;
    if (size > 0.01f) {

        invsqrt = 1.0f / size;

        dest[0] *= invsqrt;
        dest[1] *= invsqrt;
        dest[2] *= invsqrt;
    } else {
        dest[0] = 0;
        dest[1] = 1;
        dest[2] = 0;
    }
}

#pragma GCC diagnostic pop
struct CopyMe {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
    f32 x1;
    f32 y1;
    f32 z1;
    f32 w1;
    f32 x2;
    f32 y2;
    f32 z2;
    f32 w2;
    f32 x3;
    f32 y3;
    f32 z3;
    f32 w3;
};
/// Copy matrix 'src' to 'dest'
void mtxf_copy(register Mat4 dest,
               register Mat4 src) { // TODO: handwrite, use LD, add alignment to types.h
    *((struct CopyMe *) dest) = *((struct CopyMe *) src);
}

/**
 * Set mtx to the identity matrix
 */
void mtxf_identity(register Mat4 mtx) {
    s32 i;
    f32 *dest;
    for (dest = (f32 *) mtx + 1, i = 0; i < 14; dest++, i++) {
        *dest = 0;
    }
    for (dest = (f32 *) mtx, i = 0; i < 4; dest += 5, i++) {
        *((u32 *) dest) = 0x3F800000;
    }
}

/**
 * Set dest to a translation matrix of vector b
 */
void mtxf_translate(Mat4 dest, Vec3f b) {
    register s32 i;
    register f32 *pen;
    for (pen = (f32 *) dest + 1, i = 0; i < 12; pen++, i++) {
        *pen = 0;
    }
    for (pen = (f32 *) dest, i = 0; i < 4; pen += 5, i++) {
        *((u32 *) pen) = 0x3F800000;
    }

    vec3f_copy(&dest[3][0], &b[0]);
}

f32 lookAtCalc(f32 sqrtsqrt) {
    if (sqrtsqrt == 0)
        sqrtsqrt = 0.00000000001;
    return -1.0 / sqrtsqrt;
}
void *matRow_mult(f32 *dest, f32 a) {
    register f32 *temp = dest;
    while (temp < dest + 12) {
        *temp *= a;
        temp += 4;
    }
}
f32 matRow_length(f32 *a) {
    return sqrtf(a[0] * a[0] + a[4] * a[4] + a[8] * a[8]);
}
__attribute__((aligned(8))) u8 indices[5] = { 0, 1, 2, 0, 1 };
void mtxf_lookat(Mat4 mtx, Vec3f from, Vec3f to, s16 roll) {
    register f32 invLength;
    f32 dx;
    f32 dz;
    s32 i, j;

    dx = to[0] - from[0];
    dz = to[2] - from[2];

    invLength = lookAtCalc(sqrtf(dx * dx + dz * dz));
    dx *= invLength;
    dz *= invLength;

    mtx[1][1] = coss(roll);
    mtx[0][1] = sins(roll) * dz;
    mtx[2][1] = -sins(roll) * dx;

    for (i = 0; i < 3; i++) {
        mtx[i][2] = to[i] - from[i];
    }
    matRow_mult(&mtx[0][2], lookAtCalc(matRow_length(&mtx[0][2])));

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 3; i++) {
            mtx[i][j] = mtx[indices[1 + i]][indices[j+1]] * mtx[indices[2 + i]][indices[j+2]]
                        - mtx[indices[2 + i]][indices[j+1]] * mtx[indices[1 + i]][indices[j+2]];
        }
        matRow_mult(&mtx[0][j], -lookAtCalc(matRow_length(&mtx[0][j])));
    }

    for (i = 0; i < 3; i++) {
        mtx[3][i] = -(from[0] * mtx[0][i] + from[1] * mtx[1][i] + from[2] * mtx[2][i]);
        mtx[i][3] = 0;
    }
    ((u32 *) mtx)[15] = 0x3F800000;
}

/**
 * Build a matrix that rotates around the z axis, then the x axis, then the y
 * axis, and then translates.
 */
void mtxf_rotate_zxy_and_translate(Mat4 dest, Vec3f translate, Vec3s rotate) {
    register f32 sx = sins(rotate[0]);
    register f32 cx = coss(rotate[0]);

    register f32 sy = sins(rotate[1]);
    register f32 cy = coss(rotate[1]);

    register f32 sz = sins(rotate[2]);
    register f32 cz = coss(rotate[2]);
    s32 i;

    dest[0][0] = cy * cz + sx * sy * sz;
    dest[1][0] = -cy * sz + sx * sy * cz;
    dest[2][0] = cx * sy;
    //  dest[3][0] = translate[0];

    dest[0][1] = cx * sz;
    dest[1][1] = cx * cz;
    dest[2][1] = -sx;
    //  dest[3][1] = translate[1];

    dest[0][2] = -sy * cz + sx * cy * sz;
    dest[1][2] = sy * sz + sx * cy * cz;
    dest[2][2] = cx * cy;
    //  dest[3][2] = translate[2];

    //  dest[0][3] = dest[1][3] = dest[2][3] = 0.;

    for (i = 0; i < 3; i++) {
        dest[3][i] = translate[i];
        dest[i][3] = 0;
    }
    ((u32 *) dest)[15] = 0x3F800000;
}

f32 billboardMatrix[3][4] = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 1.f, 0 },
};

/**
 * Set 'dest' to a transformation matrix that turns an object to face the camera.
 * 'mtx' is the look-at matrix from the camera
 * 'position' is the position of the object in the world
 * 'angle' rotates the object while still facing the camera.
 */
void mtxf_billboard(Mat4 dest, Mat4 mtx, Vec3f position, s16 angle) {
    register f32 *temp, *temp2;
    memcpy(dest, billboardMatrix, 0x48);
    ((u32 *) dest)[15] = 0x3F800000;
    temp = dest;
    temp2 = mtx;
    while (temp < ((f32 *) dest) + 3) {
        temp[12] = temp2[0] * position[0] + temp2[4] * position[1] + temp2[8] * position[2] + temp2[12];
        temp++;
        temp2++;
    }
}

/**
 * Set 'dest' to a transformation matrix that aligns an object with the terrain
 * based on the normal. Used for enemies.
 * 'upDir' is the terrain normal
 * 'yaw' is the angle which it should face
 * 'pos' is the object's position in the world
 */
void mtxf_align_terrain_normal(Mat4 dest, Vec3f upDir, Vec3f pos, s16 yaw) {
    Vec3f lateralDir;
    Vec3f leftDir;
    Vec3f forwardDir;
    s32 i;

    vec3f_set(lateralDir, sins(yaw), 0, coss(yaw));
    vec3f_normalize(upDir);

    vec3f_cross(leftDir, upDir, lateralDir);
    vec3f_normalize(leftDir);

    vec3f_cross(forwardDir, leftDir, upDir);
    vec3f_normalize(forwardDir);

    for (i = 0; i < 3; i++) {
        dest[0][i] = leftDir[i];
        dest[1][i] = upDir[i];
        dest[2][i] = forwardDir[i];
        dest[3][i] = pos[i];
    }
}

void mtxf_align_terrain_shadow(Mat4 dest, Vec3f upDir, Vec3f pos, f32 scale, s16 yaw) {
    Vec3f lateralDir;
    Vec3f leftDir;
    Vec3f forwardDir;
    s32 i;
    vec3f_set(lateralDir, sins(yaw), 0, coss(yaw));
    vec3f_normalize(upDir);

    vec3f_cross(leftDir, upDir, lateralDir);
    vec3f_normalize(leftDir);

    vec3f_cross(forwardDir, leftDir, upDir);
    vec3f_normalize(forwardDir);

    for (i = 0; i < 3; i++) {
        dest[0][i] = leftDir[i] * scale;
        dest[1][i] = upDir[i] * scale;
        dest[2][i] = forwardDir[i] * scale;
        dest[3][i] = pos[i];
    }
}
/**
 * Set 'mtx' to a transformation matrix that aligns an object with the terrain
 * based on 3 height samples in an equilateral triangle around the object.
 * Used for Mario when crawling or sliding.
 * 'yaw' is the angle which it should face
 * 'pos' is the object's position in the world
 * 'radius' is the distance from each triangle vertex to the center
 */
void mtxf_align_terrain_triangle(Mat4 mtx, Vec3f pos, s16 yaw, f32 radius) {
    struct Surface *sp74;
    Vec3f point0;
    Vec3f point1;
    Vec3f point2;
    Vec3f forward;
    Vec3f xColumn;
    Vec3f yColumn;
    Vec3f zColumn;
    f32 avgY;
    f32 minY = -radius * 3;

    point0[0] = pos[0] + radius * sins(yaw + 0x2AAA);
    point0[2] = pos[2] + radius * coss(yaw + 0x2AAA);
    point1[0] = pos[0] + radius * sins(yaw + 0x8000);
    point1[2] = pos[2] + radius * coss(yaw + 0x8000);
    point2[0] = pos[0] + radius * sins(yaw + 0xD555);
    point2[2] = pos[2] + radius * coss(yaw + 0xD555);

    point0[1] = find_floor(point0[0], pos[1] + 150, point0[2], &sp74);
    point1[1] = find_floor(point1[0], pos[1] + 150, point1[2], &sp74);
    point2[1] = find_floor(point2[0], pos[1] + 150, point2[2], &sp74);

    if (point0[1] - pos[1] < minY) {
        point0[1] = pos[1];
    }

    if (point1[1] - pos[1] < minY) {
        point1[1] = pos[1];
    }

    if (point2[1] - pos[1] < minY) {
        point2[1] = pos[1];
    }

    avgY = (point0[1] + point1[1] + point2[1]) * .3333333333333333333333333333333333333333333333f;

    vec3f_set(forward, sins(yaw), 0, coss(yaw));
    find_vector_perpendicular_to_plane(yColumn, point0, point1, point2);
    vec3f_normalize(yColumn);
    vec3f_cross(xColumn, yColumn, forward);
    vec3f_normalize(xColumn);
    vec3f_cross(zColumn, xColumn, yColumn);
    vec3f_normalize(zColumn);

    mtx[0][0] = xColumn[0];
    mtx[0][1] = xColumn[1];
    mtx[0][2] = xColumn[2];
    mtx[3][0] = pos[0];

    mtx[1][0] = yColumn[0];
    mtx[1][1] = yColumn[1];
    mtx[1][2] = yColumn[2];
    mtx[3][1] = (avgY < pos[1]) ? pos[1] : avgY;

    mtx[2][0] = zColumn[0];
    mtx[2][1] = zColumn[1];
    mtx[2][2] = zColumn[2];
    mtx[3][2] = pos[2];

    mtx[0][3] = 0;
    mtx[1][3] = 0;
    mtx[2][3] = 0;
    ((u32 *) mtx)[15] = 0x3F800000;
}

/**
 * Sets matrix 'dest' to the matrix product b * a assuming they are both
 * transformation matrices with a w-component of 1. Since the bottom row
 * is assumed to equal [0, 0, 0, 1], it saves some multiplications and
 * addition.
 * The resulting matrix represents first applying transformation b and
 * then a.
 */
void mtxf_mul(Mat4 dest, Mat4 a, Mat4 b) {
    register f32 entry0;
    register f32 entry1;
    register f32 entry2;
    register f32 *temp = a;
    register f32 *temp2 = dest;
    register f32 *temp3;
    register s32 i;
    for (i = 0; i < 16; i++) {
        entry0 = temp[0];
        entry1 = temp[1];
        entry2 = temp[2];
        temp3 = b;
        for (; (i & 3) != 3; i++) {
            *temp2 = entry0 * temp3[0] + entry1 * temp3[4] + entry2 * temp3[8];
            temp2++;
            temp3++;
        }
        *temp2 = 0;
        temp += 4;
        temp2++;
    }
    vec3f_add(&dest[3][0], &b[3][0]);
    ((u32 *) dest)[15] = 0x3F800000;
}
/*for (i = 0; i < 16; i++) {
        entry0 = temp[0];
        entry1 = temp[1];
        entry2 = temp[2];
        temp3 = b;
        for (; i & 3; i++) {
            *temp2 = entry0 * temp3[0] + entry1 * temp3[4] + entry2 * temp3[8];
            temp2++;
            temp3++;
        }
        *temp2 = 0;
        temp += 4;
        temp2++;
    }*/

/**
 * Set matrix 'dest' to 'mtx' scaled by vector s
 */
void mtxf_scale_vec3f(Mat4 dest, Mat4 mtx, register Vec3f s) {
    register f32 *temp = dest;
    register f32 *temp2 = mtx;
    register s32 i;
    while (temp < ((f32 *) dest) + 4) {
        for (i = 0; i < 3; i++) {
            temp[i * 4] = temp2[i * 4] * s[i];
        }
        temp[12] = temp2[12];
        temp++;
        temp2++;
    }
}

/**
 * Multiply a vector with a transformation matrix, which applies the transformation
 * to the point. Note that the bottom row is assumed to be [0, 0, 0, 1], which is
 * true for transformation matrices if the translation has a w component of 1.
 */
/*void mtxf_mul_vec3s(Mat4 mtx, Vec3s b) {
    register f32 x = b[0];
    register f32 y = b[1];
    register f32 z = b[2];
    register f32 *temp2 = mtx;
    register s32 i;
    register s16 *c = b;
    while (temp2 < ((f32 *) mtx) + 3) {
        c[0] = x * temp2[0] + y * temp2[4] + z * temp2[8] + temp2[12];
        c++;
        temp2++;
    }
}*/

#define WORLD_SCALE 1.f
#define ALWAYS_INLINE __attribute__((always_inline)) inline
static ALWAYS_INLINE float construct_float(const float f)
{
    u32 r;
    float f_out;
    u32 i = *(u32*)(&f);

    if (!__builtin_constant_p(i))
    {
        return *(float*)(&i);
    }

    u32 upper = (i >> 16);
    u32 lower = (i >>  0) & 0xFFFF;

    if ((i & 0xFFFF) == 0) {
        __asm__ ("lui %0, %1"
                                : "=r"(r)
                                : "K"(upper));
    } else if ((i & 0xFFFF0000) == 0) {
        __asm__ ("addiu %0, $0, %1"
                                : "+r"(r)
                                : "K"(lower));
    } else {
        __asm__ ("lui %0, %1"
                                : "=r"(r)
                                : "K"(upper));
        __asm__ ("addiu %0, %0, %1"
                                : "+r"(r)
                                : "K"(lower));
    }

    __asm__ ("mtc1 %1, %0"
                         : "=f"(f_out)
                         : "r"(r));
    return f_out;
}

static ALWAYS_INLINE float mul_without_nop(float a, float b)
{
    float ret;
    __asm__ ("mul.s %0, %1, %2"
                         : "=f"(ret)
                         : "f"(a), "f"(b));
    return ret;
}

static ALWAYS_INLINE void swl(void* addr, s32 val, const int offset)
{
    __asm__ ("swl %1, %2(%0)"
                        : 
                        : "g"(addr), "g"(val), "I"(offset));
}

// Converts a floating point matrix to a fixed point matrix
// Makes some assumptions about certain fields in the matrix, which will always be true for valid matrices.
__attribute__((optimize("Os"))) __attribute__((aligned(32)))
void mtxf_to_mtx(s16* dst, float* src)
{
    int i;
    float scale = construct_float(65536.0f / WORLD_SCALE);
    // Iterate over rows of values in the input matrix
    for (i = 0; i < 4; i++)
    {
        // Read the three input in the current row (assume the fourth is zero)
        float a = src[4 * i + 0];
        float b = src[4 * i + 1];
        float c = src[4 * i + 2];
        float a_scaled = mul_without_nop(a,scale);
        float b_scaled = mul_without_nop(b,scale);
        float c_scaled = mul_without_nop(c,scale);

        // Convert the three inputs to fixed
        s32 a_int = (s32)a_scaled;
        s32 b_int = (s32)b_scaled;
        s32 c_int = (s32)c_scaled;
        s32 c_high = c_int & 0xFFFF0000;
        s32 c_low = c_int << 16;
        
        // Write the integer part of a, as well as garbage into the next two bytes.
        // Those two bytes will get overwritten by the integer part of b.
        // This prevents needing to shift or mask the integer value of a.
        *(s32*)(&dst[4 * i +  0]) = a_int;
        // Write the fractional part of a
        dst[4 * i + 16] = (s16)a_int;

        // Write the integer part of b using swl to avoid needing to shift.
        swl(dst + 4 * i, b_int, 2);
        // Write the fractional part of b.
        dst[4 * i + 17] = (s16)b_int;

        // Write the integer part of c and two zeroes for the 4th column.
        *(s32*)(&dst[4 * i +  2]) = c_high;
        // Write the fractional part of c and two zeroes for the 4th column
        *(s32*)(&dst[4 * i + 18]) = c_low;
    }
    // Write 1.0 to the bottom right entry in the output matrix
    // The low half was already set to zero in the loop, so we only need
    //  to set the top half.
    dst[15] = 1;
}
#define MATENTRY(a, b)                                                                                 \
    ((s16 *) mtx)[a] = ((s32) b) >> 16;                                                                \
    ((s16 *) mtx)[a + 16] = ((s32) b) & 0xFFFF;
void mtxf_rotate_xy(Mtx *mtx, s16 angle) {
    register s32 i = coss(angle) * 65536;
    register s32 j = sins(angle) * 65536;
    register f32 *temp = mtx;
    for (; temp < mtx + 1;) {
        *temp = 0;
        temp++;
    }
    MATENTRY(0, i)
    MATENTRY(1, j)
    MATENTRY(4, -j)
    MATENTRY(5, i)
    ((s16 *) mtx)[10] = 1;
    ((s16 *) mtx)[15] = 1;
}

/**
 * Extract a position given an object's transformation matrix and a camera matrix.
 * This is used for determining the world position of the held object: since objMtx
 * inherits the transformation from both the camera and Mario, it calculates this
 * by taking the camera matrix and inverting its transformation by first rotating
 * objMtx back from screen orientation to world orientation, and then subtracting
 * the camera position.
 */
void get_pos_from_transform_mtx(Vec3f dest, Mat4 objMtx, register Mat4 camMtx) {
    register s32 i;
    register f32 *temp = dest;
    register f32 *temp2 = camMtx;
    f32 y[3];

    y[0] = (((f32 *) objMtx)[12] - temp2[12]);
    y[1] = (((f32 *) objMtx)[13] - temp2[13]);
    y[2] = (((f32 *) objMtx)[14] - temp2[14]);
    for (i = 0; i < 3; i++) {
        *temp = y[0] * temp2[0] + y[1] * temp2[1] + y[2] * temp2[2];
        temp++;
        temp2 += 4;
    }
}

/**
 * Take the vector starting at 'from' pointed at 'to' an retrieve the length
 * of that vector, as well as the yaw and pitch angles.
 * Basically it converts the direction to spherical coordinates.
 */
void vec3f_get_dist_and_angle(Vec3f from, Vec3f to, f32 *dist, s16 *pitch, s16 *yaw) {
    register f32 x = to[0] - from[0];
    register f32 y = to[1] - from[1];
    register f32 z = to[2] - from[2];
    register f32 xs = x * x;
    register f32 zs = z * z;

    *dist = sqrtf(xs + zs + y * y);
    *pitch = atan2s(sqrtf(xs + zs), y);
    *yaw = atan2s(z, x);
}

/**
 * Construct the 'to' point which is distance 'dist' away from the 'from' position,
 * and has the angles pitch and yaw.
 */
void vec3f_set_dist_and_angle(Vec3f from, Vec3f to, f32 dist, s16 pitch, s16 yaw) {
    to[0] = from[0] + dist * coss(pitch) * sins(yaw);
    to[1] = from[1] + dist * sins(pitch);
    to[2] = from[2] + dist * coss(pitch) * coss(yaw);
}

/**
 * Return the value 'current' after it tries to approach target, going up at
 * most 'inc' and going down at most 'dec'.
 */
s32 approach_s32(s32 current, s32 target, s32 inc, s32 dec) {
    //! If target is close to the max or min s32, then it's possible to overflow
    // past it without stopping.

    if (current < target) {
        current += inc;
        if (current > target) {
            current = target;
        }
    } else {
        current -= dec;
        if (current < target) {
            current = target;
        }
    }
    return current;
}

/**
 * Return the value 'current' after it tries to approach target, going up at
 * most 'inc' and going down at most 'dec'.
 */
f32 approach_f32(f32 current, f32 target, f32 inc, f32 dec) {
    if (current < target) {
        current += inc;
        if (current > target) {
            current = target;
        }
    } else {
        current -= dec;
        if (current < target) {
            current = target;
        }
    }
    return current;
}

/**
 * Helper function for atan2s. Does a look up of the arctangent of y/x assuming
 * the resulting angle is in range [0, 0x2000] (1/8 of a circle).
 */
static u16 atan2_lookup(f32 y, f32 x) {
    u16 ret;

    if (x == 0) {
        ret = gArctanTable[0];
    } else {
        ret = gArctanTable[(s32) (y / x * 1024 + 0.5f)];
    }
    return ret;
}

/**
 * Compute the angle from (0, 0) to (x, y) as a s16. Given that terrain is in
 * the xz-plane, this is commonly called with (z, x) to get a yaw angle.
 */
s32 atan2s(f32 y, f32 x) {
    u16 ret;

    if (x >= 0) {
        if (y >= 0) {
            if (y >= x) {
                ret = atan2_lookup(x, y);
            } else {
                ret = 0x4000 - atan2_lookup(y, x);
            }
        } else {
            y = -y;
            if (y < x) {
                ret = 0x4000 + atan2_lookup(y, x);
            } else {
                ret = 0x8000 - atan2_lookup(x, y);
            }
        }
    } else {
        x = -x;
        if (y < 0) {
            y = -y;
            if (y >= x) {
                ret = 0x8000 + atan2_lookup(x, y);
            } else {
                ret = 0xC000 - atan2_lookup(y, x);
            }
        } else {
            if (y < x) {
                ret = 0xC000 + atan2_lookup(y, x);
            } else {
                ret = -atan2_lookup(x, y);
            }
        }
    }
    return ret;
}

/// Get length of vector 'a'
f32 vec3f_length(Vec3f a) {
    return sqrtf(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
}

/// Get dot product of vectors 'a' and 'b'
f32 vec3f_dot(Vec3f a, Vec3f b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}