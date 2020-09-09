// software_renderer.c

static render_state RenderState;

static mat4 Proj;

inline i32 Barycentric(v3 A, v3 B, v3 C) {
  i32 Result = 0;

  Result = (C.X - A.X) * (B.Y - A.Y) - (C.Y - A.Y) * (B.X - A.X);

  return Result;
}

inline void DrawPixel(framebuffer* FrameBuffer, i32 X, i32 Y, u8 ColorR, u8 ColorG, u8 ColorB) {
  if (X < 0 || Y < 0 || X >= FrameBuffer->Width || Y >= FrameBuffer->Height) {
    return;
  }

  // NOTE(lucas): Origin is at the bottom left corner!
  i8* Pixel = &FrameBuffer->Data[4 * (((FrameBuffer->Height - Y) * FrameBuffer->Width) + X)];
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

// NOTE(lucas): Triangles are drawn in counterclockwise order
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

v3 MatToV3(mat4 M) {
  v3 Result;

  Result = V3(M.Elements[0][0] / M.Elements[3][0], M.Elements[1][0] / M.Elements[3][0], M.Elements[2][0] / M.Elements[3][0]);

  return Result;
}

#define LightStrength 60
#define MODEL_SCALE 2500

static void DrawMesh(framebuffer* FrameBuffer, i32* ZBuffer, mesh* Mesh, v3 P, v3 Light) {
  for (u32 Index = 0; Index < Mesh->IndexCount; Index += 3) {
    v3 V[3];
    v3 L[3];
    v3 N[3];

    V[0] = Mesh->Vertices[Mesh->Indices[Index + 0]];
    V[1] = Mesh->Vertices[Mesh->Indices[Index + 1]];
    V[2] = Mesh->Vertices[Mesh->Indices[Index + 2]];

    N[0] = TransformApply(Proj, V[0]);
    N[1] = TransformApply(Proj, V[1]);
    N[2] = TransformApply(Proj, V[2]);

    L[0] = AddToV3(N[0], P);
    L[1] = AddToV3(N[1], P);
    L[2] = AddToV3(N[2], P);

#if 0
    printf("%g, %g; %g, %g; %g, %g\n", N[0].X, N[0].Y, N[1].X, N[1].Y, N[2].X, N[2].Y);
#endif

    float LightDistance = 1.0f / Distance(L[0], Light);
    float LightFactor = Clamp(LightDistance * LightStrength, 0, 1.0f);

    DrawFilledTriangleAt(FrameBuffer, ZBuffer, N[0], N[1], N[2], P,
      50 * LightFactor,
      80 * LightFactor,
      255 * LightFactor
    );
  }
}

i32 RendererInit(u32 Width, u32 Height) {
  (void)LoadImage; (void)StoreImage; (void)DrawLine; (void)DrawFilledTriangle;
 
  Proj = Perspective(1, 4 / 3.0f, 0.1f, 500);

  render_state* State = &RenderState;
  FrameBufferCreate(&State->FrameBuffer, Width, Height);
  State->ZBuffer = calloc(Width * Height, sizeof(float));

  WindowOpen(Width, Height, WINDOW_TITLE);

  return 0;
}

static void RendererSwapBuffers() {
  WindowSwapBuffers(&RenderState.FrameBuffer);
}

static void RendererClear(u8 ColorR, u8 ColorG, u8 ColorB) {
  FrameBufferClear(&RenderState.FrameBuffer, ColorR, ColorG, ColorB);
  memset(RenderState.ZBuffer, -1000, sizeof(i32) * RenderState.FrameBuffer.Width * RenderState.FrameBuffer.Height);
}

void RendererDestroy() {
  FrameBufferDestroy(&RenderState.FrameBuffer);
  free(RenderState.ZBuffer);
  WindowClose();
}
