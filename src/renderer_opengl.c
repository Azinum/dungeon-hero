// renderer_opengl.c

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "renderer_opengl.h"

static render_state RenderState;

#define DrawSimpleTexture2D(RenderState, X, Y, W, H, TEXTURE, TINT) \
  DrawTexture2D(RenderState, X, Y, W, H, 0, 0, 1, 1, TEXTURE, TINT)

inline void DrawTexture2D(render_state* RenderState, i32 X, i32 Y, i32 W, i32 H, float XOffset, float YOffset, float XRange, float YRange, image* Texture, color Tint) {

}

static void DrawMesh(render_state* RenderState, mesh* Mesh, image* Texture, v3 P, v3 Light, float Rotation, v3 Scaling, camera* Camera) {

}

static void OpenGLInit() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLint Attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  Win.VisualInfo = glXChooseVisual(Win.Disp, 0, Attribs);
  RenderState.Context = glXCreateContext(Win.Disp, Win.VisualInfo, NULL, GL_TRUE);
  glXMakeCurrent(Win.Disp, Win.Win, RenderState.Context);
}

i32 RendererInit() {
  OpenGLInit();
  return 0;
}

static void RendererSwapBuffers() {
  glXSwapBuffers(Win.Disp, Win.Win);
}

static void RendererClear(u8 ColorR, u8 ColorG, u8 ColorB) {
  glClearColor(ColorR / 255.0f, ColorG / 255.0f, ColorB / 255.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererDestroy() {

}
