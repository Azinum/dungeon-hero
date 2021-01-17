// renderer_opengl.h

typedef struct model {
  u32 DrawCount;
  u32 VAO;
  u32 VBO;
  u32 EBO;
} model;

#define MAX_MODEL MAX_MESH

typedef struct render_state {
  model Models[MAX_MODEL];
  u32 ModelCount;

  u32 Textures[MAX_TEXTURE];

  u32 TextureCount;
} render_state;

i32 RendererInit(render_state* RenderState, assets* Assets);

void RendererDestroy();
