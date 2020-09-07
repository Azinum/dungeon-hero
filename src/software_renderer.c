// software_renderer.c

static render_state RenderState;

inline i32 Barycentric(v3 A, v3 B, v3 C) {
  i32 Result = 0;

  Result = (C.X - A.X) * (B.Y - A.Y) - (C.Y - A.Y) * (B.X - A.X);

  return Result;
}

inline void DrawPixel(framebuffer* FrameBuffer, i32 X, i32 Y, u8 ColorR, u8 ColorG, u8 ColorB) {
  if (X < 0 || Y < 0 || X >= FrameBuffer->Width || Y >= FrameBuffer->Height) {
    return;
  }
  i8* Pixel = &FrameBuffer->Data[4 * ((Y * FrameBuffer->Width) + X)];
  Pixel[0] = ColorB;
  Pixel[1] = ColorG;
  Pixel[2] = ColorR;
}

inline void DrawLine(framebuffer* FrameBuffer, v2 A, v2 B, u8 ColorR, u8 ColorG, u8 ColorB) {
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

// NOTE(lucas): Triangles are drawn in counterclockwise order:
// ABC ABC ABC
inline void DrawFilledTriangle(framebuffer* FrameBuffer, i32* ZBuffer, v3 A, v3 B, v3 C, u8 ColorR, u8 ColorG, u8 ColorB) {
  i32 MinX = Min3(A.X, B.X, C.X);
  i32 MinY = Min3(A.Y, B.Y, C.Y);
  i32 MaxX = Max3(A.X, B.X, C.X);
  i32 MaxY = Max3(A.Y, B.Y, C.Y);

  MinX = Max(MinX, 0);
  MinY = Max(MinY, 0);
  MaxX = Min(MaxX, FrameBuffer->Width - 1);
  MaxY = Min(MaxY, FrameBuffer->Height - 1);

  v3 P = {0};
  float Z = 0;
  for (P.Y = MinY; P.Y <= MaxY; P.Y++) {
    for (P.X = MinX; P.X <= MaxX; P.X++) {
      i32 W0 = Barycentric(B, C, P);
      i32 W1 = Barycentric(C, A, P);
      i32 W2 = Barycentric(A, B, P);

      // NOTE(lucas): Are we inside the triangle?
      if (W0 >= 0 && W1 >= 0 && W2 >= 0) {
        i32 Z = 0;
        Z += (A.Z * W0);
        Z += (B.Z * W1);
        Z += (C.Z * W2);

        i32 Index = P.X + (P.Y * FrameBuffer->Width);
        if (ZBuffer[Index] > Z) {
          ZBuffer[Index] = Z;
          DrawPixel(FrameBuffer, P.X, P.Y, ColorR, ColorG, ColorB);
        }
      }
    }
  }
}

inline void DrawFilledTriangleAt(framebuffer* FrameBuffer, i32* ZBuffer, v3 A, v3 B, v3 C, v3 P, u8 ColorR, u8 ColorG, u8 ColorB) {
  DrawFilledTriangle(
    FrameBuffer,
    ZBuffer,
    V3(P.X + A.X, P.Y + A.Y, P.Z + A.Z),
    V3(P.X + B.X, P.Y + B.Y, P.Z + B.Z),
    V3(P.X + C.X, P.Y + C.Y, P.Z + C.Z),
    ColorR, ColorG, ColorB);
}

#define LightStrength 12500
#define MODEL_SCALE 1000

inline void DrawMesh(framebuffer* FrameBuffer, i32* ZBuffer, mesh* Mesh, v3 P, v2 Light) {
  for (u32 Index = 0; Index < Mesh->IndexCount; Index += 3) {
    v3 V[3];
    v3 A[3];

    V[0] = Mesh->Vertices[Mesh->Indices[Index + 0]];
    V[1] = Mesh->Vertices[Mesh->Indices[Index + 1]];
    V[2] = Mesh->Vertices[Mesh->Indices[Index + 2]];
    A[0] = V3(V[0].X * MODEL_SCALE, V[0].Y * MODEL_SCALE, V[0].Z * MODEL_SCALE);
    A[1] = V3(V[1].X * MODEL_SCALE, V[1].Y * MODEL_SCALE, V[1].Z * MODEL_SCALE);
    A[2] = V3(V[2].X * MODEL_SCALE, V[2].Y * MODEL_SCALE, V[2].Z * MODEL_SCALE);

    float LightFactorX = 1.0f / (Abs(Light.X - A[0].X));
    float LightFactorY = 1.0f / (Abs(Light.Y - A[0].Y));
    float LightFactor = Clamp(LightFactorX * LightFactorY * LightStrength, 1.0f);

    DrawFilledTriangleAt(FrameBuffer, ZBuffer, A[0], A[1], A[2], P,
      30 * LightFactor,
      90 * LightFactor,
      255 * LightFactor
    );
  }
}

i32 RendererInit(u32 Width, u32 Height) {
  (void)LoadImage; (void)StoreImage; (void)DrawLine; (void)DrawFilledTriangle;

  render_state* State = &RenderState;
  FrameBufferCreate(&State->FrameBuffer, Width, Height);
  State->ZBuffer = calloc(Width * Height, sizeof(float));

  WindowOpen(Width, Height, WINDOW_TITLE);

  return 0;
}

static void RendererSwapBuffers() {
  WindowSwapBuffers(&RenderState.FrameBuffer);
}

static void RendererClear() {
  FrameBufferClear(&RenderState.FrameBuffer, 0, 0, 0);
  memset(RenderState.ZBuffer, 10000, sizeof(i32) * RenderState.FrameBuffer.Width * RenderState.FrameBuffer.Height);
}

void RendererDestroy() {
  FrameBufferDestroy(&RenderState.FrameBuffer);
  free(RenderState.ZBuffer);
  WindowClose();
}
