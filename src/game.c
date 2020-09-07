// game.c

#include "game.h"

#include "image.c"
#include "window.c"
#include "framebuffer.c"
#include "software_renderer.c"
#include "entity.c"

static game_state GameState;

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->EntityCount = 0;
}

static entity* GameAddEntity(v2 Position) {
  if (GameState.EntityCount >= MAX_ENTITY) {
    return NULL;
  }
  entity* Entity = &GameState.Entities[GameState.EntityCount++];
  EntityInit(Entity, Position);
  return Entity;
}

static void GameRun(game_state* Game) {
  for (u32 Index = 0; Index < 64; ++Index) {
    entity* Entity = GameAddEntity(V2(Random(0, 300), Random(0, 300)));
    if (!Entity) {
      break;
    }
    Entity->Speed = V2(Random(-.5, .5), Random(-.5, .5));
  }

  u8 IsRunning = 1;
  while (IsRunning) {
    if (WindowEvents() != 0) {
      break;
    }

    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer);
    RendererSwapBuffers();
    RendererClear();
  }
}

void GameStart() {
  game_state* Game = &GameState;
  GameStateInit(Game);
  RendererInit(WINDOW_WIDTH, WINDOW_HEIGHT);

  GameRun(Game);

  RendererDestroy();
}
