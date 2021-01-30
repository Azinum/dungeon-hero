// audio.c

#include <portaudio.h>

static audio_engine AudioEngine;
static PaStream* Stream;
static PaStreamParameters OutPort;

static i32 StereoCallback(const void* InBuffer, void* OutBuffer, u64 FramesPerBuffer, const PaStreamCallbackTimeInfo* TimeInfo, PaStreamCallbackFlags Flags, void* UserData);
static i32 OpenStream(PaStream* Stream, PaStreamParameters* InPort, PaStreamParameters* OutPort, u32 SampleRate, u32 FramesPerBuffer);

i32 StereoCallback(const void* InBuffer, void* OutBuffer, u64 FramesPerBuffer, const PaStreamCallbackTimeInfo* TimeInfo, PaStreamCallbackFlags Flags, void* UserData) {
  float* Out = (float*)OutBuffer;
  (void)InBuffer; (void)TimeInfo; (void)Flags; (void)UserData;

  for (u32 SampleIndex = 0; SampleIndex < FramesPerBuffer; ++SampleIndex) {
    float Left = 0;
    float Right = 0;

    audio_state* Sound = &AudioEngine.Sound;
    if (Sound->Id >= 0 && Sound->Id < MAX_AUDIO) {
      audio_source* Source = &Assets.Audio[Sound->Id];
      if (Source) {
        if (Sound->Index < Source->SampleCount) {
          if (Source->ChannelCount == 2) {
            Left  += Sound->Volume * Source->Buffer[Sound->Index++];
            Right += Sound->Volume * Source->Buffer[Sound->Index++];
          }
          else if (Source->ChannelCount == 1) {
            float Frame = Sound->Volume * Source->Buffer[Sound->Index++];
            Left  += Frame;
            Right += Frame;
          }
        }
      }
    }

    Left  *= G_MasterVolume;
    Right *= G_MasterVolume;
    *(Out++) = Left;
    *(Out++) = Right;
  }
  return paContinue;
}

i32 OpenStream(PaStream* Stream, PaStreamParameters* InPort, PaStreamParameters* OutPort, u32 SampleRate, u32 FramesPerBuffer) {
  PaError Error = Pa_OpenStream(
    &Stream,
    InPort,
    OutPort,
    SampleRate,
    FramesPerBuffer,
    0,
    StereoCallback,
    NULL
  );
  if (Error != paNoError) {
    fprintf(stderr, "[PortAudio Error]: %s\n", Pa_GetErrorText(Error));
    return -1;
  }
  Pa_StartStream(Stream);
  return 0;
}

static i32 AudioInit(u32 SampleRate, u32 FramesPerBuffer, callback Callback) {
  i32 Result = 0;
  PaError Error;
  if ((Error = Pa_Initialize()) != paNoError) {
    fprintf(stderr, "[PortAudio Error]: %s\n", Pa_GetErrorText(Error));
    Result = -1;
    goto Done;
  }
  AudioEngine.Sound = (audio_state) {
    .Volume = 0.0f,
    .Id = -1,
    .Index = 0,
  };

  i32 OutputDevice = Pa_GetDefaultOutputDevice();
  OutPort.device = OutputDevice;
  OutPort.channelCount = 2;
  OutPort.sampleFormat = paFloat32;
  OutPort.suggestedLatency = Pa_GetDeviceInfo(OutputDevice)->defaultHighOutputLatency;
  OutPort.hostApiSpecificStreamInfo = NULL;

  if ((Result = OpenStream(Stream, NULL, &OutPort, SampleRate, FramesPerBuffer)) != 0) {
    goto Done;
  }

  if (Callback) {
    Callback();
  }
Done:
  Pa_CloseStream(Stream);
  Pa_Terminate();
  return Result;
}

static void AudioPlay(i32 Id, float Volume) {
  audio_state* Sound = &AudioEngine.Sound;
  Sound->Volume = Volume;
  Sound->Id = Id;
  Sound->Index = 0;
}
