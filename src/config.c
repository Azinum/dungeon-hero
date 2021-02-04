// config.c

static i32 G_WindowWidth = 800;
static i32 G_WindowHeight = 600;
static i32 G_WindowFullscreen = 0;
static i32 G_TargetFps = 60;
static i32 G_CompatibleOpenGL = 0;
static i32 G_Fov = 75;
static double G_Sensitivity = 0.05f;

static float G_Ambient = 5.0f;
static i32 G_DrawSolid = 0;
static i32 G_DrawBoundingBox = 0;
static i32 G_DrawBoundingBoxPoints = 0;
static i32 G_DrawVertices = 0;
static i32 G_DrawWireframe = 0;
static i32 G_NoLighting = 0;
static v3 G_BackgroundColor = V3(40, 40, 100);

static float G_MasterVolume = 1.0f;
static float G_MusicVolume = 0.5f;

#define FOV_MIN 30
#define FOV_MAX 110

#define SScanf(RESULT, IterBuffer, Format, ...) {\
  u32 ReadBytes = 0;\
  RESULT = sscanf(IterBuffer, Format "%n", __VA_ARGS__, &ReadBytes);\
  IterBuffer += ReadBytes;\
}

#define WORD_SIZE 128

static i32 LoadConfig(const char* Path) {
  buffer Buffer = {0};
  if (ReadFile(Path, &Buffer) != 0) {
    fprintf(stderr, "Failed to load config '%s'\n", Path);
    return -1;
  }

  char Word[WORD_SIZE] = {0};
  char* Iter = Buffer.Data;
  i32 Result = 0;

  do {
    SScanf(Result, Iter, "%s", Word);
    if (Result == EOF) {
      break;
    }

    if (!strncmp(Word, "window_width", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_WindowWidth);
    }
    else if (!strncmp(Word, "window_height", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_WindowHeight);
    }
    else if (!strncmp(Word, "window_fullscreen", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_WindowFullscreen);
    }
    else if (!strncmp(Word, "target_fps", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_TargetFps);
    }
    else if (!strncmp(Word, "compat_opengl", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_CompatibleOpenGL);
    }
    else if (!strncmp(Word, "fov", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_Fov);
      G_Fov = Clamp(G_Fov, FOV_MIN, FOV_MAX);
    }
    else if (!strncmp(Word, "sensitivity", WORD_SIZE)) {
      SScanf(Result, Iter, "%lf", &G_Sensitivity);
    }

    // Rendering options:
    else if (!strncmp(Word, "r_ambient", WORD_SIZE)) {
      SScanf(Result, Iter, "%f", &G_Ambient);
    }
    else if (!strncmp(Word, "r_draw_solid", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_DrawSolid);
    }
    else if (!strncmp(Word, "r_draw_bounding_box", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_DrawBoundingBox);
    }
    else if (!strncmp(Word, "r_draw_bounding_box_points", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_DrawBoundingBoxPoints);
    }
    else if (!strncmp(Word, "r_draw_vertices", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_DrawVertices);
    }
    else if (!strncmp(Word, "r_draw_wireframe", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_DrawWireframe);
    }
    else if (!strncmp(Word, "r_no_lighting", WORD_SIZE)) {
      SScanf(Result, Iter, "%i", &G_NoLighting);
    }
    else if (!strncmp(Word, "r_background_color", WORD_SIZE)) {
      SScanf(Result, Iter, "%f %f %f", &G_BackgroundColor.X, &G_BackgroundColor.Y, &G_BackgroundColor.Z);
    }

    // Audio:
    else if (!strncmp(Word, "master_volume", WORD_SIZE)) {
      SScanf(Result, Iter, "%f", &G_MasterVolume);
      G_MasterVolume = Clamp(G_MasterVolume, 0.0f, 1.0f);
    }
    else if (!strncmp(Word, "music_volume", WORD_SIZE)) {
      SScanf(Result, Iter, "%f", &G_MusicVolume);
      G_MusicVolume = Clamp(G_MusicVolume, 0.0f, 1.0f);
    }
  } while (1);

  BufferFree(Buffer.Data, Buffer.Count);
  return 0;
}
