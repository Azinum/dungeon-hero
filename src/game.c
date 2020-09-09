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

  for (u32 Index = 0; Index < MAX_ENTITY; ++Index) {
    entity* Entity = GameAddEntity(V3(Random(0, 800), Random(0, 600), 0));
    if (!Entity) {
      break;
    }
    Entity->Speed = V2(Random(-.4, .4), Random(-.4, .4));
    Entity->P.Z = Random(-2, 2);
  }
}

static v3 Light = V3(0, 300, 0);

static void GameRun(game_state* Game) {
  mesh Mesh;
  MeshLoadOBJ("resource/mesh/test.obj", &Mesh);

  u8 IsRunning = 1;
  u32 Tick = 0;
  while (IsRunning) {
    if (WindowEvents() != 0) {
      break;
    }
    ++Tick;
    Light.X = (((RenderState.FrameBuffer.Width >> 1) * sin(Tick / 400.0f)) / 4.0f) + 400;

    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer, RenderState.ZBuffer, &Mesh, Light);

    RendererSwapBuffers();
    RendererClear(2, 15, 40);
  }
  MeshUnload(&Mesh);
}

static entity* GameAddEntity(v3 Position) {
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
