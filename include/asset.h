// asset.h

#define MESH_PATH "resource/mesh"
#define TEXTURE_PATH "resource/texture"
#define AUDIO_PATH "resource/audio"

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
  CUBEMAP_SKYBOX_DOOM = 0,
  CUBEMAP_SKYBOX_STORMYDAYS,

  MAX_CUBEMAP,
} cubemap_id;

typedef enum skybox_id {
  SKYBOX_DOOM = 0,

  MAX_SKYBOX,
} skybox_id;

typedef enum audio_id {
  AUDIO_TRACK_1,

  MAX_AUDIO,
} audio_id;

#define MAX_SKYBOX_TEXTURE (MAX_SKYBOX * 6)

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

// TODO(lucas): Implement an easier way of loading skyboxes!
static const char* SkyboxFileNames[MAX_SKYBOX_TEXTURE] = {
  "doom_rt",
  "doom_lf",
  "doom_up",
  "doom_dn",
  "doom_bk",
  "doom_ft",
};

static const char* AudioFileNames[MAX_AUDIO] = {
  "track-1",
};

typedef struct assets {
  mesh Meshes[MAX_MESH];
  u32 MeshCount;

  image Textures[MAX_TEXTURE];
  u32 TextureCount;

  image Skyboxes[MAX_SKYBOX_TEXTURE];
  u32 SkyboxCount;

  audio_source Audio[MAX_SKYBOX_TEXTURE];
  u32 AudioCount;
} assets;
