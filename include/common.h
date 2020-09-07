// common.h

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))

#define V2(X, Y) ((v2) {X, Y})
#define V3(X, Y, Z) ((v3) {X, Y, Z})
#define V4(X, Y, Z, W) ((v4) {X, Y, Z, W})

#define Abs(X) (((X) >= 0) ? (X) : -(X))

#define Swap(V0, V1) { \
  __typeof__(V0) Temp = V0; \
  V0 = V1; \
  V1 = Temp; \
}

#define Min3(A, B, C) (((A < B) && (A < C)) ? (A) : ((B < C) ? (B) : C ))
#define Max3(A, B, C) (((A > B) && (A > C)) ? (A) : ((B > C) ? (B) : C ))

#define Min(A, B) ((A < B ) ? (A) : (B))
#define Max(A, B) ((A > B ) ? (A) : (B))

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
  float X, Y, Z, W;
} v4;

typedef struct v3 {
  float X, Y, Z;
} v3;

typedef struct v2 {
  float X, Y;
} v2;

typedef struct v3i {
  i32 X, Y, Z;
} v3i;

inline float Random(float From, float To) {
  return (float)rand() / (float)(RAND_MAX / To) + (float)rand() / (float)(RAND_MAX / From);
}

#define Assert(VALUE) assert(VALUE)
