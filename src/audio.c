// audio.c

#include <portaudio.h>

static audio_state AudioState;
static PaStream* Stream;
static PaStreamParameters OutPort;

static i32 StereoCallback(const void* InBuffer, void* OutBuffer, u64 FramesPerBuffer, const PaStreamCallbackTimeInfo* TimeInfo, PaStreamCallbackFlags Flags, void* UserData);
static i32 OpenStream(PaStream* Stream, PaStreamParameters* InPort, PaStreamParameters* OutPort, u32 SampleRate, u32 FramesPerBuffer);

i32 StereoCallback(const void* InBuffer, void* OutBuffer, u64 FramesPerBuffer, const PaStreamCallbackTimeInfo* TimeInfo, PaStreamCallbackFlags Flags, void* UserData) {
  float* Out = (float*)OutBuffer;
  for (u32 SampleIndex = 0; SampleIndex < FramesPerBuffer; ++SampleIndex) {
    *(Out++) = 0;
    *(Out++) = 0;
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
  AudioState.Volume = 1.0f;
  AudioState.SampleRate = SampleRate;
  AudioState.FramesPerBuffer = FramesPerBuffer;

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
