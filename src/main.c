// main.c
// Software Renderer

#include "software_renderer.c"

int main(void) {
  SoftwareRendererInit(WINDOW_WIDTH, WINDOW_HEIGHT);
  SoftwareRendererStart();
  return 0;
}
