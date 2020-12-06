// game.c

#include "game.h"

#include "misc.c"
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
static v3 Light = V3(0.0f, 2.0f, 6);

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
  for (i32 Z = 4; Z <= 11; ++Z) {
    for (i32 X = -5; X <= 5; ++X) {
      if (!(rand() % 20)) {
        entity* E = GameAddEntity(V3(X, 0, Z), MESH_STONE, TEXTURE_TEST);
        E->Type = ENTITY_ROTATOR;
      }
      if (!(rand() % 20)) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_BOX);
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
#if 0
  for (i32 Z = 4; Z < 10; ++Z) {
    for (i32 X = -4; X <= 4; ++X) {
      if (!(rand() % 16)) {
        entity* E = GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_UV);
        E->Type = ENTITY_ROTATOR;
      }
      if (!(rand() % 16)) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_UV);
      }
      if (!(rand() % 20)) {
        GameAddEntity(V3(X, 0, Z), MESH_CUBE, TEXTURE_UV);
      }

      GameAddEntity(V3(X, -1, Z), MESH_CUBE, TEXTURE_UV);
    }
  }
#endif
#if 0
  entity* Entity = GameAddEntity(V3(0, -1, 6), MESH_MONSTER, TEXTURE_MONSTER);
  Entity->Type = ENTITY_ROTATOR;
#endif
}

static void GameRun() {
  game_state* Game = &GameState;
  assets Assets;
  AssetsLoadAll(&Assets);

  render_state* Renderer = &RenderState;

  WindowOpen(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  if (RendererInit(Renderer, &Assets) != 0)
    return;
  GameStateInit(Game);
  CameraInit(&Camera, V3(0, 0, 0));

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
    // TODO(lucas): Properly implement timestepping!
    if (LastFrame > (1.0f / TARGET_FPS)) {
      ++Tick;
      float Delta = LastFrame - (1.0f / TARGET_FPS);
      if (!(Tick % 15)) {
        snprintf(Title, BUFFER_SIZE, "%s | fps: %i, dt: %g, last: %.3f ms", WINDOW_TITLE, (i32)(1.0f / Game->DeltaTime), Game->DeltaTime, LastFrame);
        WindowSetTitle(Title);
      }
      LastFrame -= Delta;
      if (LastFrame > 1.0f) {
        LastFrame = 1.0f;
      }
      RendererSwapBuffers(Renderer);
      RendererClear(30, 40, 100);
    }
  }

  AssetsUnloadAll(&Assets);
  RendererDestroy();
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
  srand(time(NULL));
  AudioInit(SAMPLE_RATE, FRAMES_PER_BUFFER, GameRun);
}
