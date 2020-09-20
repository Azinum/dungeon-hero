// software_renderer.h

// BGRa
typedef struct color {
  u8 B;
  u8 G;
  u8 R;
  u8 A;
} color;

#define COLOR(R, G, B) (color) {B, G, R, .A = 255}
#define COLORA(R, G, B, ColorA) (color) {B, G, R, .A = ColorA}

typedef struct framebuffer {
  union {
    u8* Data;
    color* Color;
  };
  u32 Width;
  u32 Height;
} framebuffer;

typedef struct render_state {
  framebuffer FrameBuffer;
  float* ZBuffer;
} render_state;

i32 RendererInit(u32 Width, u32 Height);

void RendererDestroy();
