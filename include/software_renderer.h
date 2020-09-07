// software_renderer.h

typedef struct render_state {
  framebuffer FrameBuffer;
} render_state;

i32 RendererInit(u32 Width, u32 Height);

void RendererDestroy();
