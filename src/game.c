// game.c

#include "game.h"

#include "misc.c"
#include "config.c"
#include "image.c"
#include "mesh.c"
#include "audio.c"
#include "asset.c"
#include "platform.c"

#include "camera.c"
#include "entity.c"

#define BUFFER_SIZE 512
#define MAX_DELTA_TIME 0.5f

game_state GameState;
static v3 Light = V3(0.0f, 0.7f, 8.0f);
double MouseX = 0;
double MouseY = 0;

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->Time = 0;
  Game->DeltaTime = 0;
  Game->EntityCount = 0;

#if 1
  for (i32 Z = 4; Z <= 11; ++Z) {
    for (i32 X = -5; X <= 5; ++X) {
      if (!(rand() % 20)) {
        entity* Cube = GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_BOX);
        Cube->Type = ENTITY_ROTATOR;
      }
      if (!(rand() % 20)) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_UV);
      }
      if (!(rand() % 35)) {
        GameAddEntity(V3(X, 0, Z), MESH_MONSTER, TEXTURE_MONSTER);
      }

      GameAddEntity(V3(X, -1, Z), MESH_PLANE, TEXTURE_TEST);
    }
  }
#endif
}

static void GameRun() {
  game_state* Game = &GameState;
  assets Assets;
  AssetsLoadAll(&Assets);

  render_state* Renderer = &RenderState;

  WindowOpen(G_WindowWidth, G_WindowHeight, WINDOW_TITLE, G_WindowFullscreen);
  if (RendererInit(Renderer, &Assets) != 0) {
    return;
  }
  WindowFocus();
  PlatformSetCursorMode(CURSOR_DISABLED);
  GameStateInit(Game);
  CameraInit(&Camera, V3(0, 1, 0));

  char Title[BUFFER_SIZE] = {0};
  struct timeval TimeNow = {0};
  struct timeval TimeLast = {0};
  float LastFrame = 0;

  u8 IsRunning = 1;
  u32 Tick = 0;
  while (IsRunning && !WindowEvents()) {
    TimeLast = TimeNow;
    gettimeofday(&TimeNow, NULL);
    Game->DeltaTime = ((((TimeNow.tv_sec - TimeLast.tv_sec) * 1000000.0f) + TimeNow.tv_usec) - (TimeLast.tv_usec)) / 1000000.0f;
    if (Game->DeltaTime > MAX_DELTA_TIME) {
      Game->DeltaTime = MAX_DELTA_TIME;
    }
    Game->Time += Game->DeltaTime;
    LastFrame += Game->DeltaTime;

    PlatformGetCursorPos(&MouseX, &MouseY);
    CameraUpdate(&Camera);
    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, Renderer, &Assets, Light, &Camera);

    // DrawSimpleTexture2D(Renderer, Light.X - 16, Light.Y - 16, 36, 36, &Assets.Textures[TEXTURE_SUN_ICON], COLOR(1, 1, 0));

    if (KeyPressed[KEY_COMMA]) {
      char Date[MAX_PATH_SIZE];
      char Path[MAX_PATH_SIZE];
      GetDateAndTime(Date, MAX_PATH_SIZE);
      snprintf(Path, MAX_PATH_SIZE, "%s/zbuffer_%s.png", SCREENSHOT_DIR, Date);
      OutputZBufferToFile(Renderer, Path);
      snprintf(Path, MAX_PATH_SIZE, "%s/screenshot_%s.png", SCREENSHOT_DIR, Date);
      OutputFrameBufferToFile(Renderer, Path);
    }
    if (KeyPressed[KEY_R]) {
      GameStateInit(Game);
      continue;
    }
    if (KeyPressed[KEY_M]) {
      PlatformSetCursorMode(!Win.CursorMode);
    }
    // TODO(lucas): Properly implement timestepping!
    if (LastFrame > (1.0f / G_TargetFps)) {
      ++Tick;
      float Delta = LastFrame - (1.0f / G_TargetFps);
      if (!(Tick % 15)) {
        snprintf(Title, BUFFER_SIZE, "%s | fps: %i, dt: %g, last: %.3f ms | %ix%i", WINDOW_TITLE, (i32)(1.0f / Game->DeltaTime), Game->DeltaTime, LastFrame, WindowWidth(), WindowHeight());
        WindowSetTitle(Title);
      }
      LastFrame -= Delta;
      if (LastFrame > 1.0f) {
        LastFrame = 1.0f;
      }
      RendererSwapBuffers(Renderer);
      RendererClear(40, 40, 100);
    }
  }

  AssetsUnloadAll(&Assets);
  RendererDestroy(Renderer);
  WindowDestroyContext();
  WindowClose();
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
  LoadConfig(CONFIG_FILE);
  srand(time(NULL));
  if (AudioInit(SAMPLE_RATE, FRAMES_PER_BUFFER, GameRun) != 0) {
    // NOTE(lucas): We failed to initialize portaudio if we get to here, but we start the game anyways
    GameRun();
  }
}
