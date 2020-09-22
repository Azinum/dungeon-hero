// renderer_opengl.h

typedef i32(glSwapInterval_t)(Display* Disp, GLXDrawable Drawable, i32 Interval);

typedef struct render_state {
  GLXContext Context;
  glSwapInterval_t* glSwapIntervalEXT;
} render_state;

i32 RendererInit(render_state* RenderState, assets* Assets);

void RendererDestroy();