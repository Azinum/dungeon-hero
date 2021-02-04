// types.h

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

typedef union quaternion {
  struct {
    float X, Y, Z, W;
  };
} quaternion;

typedef union v4 {
  struct {
    float X, Y, Z, W;
  };
} v4;

typedef union v3 {
  struct {
    float X, Y, Z;
  };
  struct {
    float R, G, B;
  };
} v3;

typedef union v2 {
  struct {
    float X, Y;
  };
  struct {
    float U, V;
  };
} v2;

typedef struct v3i {
  i32 X, Y, Z;
} v3i;

typedef struct string {
  char* Data;
  u32 Count;
} string;

typedef string buffer;
