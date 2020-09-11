// game.h

#include "common.h"
#include "types.h"
#include "config.h"
#include "math_util.h"
#include "list.h"

#include "misc.h"
#include "image.h"
#include "mesh.h"
#include "framebuffer.h"
#include "window.h"
#include "software_renderer.h"
#include "entity.h"

#define MAX_ENTITY 5

typedef struct game_state {
  entity Entities[MAX_ENTITY];
  u32 EntityCount;
} game_state;

static entity* GameAddEntity(v3 Position);

void GameStart();
