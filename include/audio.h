// audio.h

typedef struct audio_source {
  float* Buffer;
  i32 SampleCount;
  i32 ChannelCount;
} audio_source;

typedef struct audio_state {
  float Volume;
  i32 Id;
  i32 Index;
} audio_state;

typedef struct audio_engine {
  float MasterVolume;
  audio_state Sound;
} audio_engine;

typedef void (*callback)();

static i32 AudioInit(u32 SampleRate, u32 FramesPerBuffer, callback Callback);

static void AudioPlay(i32 Id, float Volume);
