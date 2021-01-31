// game.c

#include "game.h"

#include "misc.c"
#include "config.c"
#include "image.c"
#include "mesh.c"
#include "audio.c"
#include "riff.c"
#include "asset.c"
#include "platform.c"

#include "camera.c"
#include "entity.c"

#define BUFFER_SIZE 512
#define MAX_DELTA_TIME 0.5f

#undef LIGHT_STRENGTH
#define LIGHT_STRENGTH 30.0f

game_state GameState;
assets Assets = {0};
static v3 Light = V3(0.0f, 0.5f, 7.0f);
static float LightStrength = 15.0f;
double MouseX = 0;
double MouseY = 0;

v3 WorldMin = V3(-13, 0, 4);
v3 WorldMax = V3(12, 0, 15);

static void GameStateInit(game_state* Game) {
  memset(Game, 0, sizeof(game_state));
  Game->Time = 0;
  Game->DeltaTime = 0;
  Game->EntityCount = 0;

#if 1
  for (i32 Z = WorldMin.Z; Z <= WorldMax.Z; ++Z) {
    for (i32 X = WorldMin.X; X <= WorldMax.X; ++X) {
      GameAddEntity(V3(X, -1, Z), MESH_PLANE, TEXTURE_UV);

      if (X == WorldMin.X || X == WorldMax.X || Z == WorldMin.Z || Z == WorldMax.Z) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_TEST);
        GameAddEntity(V3(X, 1, Z), MESH_CUBE, TEXTURE_TEST);
        GameAddEntity(V3(X, 2, Z), MESH_CUBE, TEXTURE_TEST);
        continue;
      }
      if (!(rand() % 40)) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_BOX);
        continue;
      }
      if (!(rand() % 49)) {
        entity* Monster = GameAddEntity(V3(X, 0, Z), MESH_MONSTER, TEXTURE_MONSTER);
        Monster->Type = ENTITY_ROTATOR;
        continue;
      }
    }
  }
#else
#endif
}

static void GameRun() {
  game_state* Game = &GameState;
  AssetsLoadAll(&Assets);

  render_state* Renderer = &RenderState;

  WindowOpen(G_WindowWidth, G_WindowHeight, WINDOW_TITLE, G_WindowFullscreen);
  if (RendererInit(Renderer, &Assets) != 0) {
    return;
  }

  PlatformSetCursorMode(CURSOR_DISABLED);
  PlatformEnableRawMouseMotion();

  GameStateInit(Game);
  AudioPlay(AUDIO_TRACK_1, G_MusicVolume);
  CameraInit(&Camera, V3(0, 1.5f, 0));

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

    LightStrength = LIGHT_STRENGTH + 50.0f * (0.2f * sin(Game->Time * 1.5f));
    PlatformGetCursorPos(&MouseX, &MouseY);
    CameraUpdate(&Camera);
    CameraUpdateViewMatrix(&Camera);
    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, Renderer, &Assets, Light, LightStrength, &Camera);

    DrawTexture2D(Renderer, &Assets, 16, 16, 0.5f, 48, 48, 0, 0, 1, 1, TEXTURE_SUN_ICON, COLOR(1, 1, 1));

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
      DrawSkybox(Renderer, &Assets, &Camera, CUBEMAP_SKYBOX_DOOM);
      RendererSwapBuffers(Renderer);
      RendererClear(G_BackgroundColor.X, G_BackgroundColor.Y, G_BackgroundColor.Z);
    }
  }

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
  AssetsUnloadAll(&Assets);
}
