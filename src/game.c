// game.c

#include "game.h"

#include "misc.c"
#include "image.c"
#include "mesh.c"
#include "window.c"
#include "software_renderer.c"
#include "entity.c"

#define BUFFER_SIZE 512
#define MAX_DELTA_TIME 0.2f

game_state GameState;
static v3 Light = V3(400, 350.0f, -100.0f);

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->Time = 0;
  Game->DeltaTime = 0;
  Game->EntityCount = 0;

#if 1
  for (u32 Index = 0; Index < 5; ++Index) {
    entity* Entity = GameAddEntity(V3(Index - 2.0f, Index - 2.0f, 5.0f));
    Entity->Speed = V2(0, 1.0f);
  }
#else
  for (i32 Z = 0; Z < 10; ++Z) {
    for (i32 X = -2; X < 3; ++X) {
      GameAddEntity(V3(X, -2.5f, 12.0f + Z));
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
  MeshLoadOBJ("resource/mesh/test.obj", &Mesh);
  image Texture;
  LoadImage("resource/texture/test.png", &Texture);
  image SunTexture;
  LoadImage("resource/texture/sun-icon.png", &SunTexture);

  char Title[BUFFER_SIZE] = {0};
  struct timeval TimeNow = {0};
  struct timeval TimeLast = {0};
  float LastFrame = 0;

  u8 IsRunning = 1;
  u32 Tick = 0;
  while (IsRunning) {
    ++Tick;
    TimeLast = TimeNow;
    gettimeofday(&TimeNow, NULL);
    Game->DeltaTime = ((((TimeNow.tv_sec - TimeLast.tv_sec) * 1000000.0f) + TimeNow.tv_usec) - (TimeLast.tv_usec)) / 1000000.0f;
    if (Game->DeltaTime > MAX_DELTA_TIME) {
      Game->DeltaTime = MAX_DELTA_TIME;
    }
    Game->Time += Game->DeltaTime;
    LastFrame += Game->DeltaTime;

    Light.X = 400 + (200.0f * sin(Game->Time * PI32 * 0.25f));

    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer, RenderState.ZBuffer, &Mesh, &Texture, Light);

    DrawSimpleTexture2D(&RenderState.FrameBuffer, Light.X - 16, Light.Y - 16, 32, 32, &SunTexture, COLOR(1, 1, 0));
    DrawTexture2D(&RenderState.FrameBuffer, 5, 5, 64, 64, 0.0f, 0.0f, 0.25f, 0.25f, &Texture, COLOR(1, 1, 1));

    if (WindowEvents() != 0) {
      break;
    }

    if (LastFrame > (1.0f / TARGET_FPS)) {
      snprintf(Title, BUFFER_SIZE, "Software Renderer | fps: %i, dt: %g, last: %.3f ms", (i32)(1.0f / Game->DeltaTime), Game->DeltaTime, LastFrame);
      WindowSetTitle(Title);
      LastFrame -= (1.0f / TARGET_FPS);
      RendererSwapBuffers();
    }
    RendererClear(0, 0, 0);
  }

  OutputZBufferToFile("zbuffer.png");
  MeshUnload(&Mesh);
  UnloadImage(&Texture);
  UnloadImage(&SunTexture);
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
