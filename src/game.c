// game.c

#include "game.h"

#include "image.c"
#include "mesh.c"
#include "framebuffer.c"
#include "window.c"
#include "software_renderer.c"
#include "entity.c"

static game_state GameState;

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->EntityCount = 0;

  for (u32 Index = 0; Index < 64; ++Index) {
    entity* Entity = GameAddEntity(V2(Random(0, 300), Random(0, 300)));
    if (!Entity) {
      break;
    }
    Entity->Speed = V2(Random(-.1, .1), Random(-.1, .1));
  }
}

static void GameRun(game_state* Game) {
  mesh Mesh;
  MeshLoadOBJ("resource/mesh/icosphere.obj", &Mesh);

  u8 IsRunning = 1;
  while (IsRunning) {
    if (WindowEvents() != 0) {
      break;
    }
    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer);

    RendererSwapBuffers();
    RendererClear();
  }
  MeshUnload(&Mesh);
}

static entity* GameAddEntity(v2 Position) {
  if (GameState.EntityCount >= MAX_ENTITY) {
    return NULL;
  }
  entity* Entity = &GameState.Entities[GameState.EntityCount++];
  EntityInit(Entity, Position);
  return Entity;
}


void GameStart() {
  game_state* Game = &GameState;
  GameStateInit(Game);

  RendererInit(WINDOW_WIDTH, WINDOW_HEIGHT);

  GameRun(Game);

  RendererDestroy();
}
