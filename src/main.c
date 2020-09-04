// main.c
// Software Renderer

#include "software_renderer.c"

int main(void) {
  SoftwareRendererInit(800, 600);
  SoftwareRendererStart();
  return 0;
}
