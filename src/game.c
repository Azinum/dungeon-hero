// game.c

#include "game.h"

#include "image.c"
#include "mesh.c"
#include "framebuffer.c"
#include "window.c"
#include "software_renderer.c"
#include "entity.c"

static game_state GameState;
static v3 Light = V3(0, 300, -25);

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->EntityCount = 0;

  for (u32 Index = 0; Index < MAX_ENTITY; ++Index) {
    entity* Entity = GameAddEntity(V3(-6.0f + ((1 + Index) * 2.0f), (Index * 2.0f), 20.0f)); // GameAddEntity(V3(Random(-15, 15), Random(-15, 15), 20));
    if (!Entity) {
      break;
    }
    Entity->Speed = V2(0, 0.005f);
  }
}

static void OutputZBufferToFile() {
  image Image;
  Image.Width = Win.Width;
  Image.Height = Win.Height;
  Image.Depth = 24;
  Image.Pitch = Win.Width * 3;
  Image.PixelBuffer = malloc(4 * sizeof(u8) * Image.Width * Image.Height);
  Image.BytesPerPixel = 4;

  for (u32 Index = 0; Index < Image.Width * Image.Height; Index++) {
    i32 V = Abs(RenderState.ZBuffer[Index]);
    color Color = {V, V, V, 255};
    color* Pixel = (color*)&Image.PixelBuffer[Index * 4];
    *Pixel = Color;
  }
  StorePNGFromSource("zbuffer.png", &Image);
  free(Image.PixelBuffer);
}

static void GameRun(game_state* Game) {
  mesh Mesh;
  MeshLoadOBJ("resource/mesh/test.obj", &Mesh);

  u8 IsRunning = 1;
  u32 Tick = 0;
  while (IsRunning) {
    ++Tick;
    Light.X = (((RenderState.FrameBuffer.Width >> 1) * sin(Tick / 400.0f)) / 2.0f) + 400;

    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer, RenderState.ZBuffer, &Mesh, Light);

    if (WindowEvents() != 0) {
      break;
    }

    RendererSwapBuffers();
    RendererClear(2, 15, 40);
  }
  OutputZBufferToFile();
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
  srand(time(NULL));
  game_state* Game = &GameState;
  GameStateInit(Game);

  RendererInit(WINDOW_WIDTH, WINDOW_HEIGHT);

  GameRun(Game);

  RendererDestroy();
}
