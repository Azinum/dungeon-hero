// game.h

#include "common.h"
#include "types.h"
#include "config.h"
#include "math_util.h"
#include "list.h"

#include "camera.h"
#include "misc.h"
#include "image.h"
#include "mesh.h"
#include "asset.h"
#include "window.h"
#include "entity.h"
#include "software_renderer.h"

#define MAX_ENTITY 256

typedef struct game_state {
  float Time;
  float DeltaTime;
  entity Entities[MAX_ENTITY];
  u32 EntityCount;
} game_state;

extern game_state GameState;

static entity* GameAddEntity(v3 Position, mesh_id MeshId, texture_id TextureId);

void GameStart();
