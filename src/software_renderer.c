// software_renderer.c

#include "software_renderer.h"

#include "image.c"

#define Abs(X) (((X) >= 0) ? (X) : -(X))

#define Swap(V0, V1) { \
  __typeof__(V0) Temp = V0; \
  V0 = V1; \
  V1 = Temp; \
}

#define Min3(A, B, C) (((A < B) && (A < C)) ? (A) : ((B < C) ? (B) : C ))
#define Max3(A, B, C) (((A > B) && (A > C)) ? (A) : ((B > C) ? (B) : C ))

#define Min(A, B) ((A < B ) ? (A) : (B))
#define Max(A, B) ((A > B ) ? (A) : (B))

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

static i32 OrientationTest2D(v2 A, v2 B, v2 C) {
  i32 Result = 0;

  Result = (C.X - A.X) * (B.Y - A.Y) - (C.Y - A.Y) * (B.X - A.X);

  return Result;
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

static void DrawTriangle(image* FrameBuffer, v2 A, v2 B, v2 C, u8 ColorR, u8 ColorG, u8 ColorB) {
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
}

static void DrawScanLine(image* FrameBuffer, v2 A, v2 B, u8 ColorR, u8 ColorG, u8 ColorB) {
  for (i32 PixelX = A.X; PixelX < B.X; ++PixelX) {
    DrawPixel(FrameBuffer, PixelX, A.Y, ColorR, ColorG, ColorB);
  }
}

static void DrawFilledTriangle(image* FrameBuffer, v2 A, v2 B, v2 C, u8 ColorR, u8 ColorG, u8 ColorB) {
#if 0
  if (A.Y > B.Y) {
    Swap(A, B);
  }
  if (A.Y > C.Y) {
    Swap(A, C);
  }
  if (B.Y > C.Y) {
    Swap(B, C);
  }
#endif

  i32 MinX = Min3(A.X, B.X, C.X);
  i32 MinY = Min3(A.Y, B.Y, C.Y);
  i32 MaxX = Max3(A.X, B.X, C.X);
  i32 MaxY = Max3(A.Y, B.Y, C.Y);

  MinX = Max(MinX, 0);
  MinX = Max(MinX, 0);
  MaxX = Min(MaxX, FrameBuffer->Width - 1);
  MaxY = Min(MaxY, FrameBuffer->Height - 1);

  v2 P = {0};
  for (P.Y = MinY; P.Y <= MaxY; P.Y++) {
    for (P.X = MinX; P.X <= MaxX; P.X++) {
      // NOTE(lucas): Find the barycentric coordinates
      i32 W0 = OrientationTest2D(B, C, P);
      i32 W1 = OrientationTest2D(C, A, P);
      i32 W2 = OrientationTest2D(A, B, P);

      if (W0 >= 0 && W1 >= 0 && W2 >= 0) {
        DrawPixel(FrameBuffer, P.X, P.Y, ColorR, ColorG, ColorB);
      }
    }
  }
}

i32 SoftwareRendererInit(u32 Width, u32 Height) {
  i32 Result = 0;
  (void)LoadImage;
  (void)DrawLine;
  (void)DrawFilledTriangle;

  render_state* State = &RenderState;
  FrameBufferInit(&State->FrameBuffer, Width, Height, 24, 3);

{
  v2 A = V2(60, 50);
  v2 B = V2(50, 100);
  v2 C = V2(100, 100);
  DrawFilledTriangle(&State->FrameBuffer, A, B, C, 255, 255, 255);
  DrawTriangle(&State->FrameBuffer, A, B, C, 255, 0, 0);
}
{
  v2 A = V2(150, 100);
  v2 B = V2(130, 150);
  v2 C = V2(180, 200);
  DrawFilledTriangle(&State->FrameBuffer, A, B, C, 255, 255, 255);
  DrawTriangle(&State->FrameBuffer, A, B, C, 255, 0, 0);
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
