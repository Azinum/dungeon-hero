// renderer.c

#include "renderer.h"

#if RENDERER_OPENGL
  #include "renderer_opengl.c"
#else
  #include "renderer_software.c"
#endif
