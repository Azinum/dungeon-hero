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
static u32 DefaultShader;
static model DefaultModel;
static u32 DefaultTexture;

#define ERR_BUFFER_SIZE 512

static void OutputZBufferToFile(render_state* RenderState, const char* Path) {
  (void)RenderState;
  (void)Path;
}

static void OutputFrameBufferToFile(render_state* RenderState, const char* Path) {
  (void)RenderState;
  (void)Path;
}

static void StoreAttrib(model* Model, i32 AttribNum, u32 Count, u32 Size, void* Data) {
  glEnableVertexAttribArray(AttribNum);

  glGenBuffers(1, &Model->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, Model->VBO);
  glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);
  glVertexAttribPointer(AttribNum, Count, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static i32 UploadModel(model* Model, mesh* Mesh) {
  Model->DrawCount = Mesh->IndexCount;

  glGenVertexArrays(1, &Model->VAO);
  glBindVertexArray(Model->VAO);

  glGenBuffers(1, &Model->EBO);

  StoreAttrib(Model, 0, 3, Mesh->VertexCount * sizeof(float) * 3, &Mesh->Vertices[0]);
  StoreAttrib(Model, 1, 2, Mesh->UVCount * sizeof(float) * 2, &Mesh->UV[0]);
  StoreAttrib(Model, 2, 3, Mesh->NormalCount * sizeof(float) * 3, &Mesh->Normals[0]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Model->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh->IndexCount * sizeof(u32), &Mesh->Indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
  return 0;
}

static i32 UploadAndIndexModel(model* Model, mesh* Mesh) {
  i32 IndexResult = MeshSortIndexedData(Mesh);
  if (IndexResult != 0) {
    return IndexResult;
  }
  return UploadModel(Model, Mesh);
}

static i32 UploadTexture(u32* TextureId, image* Texture) {
  glGenTextures(1, TextureId);
  glBindTexture(GL_TEXTURE_2D, *TextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->PixelBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
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
  u32 TextureId = DefaultTexture;

  Model = Translate(P);
  Model = MultiplyMat4(Model, Rotate(Rotation, V3(0, 1, 0)));
  Model = MultiplyMat4(Model, Scale(Scaling));

  View = LookAt(Camera->P, AddToV3(Camera->P, Camera->Forward), Camera->Up);
  // View = InverseMat4(View);

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);
  glUniform3f(glGetUniformLocation(Handle, "Light"), Light.X, Light.Y, Light.Z);

  model* Model = &DefaultModel;

  glBindVertexArray(Model->VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, TextureId);

  glDrawElements(GL_TRIANGLES, Model->DrawCount, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  glBindVertexArray(0);
  glUseProgram(0);
}

static void OpenGLInit() {
  GLint Attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  Win.VisualInfo = glXChooseVisual(Win.Disp, 0, Attribs);
  RenderState.Context = glXCreateContext(Win.Disp, Win.VisualInfo, NULL, GL_TRUE);
  glXMakeCurrent(Win.Disp, Win.Win, RenderState.Context);

  RenderState.glSwapIntervalEXT = (glSwapInterval_t*)glXGetProcAddress((u8*)"glXSwapIntervalEXT");
  if (RenderState.glSwapIntervalEXT) {
    RenderState.glSwapIntervalEXT(Win.Disp, Win.Win, 0);
  }
  else {
    fprintf(stderr, "Failed to disable vsync\n");
  }

  i32 GlewError = glewInit();
  if (GlewError != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(GlewError));
    return;
  }

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  fprintf(stdout, "GL VENDOR:    %s\n", glGetString(GL_VENDOR));
  fprintf(stdout, "GL RENDERER:  %s\n", glGetString(GL_RENDERER));
  fprintf(stdout, "GL VERSION:   %s\n", glGetString(GL_VERSION));
  fprintf(stdout, "GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

v3 Vertices[] = {
  V3(-0.5, -0.5, -0.5),
  V3(-0.5, 0.5, -0.5),
  V3(0.5, 0.5, -0.5),
  V3(0.5, -0.5, -0.5),

  V3(-0.5, -0.5, 0.5),
  V3(-0.5, 0.5, 0.5),
  V3(0.5, 0.5, 0.5),
  V3(0.5, -0.5, 0.5),

  V3(-0.5, -0.5, -0.5),
  V3(-0.5, -0.5, 0.5),
  V3(0.5, -0.5, 0.5),
  V3(0.5, -0.5, -0.5),

  V3(-0.5, 0.5, -0.5),
  V3(-0.5, 0.5, 0.5),
  V3(0.5, 0.5, 0.5),
  V3(0.5, 0.5, -0.5),

  V3(-0.5, -0.5, -0.5),
  V3(-0.5, -0.5, 0.5),
  V3(-0.5, 0.5, 0.5),
  V3(-0.5, 0.5, -0.5),

  V3(0.5, -0.5, -0.5),
  V3(0.5, -0.5, 0.5),
  V3(0.5, 0.5, 0.5),
  V3(0.5, 0.5, -0.5),
};

v2 UV[] = {
  V2(1, 0), 
  V2(0, 0), 
  V2(0, 1), 
  V2(1, 1), 
            
  V2(1, 0), 
  V2(0, 0), 
  V2(0, 1), 
  V2(1, 1), 
            
  V2(0, 1), 
  V2(1, 1), 
  V2(1, 0), 
  V2(0, 0), 
            
  V2(0, 1), 
  V2(1, 1), 
  V2(1, 0), 
  V2(0, 0), 
            
  V2(1, 1), 
  V2(1, 0), 
  V2(0, 0), 
  V2(0, 1), 
            
  V2(1, 1), 
  V2(1, 0), 
  V2(0, 0), 
  V2(0, 1), 
};

v3 Normals[] = {
  V3(0, 0, -1),
  V3(0, 0, -1),
  V3(0, 0, -1),
  V3(0, 0, -1),
               
  V3(0, 0, 1),
  V3(0, 0, 1),
  V3(0, 0, 1),
  V3(0, 0, 1),
               
  V3(0, -1, 0),
  V3(0, -1, 0),
  V3(0, -1, 0),
  V3(0, -1, 0),
               
  V3(0, 1, 0),
  V3(0, 1, 0),
  V3(0, 1, 0),
  V3(0, 1, 0),
               
  V3(-1, 0, 0),
  V3(-1, 0, 0),
  V3(-1, 0, 0),
  V3(-1, 0, 0),
               
  V3(1, 0, 0),
  V3(1, 0, 0),
  V3(1, 0, 0),
  V3(1, 0, 0),
};

static u32 Indices[] = {
  0, 1, 2,
  0, 2, 3,

  6, 5, 4,
  7, 6, 4,

  10, 9, 8,
  11, 10, 8,

  12, 13, 14,
  12, 14, 15,

  16, 17, 18,
  16, 18, 19,

  22, 21, 20,
  23, 22, 20
};

i32 RendererInit(render_state* RenderState, assets* Assets) {
  (void)RenderState;
  OpenGLInit();
  DefaultShader = ShaderCompile("resource/shader/default");

#if 1
  mesh Mesh;
  Mesh.Indices = Indices;
  Mesh.IndexCount = ARR_SIZE(Indices);
  Mesh.Vertices = Vertices;
  Mesh.VertexCount = ARR_SIZE(Vertices);
  Mesh.UV = UV;
  Mesh.UVCount = ARR_SIZE(UV);
  Mesh.Normals = Normals;
  Mesh.NormalCount = ARR_SIZE(Normals);

  UploadModel(&DefaultModel, &Mesh);
#else
  mesh* Mesh = &Assets->Meshes[MESH_CUBE];
  UploadAndIndexModel(&DefaultModel, Mesh);
#endif

  UploadTexture(&DefaultTexture, &Assets->Textures[TEXTURE_TEST]);
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
