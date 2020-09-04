// main.c
// Software Renderer

#include "software_renderer.c"

int main(void) {
  SoftwareRendererInit(512, 512);
  SoftwareRendererStart();
  return 0;
}
