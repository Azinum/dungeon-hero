// software_renderer.h

// BGRa
typedef struct color {
  u8 B;
  u8 G;
  u8 R;
  u8 A;
} color;

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
  i32* ZBuffer;
} render_state;

i32 RendererInit(u32 Width, u32 Height);

void RendererDestroy();
