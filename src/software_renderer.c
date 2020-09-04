// software_renderer.c

#include "software_renderer.h"

#include "image.c"

typedef struct render_state {
  image FrameBuffer;
} render_state;

static render_state RenderState;

static void FrameBufferInit(image* FrameBuffer, u32 Width, u32 Height, u32 Depth, u32 BytesPerPixel) {
  Assert(FrameBuffer);
  FrameBuffer->PixelBuffer = calloc(Width * Height * BytesPerPixel, sizeof(u8));
  FrameBuffer->Width = Width;
  FrameBuffer->Height = Height;
  FrameBuffer->Depth = Depth;
  FrameBuffer->Pitch = Width * BytesPerPixel;
  FrameBuffer->BytesPerPixel = BytesPerPixel;
}

static void FrameBufferDestroy(image* FrameBuffer) {
  Assert(FrameBuffer);
  free(FrameBuffer->PixelBuffer);
}

i32 SoftwareRendererInit(u32 Width, u32 Height) {
  i32 Result = 0;

  FrameBufferInit(&RenderState.FrameBuffer, Width, Height, 24, 3);
  (void)LoadImage;

  return Result;
}

void SoftwareRendererStart() {
  StoreImage("image.png", &RenderState.FrameBuffer);
  SoftwareRendererExit();
}

void SoftwareRendererExit() {
  FrameBufferDestroy(&RenderState.FrameBuffer);
}
