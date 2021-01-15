// config.c

static i32 G_WindowWidth = 800;
static i32 G_WindowHeight = 600;
static i32 G_WindowFullscreen = 0;
static i32 G_TargetFps = 60;
static i32 G_CompatibleOpenGL = 0;
static i32 G_Fov = 75;
static double G_Sensitivity = 0.05f;

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
  } while (1);

  BufferFree(Buffer.Data, Buffer.Count);
  return 0;
}
