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

static v2 Light = V2(0, 350);
static float LightStrength = 20000;

#define SCALING (128)

static void GameDrawMesh(framebuffer* FrameBuffer, mesh* Mesh, v2 P) {
  for (u32 Index = 0; Index < Mesh->IndexCount; Index += 3) {
    v3 V[3];
    v2 A[3];

    V[0] = Mesh->Vertices[Mesh->Indices[Index + 0]];
    V[1] = Mesh->Vertices[Mesh->Indices[Index + 1]];
    V[2] = Mesh->Vertices[Mesh->Indices[Index + 2]];
    A[0] = V2(V[0].X * SCALING, V[0].Y * SCALING);
    A[1] = V2(V[1].X * SCALING, V[1].Y * SCALING);
    A[2] = V2(V[2].X * SCALING, V[2].Y * SCALING);

    float LightFactorX = 1.0f / (Abs(Light.X - A[0].X));
    float LightFactorY = 1.0f / (Abs(Light.Y - A[0].Y));
    float LightFactor = Clamp(LightFactorX * LightFactorY * LightStrength, 1.0f);
    Assert(LightFactor <= 1.0f);

    DrawFilledTriangleAt(FrameBuffer, A[0], A[1], A[2], P,
      255 * LightFactor,
      30 * LightFactor,
      40* LightFactor
    );
  }
}

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
    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer);

    Light.X = (RenderState.FrameBuffer.Width >> 2) + ((RenderState.FrameBuffer.Width >> 1) * sin(Tick / 500.0f));
    GameDrawMesh(&RenderState.FrameBuffer, &Mesh, V2(350, 250));

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
