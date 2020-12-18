// config.c

static i32 G_WindowWidth = 800;
static i32 G_WindowHeight = 600;
static i8 G_WindowFullscreen = 0;
static i32 G_TargetFps = 60;

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
  } while (1);

#if 1
  printf("Loaded config file '%s'\n", Path);
  printf("window_width = %i\n", G_WindowWidth);
  printf("window_height = %i\n", G_WindowHeight);
  printf("window_fullscreen = %i\n", G_WindowFullscreen);
  printf("target_fps = %i\n", G_TargetFps);
#endif

  BufferFree(Buffer.Data, Buffer.Count);
  return 0;
}
