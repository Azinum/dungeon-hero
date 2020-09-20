// renderer_opengl.c

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "renderer_opengl.h"

static render_state RenderState;
static i32 DefaultShader;

#define ERR_BUFFER_SIZE 512

static i32 ShaderCompile(const char* ShaderPath) {
  i32 Program = -1;
  u32 VertShader = 0;
  u32 FragShader = 0;

  char Path[MAX_PATH_SIZE] = {0};
  snprintf(Path, MAX_PATH_SIZE, "%s.vert", ShaderPath);
  buffer VertSource = {0};
  ReadFile(Path, &VertSource);
  snprintf(Path, MAX_PATH_SIZE, "%s.frag", ShaderPath);
  buffer FragSource = {0};
  ReadFile(Path, &FragSource);
  if (!VertSource.Data || !FragSource.Data)
    goto done;

  i32 Report = -1;
  char ErrorLog[ERR_BUFFER_SIZE] = {0};

  VertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertShader, 1, &VertSource, NULL);
  glCompileShader(VertShader);
{
  glGetShaderiv(VertShader, GL_COMPILE_STATUS, &Report);
  if (!Report) {
    glGetShaderInfoLog(VertShader, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "%s\n", ErrorLog);
    goto done;
  }
}

  FragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(FragShader, 1, &FragSource, NULL);
  glCompileShader(FragShader);
{
  glGetShaderiv(FragShader, GL_COMPILE_STATUS, &Report);
  if (!Report) {
    glGetShaderInfoLog(FragShader, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "%s\n", ErrorLog);
    goto done;
  }
}

  Program = glCreateProgram();
  glAttachShader(Program, VertShader);
  glAttachShader(Program, FragShader);
  glLinkProgram(Program);

{
  glGetProgramiv(Program, GL_VALIDATE_STATUS, &Report);
  if (Report != GL_NO_ERROR) {
    glGetProgramInfoLog(Program, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "%s\n", ErrorLog);
    goto done;
  }
}

done:
  if (VertShader > 0) {
    glDeleteShader(VertShader);
  }
  if (FragShader > 0) {
    glDeleteShader(FragShader);
  }
  BufferFree(VertSource.Data, VertSource.Count);
  BufferFree(FragSource.Data, FragSource.Count);
  return Program;
}

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
  DefaultShader = ShaderCompile("resource/shader/default");
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
