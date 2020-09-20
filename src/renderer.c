// renderer.c

#include "renderer.h"

static mat4 Projection;
static mat4 View;
static mat4 Model;

#if RENDERER_OPENGL
  #include "renderer_opengl.c"
#else
  #include "renderer_software.c"
#endif
