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
#define UI_BUFFER_SIZE 256
#define MAX_DELTA_TIME 0.5f

#undef LIGHT_STRENGTH
#define LIGHT_STRENGTH 40.0f

game_state GameState;
assets Assets = {0};
static v3 Light = V3(0.0f, 1.0f, 9.0f);
static float LightStrength = 20.0f;
static char UIText[UI_BUFFER_SIZE] = {0};

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
        entity* Box = GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_BOX);
        Box->Type = ENTITY_ROTATOR;
        continue;
      }
      if (!(rand() % 49)) {
        GameAddEntity(V3(X, 0, Z), MESH_MONSTER, TEXTURE_MONSTER);
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
  CameraInit(&Camera, V3(0, 2.0f, 0));

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

    LightStrength = LIGHT_STRENGTH + 40.0f * (0.2f * sin(Game->Time * 2.0f));
    PlatformGetCursorPos(&MouseX, &MouseY);
    CameraUpdate(&Camera);
    CameraUpdateViewMatrix(&Camera);
    UpdateAndDrawEntities((entity*)Game->Entities, Game->EntityCount, Renderer, &Assets, Light, LightStrength, &Camera);

    if (KeyPressed[KEY_T]) {
      v3 Position = AddToV3(Camera.P, V3(
        Camera.Forward.X * 1.5f,
        Camera.Forward.Y * 1.5f,
        Camera.Forward.Z * 1.5f
      ));
      GameAddEntity(Position, MESH_CUBE, TEXTURE_TEST);
    }

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
    if (KeyPressed[KEY_0]) {
      LoadConfig(CONFIG_FILE);
      continue;
    }
    if (KeyPressed[KEY_M]) {
      PlatformSetCursorMode(!Win.CursorMode);
    }

    // TODO(lucas): Properly implement timestepping!
    // if (LastFrame > (1.0f / G_TargetFps)) {
      ++Tick;
      float Delta = LastFrame - (1.0f / G_TargetFps);
      if (!(Tick % 30)) {
        snprintf(Title, BUFFER_SIZE, "%s | fps: %i, dt: %g, last: %.3f ms | %ix%i", WINDOW_TITLE, (i32)(1.0f / Game->DeltaTime), Game->DeltaTime, LastFrame, WindowWidth(), WindowHeight());
        WindowSetTitle(Title);
        snprintf(UIText, UI_BUFFER_SIZE, "%i fps, delta: %g", (i32)(1.0f / Game->DeltaTime), Game->DeltaTime);
      }
      // LastFrame -= Delta;
      // if (LastFrame > 1.0f) {
      //   LastFrame = 1.0f;
      // }

      DrawText(Renderer, &Assets, 16, 16, 0.9f, 0, 0, 12, UIText, UI_BUFFER_SIZE, TEXTURE_DEFAULT_FONT, COLOR(255, 255, 255));
      DrawSkybox(Renderer, &Assets, &Camera, CUBEMAP_SKYBOX_DOOM);
      RendererSwapBuffers(Renderer);
      RendererClear(G_BackgroundColor.X, G_BackgroundColor.Y, G_BackgroundColor.Z);
    // }
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
