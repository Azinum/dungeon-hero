// software_renderer.h

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

i32 RendererInit(assets* Assets);

void RendererDestroy();
