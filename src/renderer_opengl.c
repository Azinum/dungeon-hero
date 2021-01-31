// renderer_opengl.c

static render_state RenderState;
static u32 DefaultShader;
static u32 SkyboxShader;
static u32 TextureShader;

static model CubeModel;
static u32 QuadVBO = 0, QuadVAO = 0;

#define SKYBOX_SIZE 1.0f

float SkyboxVertices[] = {
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,

	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,

	 SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,

	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,

	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,

	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	 SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE
};

static float QuadVertices[] = {
  // Vertex,  UV coord
  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f,

  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
};

#define ERR_BUFFER_SIZE 512

static void OutputZBufferToFile(render_state* RenderState, const char* Path) {
  (void)RenderState;
  (void)Path;
}

static void OutputFrameBufferToFile(render_state* RenderState, const char* Path) {
  (void)RenderState;
  (void)Path;
}

static void InitQuadData() {
  glGenVertexArrays(1, &QuadVAO);
  glGenBuffers(1, &QuadVBO);

  glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

  glBindVertexArray(QuadVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
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

static i32 UploadModel2(model* Model, float* Vertices, u32 VertexCount) {
  Model->DrawCount = VertexCount / 3;

  glGenVertexArrays(1, &Model->VAO);
  glBindVertexArray(Model->VAO);

  glGenBuffers(1, &Model->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, Model->VBO);
  glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(*Vertices), Vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(0, 3, GL_FLOAT, 3 * sizeof(float), GL_FALSE, (void*)0);

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

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->PixelBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
  return 0;
}

static i32 UploadCubemapTexture(u32* TextureId, image* Texture) {
  glGenTextures(1, TextureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, *TextureId);

  for (i32 Index = 0; Index < 6; ++Index) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, 0, GL_RGBA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->PixelBuffer);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return 0;
}

// TODO(lucas): The top and bottom textures seem to be rotated wrong, is this an issue with the cubemapping or is it the skyboxes (textures) themselves? Fix!
static i32 UploadSkyboxTexture(u32* TextureId, u32 SkyboxId, assets* Assets) {
  glGenTextures(1, TextureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, *TextureId);

  for (i32 Index = 0; Index < 6; ++Index) {
    image* Texture = &Assets->Skyboxes[SkyboxId + Index];
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, 0, GL_RGBA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->PixelBuffer);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return 0;
}

static i32 ShaderCompile(const char* ShaderPath) {
  i32 Program = -1;
  u32 VertShader = 0;
  u32 FragShader = 0;

  char VertPath[MAX_PATH_SIZE] = {0};
  snprintf(VertPath, MAX_PATH_SIZE, "%s.vert", ShaderPath);
  const char* VertSource = ReadFileAndNullTerminate(VertPath);
  char FragPath[MAX_PATH_SIZE] = {0};
  snprintf(FragPath, MAX_PATH_SIZE, "%s.frag", ShaderPath);
  const char* FragSource = ReadFileAndNullTerminate(FragPath);
  if (!VertSource || !FragSource) {
    fprintf(stderr, "Failed to open shader file(s)\n");
    goto done;
  }

  i32 Report = -1;
  char ErrorLog[ERR_BUFFER_SIZE] = {0};

  VertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertShader, 1, &VertSource, NULL);
  glCompileShader(VertShader);
{
  glGetShaderiv(VertShader, GL_COMPILE_STATUS, &Report);
  if (!Report) {
    glGetShaderInfoLog(VertShader, ERR_BUFFER_SIZE, NULL, ErrorLog);
    fprintf(stderr, "%s:%s\n", VertPath, ErrorLog);
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
    fprintf(stderr, "%s:%s\n", FragPath, ErrorLog);
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
    // fprintf(stderr, "%s:%s\n", ShaderPath, ErrorLog);
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

#define DrawSimpleTexture2D(RenderState, X, Y, Z, W, H, TEXTURE, TINT) \
  DrawTexture2D(RenderState, X, Y, Z, W, H, 0, 0, 1, 1, TEXTURE, TINT)

static void DrawTexture2D(render_state* RenderState, assets* Assets, i32 X, i32 Y, float Z, i32 W, i32 H, float XOffset, float YOffset, float XRange, float YRange, u32 TextureId, color Tint) {
  u32 Handle = TextureShader;
  glUseProgram(Handle);

  image* Texture = &Assets->Textures[TextureId];
  u32 Id = RenderState->Textures[TextureId];

  Model = Translate(V3(X, Y, Z));

  Model = Translate2D(Model, 0.5f * W, 0.5f * H);
  Model = Rotate2D(Model, 0);
  Model = Translate2D(Model, -0.5f * W, -0.5f * H);

  Model = Scale2D(Model, W, H);

  mat4 View = Mat4D(1.0f);

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&OrthoProjection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);

  // glUniform2f(glGetUniformLocation(Handle, "Offset"), (float)XOffset / Texture->Width, (float)YOffset / Texture->Height);  // Use when drawing from a texture atlas
  // glUniform2f(glGetUniformLocation(Handle, "Range"), (float)XRange / Texture->Width, (float)YRange / Texture->Height);     // Use when drawing from a texture atlas
  glUniform2f(glGetUniformLocation(Handle, "Offset"), (float)XOffset, (float)YOffset);
  glUniform2f(glGetUniformLocation(Handle, "Range"), (float)XRange, (float)YRange);
  glUniform4f(glGetUniformLocation(Handle, "Tint"), Tint.R, Tint.G, Tint.B, Tint.A);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Id);

  glBindVertexArray(QuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

static void DrawMesh(render_state* RenderState, assets* Assets, u32 MeshId, u32 TextureId, v3 P, v3 Light, float LightStrength, v3 Rotation, v3 Scaling, camera* Camera) {
  (void)Assets;
  u32 Handle = DefaultShader;
  glUseProgram(Handle);
  u32 Texture = RenderState->Textures[TextureId];

  Model = Translate(P);

  Model = MultiplyMat4(Model, Rotate(Rotation.Y, V3(0, 1, 0)));
  Model = MultiplyMat4(Model, Rotate(Rotation.Z, V3(0, 0, 1)));
  Model = MultiplyMat4(Model, Rotate(Rotation.X, V3(1, 0, 0)));

  Model = MultiplyMat4(Model, Scale(Scaling));

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&View);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "Model"), 1, GL_FALSE, (float*)&Model);
  glUniform3f(glGetUniformLocation(Handle, "Light"), Light.X, Light.Y, Light.Z);
  glUniform1f(glGetUniformLocation(Handle, "LightStrength"), LightStrength);

  model* Model = &RenderState->Models[MeshId];

  glBindVertexArray(Model->VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Texture);

  glDrawElements(GL_TRIANGLES, Model->DrawCount, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  glBindVertexArray(0);
  glUseProgram(0);
}

static void OpenGLInit() {
  PlatformOpenGLInit();

  i32 GlewError = glewInit();
  if (GlewError != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(GlewError));
    return;
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_FLAT);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glAlphaFunc(GL_GREATER, 1);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_TEXTURE_CUBE_MAP_EXT);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_NORMALIZE);

  fprintf(stdout, "GL VENDOR:    %s\n", glGetString(GL_VENDOR));
  fprintf(stdout, "GL RENDERER:  %s\n", glGetString(GL_RENDERER));
  fprintf(stdout, "GL VERSION:   %s\n", glGetString(GL_VERSION));
  fprintf(stdout, "GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

i32 RendererInit(render_state* RenderState, assets* Assets) {
  OpenGLInit();
  InitQuadData();
  RenderState->ModelCount = 0;
  RenderState->TextureCount = 0;

  if (G_CompatibleOpenGL) {
    DefaultShader = ShaderCompile("resource/shader/compat_default");
  }
  else {
    DefaultShader = ShaderCompile("resource/shader/default");
  }

  SkyboxShader = ShaderCompile("resource/shader/skybox");

  TextureShader = ShaderCompile("resource/shader/texture");

  if (DefaultShader < 0 || SkyboxShader < 0 || TextureShader < 0) {
    fprintf(stderr, "Failed to load shader(s)\n");
    return -1;
  }

  // TODO(lucas): Figure out and explore how asset/resource loading and unloading should work
  // when it comes to using different renderers. Should we be able to switch rendering context, i.e. switch from software to hardware rendering, in run-time?
  // How will we handle resources in that case?
  for (u32 Index = 0; Index < Assets->MeshCount; ++Index) {
    mesh* Mesh = &Assets->Meshes[Index];
    model Model;
    UploadAndIndexModel(&Model, Mesh);
    RenderState->Models[Index] = Model;
    RenderState->ModelCount++;
  }
  for (u32 Index = 0; Index < Assets->TextureCount; ++Index) {
    image* Texture = &Assets->Textures[Index];
    u32 TextureId = 0;
    UploadTexture(&TextureId, Texture);
    RenderState->Textures[Index] = TextureId;
    RenderState->TextureCount++;
  }
  for (u32 Index = 0; Index < Assets->SkyboxCount / 6; ++Index) {
    u32 TextureId = 0;
    UploadSkyboxTexture(&TextureId, Index * 6, Assets);
    RenderState->Cubemaps[Index] = TextureId;
    RenderState->CubemapCount++;
  }
	UploadModel2(&CubeModel, SkyboxVertices, ARR_SIZE(SkyboxVertices));
  return 0;
}

static void RendererSwapBuffers() {
  WindowSwapBuffers(&RenderState);
}

static void RendererClear(u8 ColorR, u8 ColorG, u8 ColorB) {
  glClearColor(ColorR / 255.0f, ColorG / 255.0f, ColorB / 255.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void RendererUpdateBuffers(render_state* RenderState) {
  (void)RenderState;
  // TODO(lucas): Implement
}

static void DrawSkybox(render_state* RenderState, assets* Assets, camera* Camera, u32 TextureId) {
  (void)Assets;
  u32 Texture = RenderState->Cubemaps[TextureId];
  model* Model = &CubeModel;
  u32 Handle = SkyboxShader;
  glUseProgram(Handle);

  glDepthFunc(GL_LEQUAL);

  mat4 ViewMatrix = View;
  // NOTE(lucas): We translate the view matrix to the origin here, this is so that we don't move outside the skybox
  ViewMatrix.Elements[3][0] = 0;
  ViewMatrix.Elements[3][1] = 0;
  ViewMatrix.Elements[3][2] = 0;

  glUniformMatrix4fv(glGetUniformLocation(Handle, "Projection"), 1, GL_FALSE, (float*)&Projection);
  glUniformMatrix4fv(glGetUniformLocation(Handle, "View"), 1, GL_FALSE, (float*)&ViewMatrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);

  glBindVertexArray(Model->VAO);
  glDrawArrays(GL_TRIANGLES, 0, Model->DrawCount);
  glBindVertexArray(0);

  glDepthFunc(GL_LESS);
  glUseProgram(0);
}

void RendererDestroy(render_state* RenderState) {
  glDeleteVertexArrays(1, &CubeModel.VAO);
  glDeleteVertexArrays(1, &CubeModel.VBO);
  CubeModel.DrawCount = 0;

  glDeleteVertexArrays(1, &QuadVAO);
  glDeleteVertexArrays(1, &QuadVBO);

  for (u32 Index = 0; Index < RenderState->ModelCount; ++Index) {
    model* Model = &RenderState->Models[Index];
    glDeleteVertexArrays(1, &Model->VAO);
    glDeleteVertexArrays(1, &Model->VBO);
    glDeleteBuffers(1, &Model->EBO);
  }
  RenderState->ModelCount = 0;

  for (u32 Index = 0; Index < RenderState->TextureCount; ++Index) {
    u32 TextureId = RenderState->Textures[Index];
    glDeleteTextures(1, &TextureId);
  }
  RenderState->TextureCount = 0;

  for (u32 Index = 0; Index < RenderState->CubemapCount; ++Index) {
    u32 TextureId = RenderState->Cubemaps[Index];
    glDeleteTextures(1, &TextureId);
  }
  RenderState->CubemapCount = 0;

  glDeleteShader(DefaultShader);
  glDeleteShader(SkyboxShader);
  glDeleteShader(TextureShader);
}
