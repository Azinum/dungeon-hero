// game.c

#include "game.h"

#include "misc.c"
#include "image.c"
#include "mesh.c"
#include "asset.c"
#include "window.c"
#include "camera.c"
#include "software_renderer.c"
#include "entity.c"

#define BUFFER_SIZE 512
#define MAX_DELTA_TIME 0.5f

game_state GameState;
// static v3 Light = V3(400, 350.0f, -50.0f);
static v3 Light = V3(0.0f, 1.1f, 0.0f);

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->Time = 0;
  Game->DeltaTime = 0;
  Game->EntityCount = 0;

#if 0
  for (u32 Index = 0; Index < 5; ++Index) {
    entity* Entity = GameAddEntity(V3(Index - 2.0f, Index - 2.0f, 5.0f), rand() % MAX_MESH, TEXTURE_TEST);
    Entity->Speed = V3(0, 1.0f, 0);
  }
#endif

#if 1
  for (i32 Z = 4; Z < 10; ++Z) {
    for (i32 X = -4; X <= 4; ++X) {
      if (!(rand() % 16)) {
        entity* E = GameAddEntity(V3(X, 0, Z), MESH_COOKING_POT, TEXTURE_UV);
        E->Type = ENTITY_ROTATOR;
      }
      if (!(rand() % 16)) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_BOX);
      }
      if (!(rand() % 20)) {
        GameAddEntity(V3(X, 0, Z), MESH_STONE, TEXTURE_TEST);
      }

      GameAddEntity(V3(X, -1, Z), MESH_PLANE, TEXTURE_TEST);
    }
  }
#endif

#if 0
  GameAddEntity(V3(0, 0, -1), MESH_CUBE, TEXTURE_BOX);
#endif
  CameraInit(&Camera, V3(0, 0, 0));
}

static void OutputZBufferToFile(const char* Path) {
  image Image;
  Image.Width = Win.Width;
  Image.Height = Win.Height;
  Image.Depth = 24;
  Image.Pitch = Win.Width * 4;
  Image.PixelBuffer = malloc(4 * sizeof(u8) * Image.Width * Image.Height);
  Image.BytesPerPixel = 4;

  for (u32 Index = 0; Index < Image.Width * Image.Height; ++Index) {
    float V = Clamp(255 * Abs(RenderState.ZBuffer[Index]), 0, 255);
    color Color = {V, V, V, 255};
    color* Pixel = (color*)&Image.PixelBuffer[Index * 4];
    *Pixel = Color;
  }
  StoreImage(Path, &Image);
  UnloadImage(&Image);
}

static void OutputFrameBufferToFile(framebuffer* FrameBuffer, const char* Path) {
  image Image;
  Image.Width = FrameBuffer->Width;
  Image.Height = FrameBuffer->Height;
  Image.Depth = 24;
  Image.Pitch = FrameBuffer->Width * 4;
  Image.PixelBuffer = malloc(4 * sizeof(u8) * Image.Width * Image.Height);
  Image.BytesPerPixel = 4;

  for (u32 Index = 0; Index < FrameBuffer->Width * FrameBuffer->Height; ++Index) {
    color Pixel = BGRToRGB(FrameBuffer->Color[Index]);
    *(color*)&Image.PixelBuffer[Index * 4] = Pixel;
  }
  StoreImage(Path, &Image);
  UnloadImage(&Image);
}

static void GameRun(game_state* Game) {
  assets Assets;
  AssetsLoadAll(&Assets);

  image SunTexture = Assets.Textures[TEXTURE_SUN_ICON];

  char Title[BUFFER_SIZE] = {0};
  struct timeval TimeNow = {0};
  struct timeval TimeLast = {0};
  float LastFrame = 0;

  u8 IsRunning = 1;
  u32 Tick = 0;
  while (IsRunning && !WindowEvents()) {
    ++Tick;
    TimeLast = TimeNow;
    gettimeofday(&TimeNow, NULL);
    Game->DeltaTime = ((((TimeNow.tv_sec - TimeLast.tv_sec) * 1000000.0f) + TimeNow.tv_usec) - (TimeLast.tv_usec)) / 1000000.0f;
    if (Game->DeltaTime > MAX_DELTA_TIME) {
      Game->DeltaTime = MAX_DELTA_TIME;
    }
    Game->Time += Game->DeltaTime;
    LastFrame += Game->DeltaTime;

    // Light.X = 400 + (100.0f * sin(Game->Time * PI32 * 0.25f));
    CameraUpdate(&Camera);
    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, &RenderState.FrameBuffer, RenderState.ZBuffer, &Assets, Light, &Camera);

    DrawSimpleTexture2D(&RenderState.FrameBuffer, Light.X - 16, Light.Y - 16, 36, 36, &SunTexture, COLOR(1, 1, 0));

    if (KeyPressed[KEY_COMMA]) {
      OutputZBufferToFile("zbuffer.png");
      fprintf(stdout, "Z buffer saved to 'zbuffer.png'\n");
    }
    if (KeyPressed[KEY_PERIOD]) {
      OutputFrameBufferToFile(&RenderState.FrameBuffer, "framebuffer.png");
      fprintf(stdout, "Framebuffer saved to 'framebuffer.png'\n");
    }

    // TODO(lucas): Properly implement timestepping!
    if (LastFrame > (1.0f / TARGET_FPS)) {
      snprintf(Title, BUFFER_SIZE, "Software Renderer | fps: %i, dt: %g, last: %.3f ms", (i32)(1.0f / Game->DeltaTime), Game->DeltaTime, LastFrame);
      WindowSetTitle(Title);
      LastFrame = 0; // (1.0f / TARGET_FPS);
      RendererSwapBuffers();
    }
    RendererClear(0, 0, 0);
  }
  AssetsUnloadAll(&Assets);
}

static entity* GameAddEntity(v3 Position, mesh_id MeshId, texture_id TextureId) {
  if (GameState.EntityCount >= MAX_ENTITY) {
    return NULL;
  }
  entity* Entity = &GameState.Entities[GameState.EntityCount++];
  EntityInit(Entity, Position, MeshId, TextureId);
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
