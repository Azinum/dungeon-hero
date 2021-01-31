// renderer.h

// BGRa
typedef struct color {
  u8 B;
  u8 G;
  u8 R;
  u8 A;
} color;

typedef struct {
  float R;
  float G;
  float B;
} color32;

#define COLOR(R, G, B) (color) {B, G, R, .A = 255}
#define COLORA(R, G, B, ColorA) (color) {B, G, R, .A = ColorA}

#define COLOR32(ColorR, ColorG, ColorB) (color32) {.R = ColorR, .G = ColorG, .B = ColorB}

#if RENDERER_OPENGL
  #include "renderer_opengl.h"
#else
  #include "renderer_software.h"
#endif

static void RendererUpdateBuffers(struct render_state* RenderState);

static void DrawSkybox(struct render_state* RenderState, struct assets* Assets, struct camera* Camera, u32 TextureId);
