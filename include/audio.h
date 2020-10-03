// audio.h

typedef struct audio_state {
  float Volume;
  u32 SampleRate;
  u32 FramesPerBuffer;
} audio_state;

typedef void (*callback)();

static i32 AudioInit(u32 SampleRate, u32 FramesPerBuffer, callback Callback);
