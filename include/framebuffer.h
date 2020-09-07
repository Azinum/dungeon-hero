// framebuffer.h

// BGRa
typedef struct color {
  u8 B;
  u8 G;
  u8 R;
  u8 A;
} color;

typedef struct framebuffer {
  union {
    i8* Data;
    color* Color;
  };
  u32 Width;
  u32 Height;
} framebuffer;

