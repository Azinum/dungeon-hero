// game.h

#include "common.h"
#include "config.h"

#include "image.h"
#include "framebuffer.h"
#include "software_renderer.h"
#include "window.h"
#include "entity.h"

#define MAX_ENTITY 256

typedef struct game_state {
  entity Entities[MAX_ENTITY];
  u32 EntityCount;
} game_state;

typedef struct triangle {
  v2 A, B, C;
} triangle;

void GameStart();
