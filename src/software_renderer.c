// software_renderer.c

#include "software_renderer.h"

#include "image.c"

#define Abs(X) (((X) >= 0) ? (X) : -(X))

#define Swap(V0, V1) { \
  __typeof__(V0) Temp = V0; \
  V0 = V1; \
  V1 = Temp; \
}

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
  if (FrameBuffer->PixelBuffer) {
    free(FrameBuffer->PixelBuffer);
  }
}

static void DrawPixel(image* FrameBuffer, i32 X, i32 Y, u8 ColorR, u8 ColorG, u8 ColorB) {
  if (X < 0 || Y < 0 || X >= FrameBuffer->Width || Y >= FrameBuffer->Height) {
    return;
  }
  u8* Pixel = &FrameBuffer->PixelBuffer[FrameBuffer->BytesPerPixel * ((Y * FrameBuffer->Width) + X)];
  Assert(Pixel);
  Pixel[0] = ColorR;
  Pixel[1] = ColorG;
  Pixel[2] = ColorB;
}

static void DrawLine(image* FrameBuffer, v2 A, v2 B, u8 ColorR, u8 ColorG, u8 ColorB) {
  u8 Steep = 0;
  if (Abs(A.X - B.X) < Abs(A.Y - B.Y)) {
    Steep = 1;
    Swap(A.X, A.Y);
    Swap(B.X, B.Y);
  }
  if (A.X > B.X) {
    Swap(A.X, B.X);
    Swap(A.Y, B.Y);
  }

  for (i32 X = A.X; X < B.X; X++) {
    float T = (X - A.X) / (float)(B.X - A.X);
    i32 Y = A.Y * (1.0f - T) + (B.Y * T);
    if (Steep) {
      DrawPixel(FrameBuffer, Y, X, ColorR, ColorG, ColorB);
    }
    else {
      DrawPixel(FrameBuffer, X, Y, ColorR, ColorG, ColorB);
    }
  }
}

static void DrawScanLine(image* FrameBuffer, v2 P1, v2 P2, u8 R, u8 G, u8 B) {
  for (i32 PixelX = P1.X; PixelX < P2.X; ++PixelX) {
    DrawPixel(FrameBuffer, PixelX, P1.Y, R, G, B);
  }
}

static void DrawFilledTriangle(image* FrameBuffer, v2 A, v2 B, v2 C, u8 ColorR, u8 ColorG, u8 ColorB) {
  if (A.X == B.X && A.Y == B.Y) {
    return;
  }
  if (A.Y > B.Y) {
    Swap(A, B);
  }
  if (A.Y > C.Y) {
    Swap(A, C);
  }
  if (B.Y > C.Y) {
    Swap(B, C);
  }

  DrawLine(FrameBuffer, A, B, ColorR, ColorG, ColorB);
  DrawLine(FrameBuffer, A, C, ColorR, ColorG, ColorB);
  DrawLine(FrameBuffer, B, C, ColorR, ColorG, ColorB);

#if 0
  if (P1.X == P2.X && P1.Y == P2.Y) {
    return;
  }

  if (P1.Y > P2.Y) {
    SwapV2(&P1, &P2);
  }
  if (P1.Y > P3.Y) {
    SwapV2(&P1, &P3);
  }
  if (P2.Y > P3.Y) {
    SwapV2(&P2, &P3);
  }

  v2 Dist1 = V2(P1.X - P2.X, P1.Y - P2.Y);
  v2 Dist2 = V2(P1.X - P3.X, P1.Y - P3.Y);
  v2 Dist3 = V2(P2.X - P3.X, P2.Y - P3.Y);

  float LeftStepX = Dist1.X / Dist1.Y;
  float RightStepX = Dist2.X / Dist2.Y;
  float SecondStepX = Dist3.X / Dist3.Y;
  
  if (!Dist1.Y)  LeftStepX = 0;
  if (!Dist2.Y)  RightStepX = 0;
  if (!Dist3.Y)  SecondStepX = 0;

  i32 StartX = P1.X;
  float PixelX1 = StartX;
  float PixelX2 = StartX;

  i32 LastPixelY = P3.Y;

  for (i32 PixelY = P1.Y;; ++PixelY) {
    if (PixelY == P2.Y) {
      LeftStepX = SecondStepX;
    }
    if (PixelY == LastPixelY) {
      break;
    }
    DrawScanLine(FrameBuffer, V2(StartX + PixelX1, PixelY), V2(StartX + PixelX2, PixelY), R, G, B);
    DrawPixel(FrameBuffer, StartX + PixelX1, PixelY, 0, 255, 0);
    DrawPixel(FrameBuffer, StartX + PixelX2, PixelY, 50, 100, 255);
    PixelX1 += LeftStepX;
    PixelX2 += RightStepX;
  }
#endif
}

i32 SoftwareRendererInit(u32 Width, u32 Height) {
  i32 Result = 0;
  (void)LoadImage;
  (void)DrawLine;
  (void)DrawFilledTriangle;

  render_state* State = &RenderState;
  FrameBufferInit(&State->FrameBuffer, Width, Height, 24, 3);

{
  v2 A = V2(100, 100);
  v2 B = V2(150, 140);
  v2 C = V2(80, 190);
  DrawFilledTriangle(&State->FrameBuffer, A, B, C, 255, 255, 255);
}
{
  v2 A = V2(220, 400);
  v2 B = V2(120, 230);
  v2 C = V2(200, 200);
  DrawFilledTriangle(&State->FrameBuffer, A, B, C, 255, 255, 255);
}
{
  v2 A = V2(150, 90 + 70);
  v2 B = V2(200, 160 + 70);
  v2 C = V2(130, 220 + 70);
  DrawFilledTriangle(&State->FrameBuffer, A, B, C, 240, 20, 20);
}

  return Result;
}

void SoftwareRendererStart() {
  StoreImage("image.png", &RenderState.FrameBuffer);
  SoftwareRendererExit();
}

void SoftwareRendererExit() {
  FrameBufferDestroy(&RenderState.FrameBuffer);
}
