// game.c

#include "game.h"

#include "misc.c"
#include "image.c"
#include "mesh.c"
#include "window.c"
#include "software_renderer.c"
#include "entity.c"

static game_state GameState;
static v3 Light = V3(400, 350.0f, -150.0f);

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->Time = 0;
  Game->DeltaTime = 0;
  Game->EntityCount = 0;

#if 0
  for (u32 Index = 0; Index < 6; ++Index) {
    entity* Entity = GameAddEntity(V3(Index - 3.0f, Index - 4.0f, 10.0f));
    Entity->Speed = V2(0, 0.002f);
  }
#else
  for (i32 Y = 0; Y < 2; ++Y) {
    for (i32 X = -1; X < 4; ++X) {
      GameAddEntity(V3(X, Y, 10.0f));
    }
  }
#endif
}

static void OutputZBufferToFile(const char* Path) {
  image Image;
  Image.Width = Win.Width;
  Image.Height = Win.Height;
  Image.Depth = 24;
  Image.Pitch = Win.Width * 3;
  Image.PixelBuffer = malloc(4 * sizeof(u8) * Image.Width * Image.Height);
  Image.BytesPerPixel = 4;

  for (u32 Index = 0; Index < Image.Width * Image.Height; Index++) {
    float V = Clamp(255 * Abs(RenderState.ZBuffer[Index]), 0, 255);
    color Color = {V, V, V, 255};
    color* Pixel = (color*)&Image.PixelBuffer[Index * 4];
    *Pixel = Color;
  }
  StoreImage(Path, &Image);
  UnloadImage(&Image);
}

static void GameRun(game_state* Game) {
  mesh Mesh;
  MeshLoadOBJ("resource/mesh/stone.obj", &Mesh);
  image Texture;
  LoadImage("resource/texture/test.png", &Texture);

  u8 IsRunning = 1;
  u32 Tick = 0;
  while (IsRunning) {
    ++Tick;
    Light.X = (((RenderState.FrameBuffer.Width >> 1) * sin(Tick / 400.0f)) / 2.0f) + 400;

    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer, RenderState.ZBuffer, &Mesh, &Texture, Light);

    if (WindowEvents() != 0) {
      break;
    }

    RendererSwapBuffers();
    RendererClear(0, 0, 0);
  }

  OutputZBufferToFile("zbuffer.png");
  MeshUnload(&Mesh);
  UnloadImage(&Texture);
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
