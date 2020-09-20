// renderer_opengl.c

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "renderer_opengl.h"

typedef struct model {
  u32 DrawCount;
  u32 VAO;
  u32 VBO;
  u32 EBO;
} model;

static render_state RenderState;
static i32 DefaultShader;
static model DefaultModel;

#define ERR_BUFFER_SIZE 512

i32 UploadModel(model* Model, mesh* Mesh) {
  Model->DrawCount = Mesh->IndexCount;

  glGenVertexArrays(1, &Model->VAO);
  glBindVertexArray(Model->VAO);

  // NOTE(lucas): Bind vertices
  glGenBuffers(1, &Model->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, Model->VBO);
  glBufferData(GL_ARRAY_BUFFER, Mesh->VertexCount * sizeof(float) * 3, &Mesh->Vertices[0], GL_STATIC_DRAW);
  i32 AttribNum = 0;
  glEnableVertexAttribArray(AttribNum);
  glVertexAttribPointer(AttribNum, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // NOTE(lucas): Bind indices
  glGenBuffers(1, &Model->EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Model->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh->IndexCount * sizeof(u32), &Mesh->Indices[0], GL_STATIC_DRAW);
  return 0;
}

static i32 ShaderCompile(const char* ShaderPath) {
  i32 Program = -1;
  u32 VertShader = 0;
  u32 FragShader = 0;

  char Path[MAX_PATH_SIZE] = {0};
  snprintf(Path, MAX_PATH_SIZE, "%s.vert", ShaderPath);
  const char* VertSource = ReadFileAndNullTerminate(Path);
  snprintf(Path, MAX_PATH_SIZE, "%s.frag", ShaderPath);
  const char* FragSource = ReadFileAndNullTerminate(Path);
  if (!VertSource || !FragSource)
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
    fprintf(stderr, "%s", ErrorLog);
    goto done;
  }
}

done:
  if (VertShader > 0) glDeleteShader(VertShader);
  if (FragShader > 0) glDeleteShader(FragShader);
  if (VertSource)     free((void*)VertSource);
  if (FragSource)     free((void*)FragSource);
  return Program;
}

#define DrawSimpleTexture2D(RenderState, X, Y, W, H, TEXTURE, TINT) \
  DrawTexture2D(RenderState, X, Y, W, H, 0, 0, 1, 1, TEXTURE, TINT)

inline void DrawTexture2D(render_state* RenderState, i32 X, i32 Y, i32 W, i32 H, float XOffset, float YOffset, float XRange, float YRange, image* Texture, color Tint) {

}

static void DrawMesh(render_state* RenderState, mesh* Mesh, image* Texture, v3 P, v3 Light, float Rotation, v3 Scaling, camera* Camera) {
  u32 Handle = DefaultShader;
  glUseProgram(Handle);

  Model = Translate(P);
  Model = MultiplyMat4(Model, Rotate(Rotation, V3(0, 1, 0)));
  Model = MultiplyMat4(Model, Scale(Scaling));

  View = LookAt(Camera->P, AddToV3(Camera->P, Camera->Forward), Camera->Up);
  View = InverseMat4(View);

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);

  model* Model = &DefaultModel;

  glBindVertexArray(Model->VAO);
  glEnableVertexAttribArray(0);
  glDrawElements(GL_TRIANGLES, Model->DrawCount, GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);

  glUseProgram(0);
}

static void OpenGLInit() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLint Attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  Win.VisualInfo = glXChooseVisual(Win.Disp, 0, Attribs);
  RenderState.Context = glXCreateContext(Win.Disp, Win.VisualInfo, NULL, GL_TRUE);
  glXMakeCurrent(Win.Disp, Win.Win, RenderState.Context);

  i32 GlewError = glewInit();
  if (GlewError != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(GlewError));
    return;
  }

  fprintf(stdout, "GL VENDOR:    %s\n", glGetString(GL_VENDOR));
  fprintf(stdout, "GL RENDERER:  %s\n", glGetString(GL_RENDERER));
  fprintf(stdout, "GL VERSION:   %s\n", glGetString(GL_VERSION));
  fprintf(stdout, "GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

i32 RendererInit(assets* Assets) {
  OpenGLInit();
  DefaultShader = ShaderCompile("resource/shader/default");
  UploadModel(&DefaultModel, &Assets->Meshes[MESH_CUBE]);
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
  // TODO(lucas): Temporary {
  glDeleteVertexArrays(1, &DefaultModel.VAO);
  glDeleteVertexArrays(1, &DefaultModel.VBO);
  glDeleteBuffers(1, &DefaultModel.EBO);
  // }
  glDeleteShader(DefaultShader);
}
