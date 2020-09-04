// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))

#define VEC2(X, Y) ((v2) {X, Y})
#define VEC3(X, Y, Z) ((v3) {X, Y, Z})
#define VEC4(X, Y, Z, W) ((v4) {X, Y, Z, W})

#define VEC3I(X, Y, Z) ((v3i) {X, Y, Z})
#define VEC3I_EQUAL(A, B) (A.x == B.x && A.y == B.y && A.z == B.z)
#define VEC3I_OFFSET(A, B) VEC3I(A.x - B.x, A.y - B.y, A.z - B.z)

typedef int64_t i64;
typedef uint64_t u64;
typedef int32_t i32;
typedef uint32_t u32;
typedef int16_t i16;
typedef uint16_t u16;
typedef int8_t i8;
typedef uint8_t u8;
typedef float r32;
typedef double r64;

typedef struct v4 {
  float x, y, z, w;
} v4;

typedef struct v3 {
  float x, y, z;
} v3;

typedef struct v2 {
  float x, y;
} v2;

typedef struct v3i {
  i32 x, y, z;
} v3i;

inline float random_number(float from, float to) {
  return (float)rand() / (float)(RAND_MAX / to) + (float)rand() / (float)(RAND_MAX / from);
}

#define Assert(VALUE) assert(VALUE)

#endif
