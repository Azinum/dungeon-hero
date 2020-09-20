// common.h

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "types.h"

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))
#define MAX_PATH_SIZE 512

#define Assert(VALUE) assert(VALUE)

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

inline float Random(float From, float To) {
  return (float)rand() / (float)(RAND_MAX / To) + (float)rand() / (float)(RAND_MAX / From);
}

