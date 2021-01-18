// asset.h

#define MESH_PATH "resource/mesh"
#define TEXTURE_PATH "resource/texture"

typedef enum mesh_id {
  MESH_CUBE = 0,
  MESH_STONE,
  MESH_COOKING_POT,
  MESH_PLANE,
  MESH_MONSTER,
  MESH_TEA_POT,
  MESH_STALL,

  MAX_MESH,
} mesh_id;

typedef enum texture_id {
  TEXTURE_TEST = 0,
  TEXTURE_UV,
  TEXTURE_SUN_ICON,
  TEXTURE_BOX,
  TEXTURE_STALL,

  TEXTURE_MONSTER,

  MAX_TEXTURE,
} texture_id;

typedef enum cubemap_id {
  CUBEMAP_DEFAULT,

  MAX_CUBEMAP,
} cubemap_id;

static const char* MeshFileNames[MAX_MESH] = {
  "cube",
  "stone",
  "cooking_pot",
  "plane",

  "monster",
  "teapot",
  "stall",
};

static const char* TextureFileNames[MAX_TEXTURE] = {
  "test",
  "uv_test",
  "sun_icon",
  "box",
  "stall",

  "monster",
};

static const char* CubemapFileNames[MAX_TEXTURE] = {
  "pan_ares_lg",
};

typedef struct assets {
  mesh Meshes[MAX_MESH];
  u32 MeshCount;

  image Textures[MAX_TEXTURE];
  u32 TextureCount;

  image Cubemaps[MAX_CUBEMAP];
  u32 CubemapCount;
} assets;
