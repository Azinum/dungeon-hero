// main.c
// Software Renderer

#include "game.c"

int main(void) {
  audio_source Source;
  i32 Result = LoadWAVE("resource/audio/track-1.wav", &Source);
  if (Result != NoError) {
    fprintf(stderr, "Failed to load WAVE file\n");
  }
  else {
    fprintf(stdout, "Success!\n");
    free(Source.Buffer);
    Source.Buffer = NULL;
  }
  // GameStart();
  return 0;
}
