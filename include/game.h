// game.h

#include "common.h"
#include "list.h"
#include "config.h"
#include "math_util.h"

#include "misc.h"
#include "image.h"
#include "mesh.h"
#include "audio.h"
#include "riff.h"
#include "asset.h"
#include "camera.h"
#include "renderer.h"
#include "platform.h"
#include "entity.h"

#define MAX_ENTITY 1024

typedef struct game_state {
  float Time;
  float DeltaTime;
  entity Entities[MAX_ENTITY];
  u32 EntityCount;
} game_state;

extern game_state GameState;
extern assets Assets;
extern double MouseX;
extern double MouseY;
extern v3 WorldMin;
extern v3 WorldMax;

static entity* GameAddEntity(v3 Position, mesh_id MeshId, texture_id TextureId);

void GameStart();
