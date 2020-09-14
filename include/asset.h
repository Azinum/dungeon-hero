// asset.h

#define MAX_PATH_SIZE 512

#define MESH_PATH "resource/mesh"
#define TEXTURE_PATH "resource/texture"

typedef enum mesh_id {
  MESH_CUBE = 0,
  MESH_STONE,

  MAX_MESH,
} mesh_id;

typedef enum texture_id {
  TEXTURE_TEST = 0,
  TEXTURE_UV,
  TEXTURE_SUN_ICON,

  MAX_TEXTURE,
} texture_id;

typedef struct assets {
  mesh Meshes[MAX_MESH];
  u32 MeshCount;

  image Textures[MAX_TEXTURE];
  u32 TextureCount;
} assets;
