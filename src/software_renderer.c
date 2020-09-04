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

static void DrawPixel(image* FrameBuffer, i32 X, i32 Y, u8 R, u8 G, u8 B) {
  if (X < 0 || Y < 0 || X > FrameBuffer->Width || Y > FrameBuffer->Height) {
    return;
  }
  u8* Pixel = &FrameBuffer->PixelBuffer[FrameBuffer->BytesPerPixel * ((Y * FrameBuffer->Width) + X)];
  Assert(Pixel);
  Pixel[0] = R;
  Pixel[1] = G;
  Pixel[2] = B;
}

static void DrawLine(image* FrameBuffer, v2 P1, v2 P2, u8 R, u8 G, u8 B) {
  i32 DistX = P2.X - P1.X;
  i32 DistY = P2.Y - P1.Y;

  float StepX = (float)DistX / (float)DistY;
  float PixelX = (float)P1.X;

  for (i32 PixelY = P1.Y; PixelY < P2.Y; ++PixelY) {
    DrawPixel(FrameBuffer, PixelX, PixelY, R, G, B);
    PixelX += StepX;
  }
}

static void DrawScanLine(image* FrameBuffer, v2 P1, v2 P2, u8 R, u8 G, u8 B) {
  for (i32 PixelX = P1.X; PixelX < P2.X; ++PixelX) {
    DrawPixel(FrameBuffer, PixelX, P1.Y, R, G, B);
  }
}

static void DrawTriangle(image* FrameBuffer, v2 P1, v2 P2, v2 P3, u8 R, u8 G, u8 B) {
  i32 DistX1 = P2.X - P1.X;
  i32 DistY1 = P2.Y - P1.Y;
  i32 DistX2 = P3.X - P1.X;
  i32 DistY2 = P3.Y - P1.Y;

  float StepX1 = (float)DistX1 / DistY1;
  float StepX2 = (float)DistX2 / DistY2;
  float PixelX1 = P1.X;
  float PixelX2 = P1.X;

  for (i32 PixelY = P1.Y; ; ++PixelY) {
    if (PixelY == P2.Y || PixelY == P3.Y) {
      break;
    }
    DrawScanLine(FrameBuffer, V2(P1.X + PixelX1, PixelY), V2(P1.X + PixelX2, PixelY), R, G, B);
    PixelX1 += StepX1;
    PixelX2 += StepX2;
  }
}

static void DrawFilledTriangle(image* FrameBuffer, v2 P1, v2 P2, v2 P3, u8 R, u8 G, u8 B) {

}

i32 SoftwareRendererInit(u32 Width, u32 Height) {
  i32 Result = 0;
  (void)LoadImage;
  (void)DrawTriangle;
  (void)DrawFilledTriangle;
  (void)DrawLine;

  render_state* State = &RenderState;
  FrameBufferInit(&State->FrameBuffer, Width, Height, 24, 3);

  v2 P1 = V2(50, 50);
  v2 P2 = V2(30, 90);
  v2 P3 = V2(70, 70);

  // DrawFilledTriangle(&State->FrameBuffer, P1, P2, P3, 0xff, 0xff, 0xff);
  DrawTriangle(&State->FrameBuffer, P1, P2, P3, 255, 255, 255);

  return Result;
}

void SoftwareRendererStart() {
  StoreImage("image.png", &RenderState.FrameBuffer);
  SoftwareRendererExit();
}

void SoftwareRendererExit() {
  FrameBufferDestroy(&RenderState.FrameBuffer);
}
