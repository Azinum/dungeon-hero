// software_renderer.c

static render_state RenderState;

static mat4 Proj;

#define DRAW_Z_BUFFER 0

static void FrameBufferCreate(framebuffer* FrameBuffer, u32 Width, u32 Height) {
  FrameBuffer->Data = malloc(Width * Height * 4);
  FrameBuffer->Width = Width;
  FrameBuffer->Height = Height;
}

static void FrameBufferClear(framebuffer* FrameBuffer, u8 ColorR, u8 ColorG, u8 ColorB) {
  color Zero = {ColorB, ColorG, ColorR, 0};
  color* Iter = FrameBuffer->Color;
  u32 Count = FrameBuffer->Width * FrameBuffer->Height;

  for (u32 Index = 0; Index < Count; ++Index) {
    *(Iter)++ = Zero;
  }
}

static void FrameBufferDestroy(framebuffer* FrameBuffer) {
  Assert(FrameBuffer);
  if (FrameBuffer->Data) {
    free(FrameBuffer->Data);
    FrameBuffer->Data = NULL;
    FrameBuffer->Width = 0;
    FrameBuffer->Height = 0;
  }
}

inline i32 Barycentric(v3 A, v3 B, v3 C) {
  i32 Result = 0;

  Result = (C.X - A.X) * (B.Y - A.Y) - (C.Y - A.Y) * (B.X - A.X);

  return Result;
}

inline void Barycentric_(v3 P, v3 A, v3 B, v3 C, float* W0, float* W1, float* W2) {
  v3 V0 = DifferenceV3(B, A);
  v3 V1 = DifferenceV3(C, A);
  v3 V2 = DifferenceV3(P, A);

  float D00 = DotVec3(V0, V0);
  float D01 = DotVec3(V0, V1);
  float D11 = DotVec3(V1, V1);
  float D20 = DotVec3(V2, V0);
  float D21 = DotVec3(V2, V1);

  float Denom = D00 * D11 - D01 * D01;
  *W1 = (D11 * D20 - D01 * D21) / Denom;
  *W2 = (D00 * D21 - D01 * D20) / Denom;
  *W0 = 1.0f - *W1 - *W2;
}

inline void DrawPixel(framebuffer* FrameBuffer, i32 X, i32 Y, color Color) {
  if (X <= 0 || Y <= 0 || X > FrameBuffer->Width || Y > FrameBuffer->Height) {
    return;
  }

  // NOTE(lucas): Origin is at the bottom left corner!
  color* Pixel = (color*)&FrameBuffer->Color[(((FrameBuffer->Height - Y) * FrameBuffer->Width) + X)];
  *Pixel = Color;
}

inline void DrawLine(framebuffer* FrameBuffer, v2 A, v2 B, u8 ColorR, u8 ColorG, u8 ColorB) {
#if 0
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
      // DrawPixel(FrameBuffer, Y, X, ColorR, ColorG, ColorB);
    }
    else {
      // DrawPixel(FrameBuffer, X, Y, ColorR, ColorG, ColorB);
    }
  }
#endif
}

// NOTE(lucas): Triangles are drawn in counterclockwise order
inline void DrawFilledTriangle(framebuffer* FrameBuffer, float* ZBuffer, v3 A, v3 B, v3 C, color Color, v2 T0, v2 T1, v2 T2, v3 V0, v3 V1, v3 V2, image* Texture) {
  if (A.X < 0 || A.Y < 0 || B.X < 0 || B.Y < 0 || C.X < 0 || C.Y < 0) {
    return;
  }

  i32 MinX = Min3(A.X, B.X, C.X);
  i32 MinY = Min3(A.Y, B.Y, C.Y);
  i32 MaxX = Max3(A.X, B.X, C.X);
  i32 MaxY = Max3(A.Y, B.Y, C.Y);

  MinX = Max(MinX, 0);
  MinY = Max(MinY, 0);
  MaxX = Min(MaxX, FrameBuffer->Width - 1);
  MaxY = Min(MaxY, FrameBuffer->Height - 1);

  v3 P = {0};
  for (P.Y = MinY; P.Y <= MaxY; P.Y++) {
    for (P.X = MinX; P.X <= MaxX; P.X++) {
      float W0 = 0.0f;
      float W1 = 0.0f;
      float W2 = 0.0f;
      Barycentric_(P, A, B, C, &W0, &W1, &W2);

      // NOTE(lucas): Are we inside the triangle?
      if (W0 >= 0 && W1 >= 0 && W2 >= 0) {
        float Z = 0;
        Z += A.Z * W0;
        Z += B.Z * W1;
        Z += C.Z * W2;

        i32 Index = ((FrameBuffer->Height - P.Y) * FrameBuffer->Width) - P.X;
        if (ZBuffer[Index] < Z) {
          ZBuffer[Index] = Z;
#if DRAW_Z_BUFFER
          u8 Value = Abs(ZBuffer[Index]);
          Color = (color) {Value, Value, Value, 255};
          DrawPixel(FrameBuffer, P.X, P.Y, Color);
#else
          DrawPixel(FrameBuffer, P.X, P.Y, Color);
#endif
        }
      }
    }
  }
}

#define LightStrength 4.0f
#define MODEL_SCALE 50

static void DrawMesh(framebuffer* FrameBuffer, float* ZBuffer, mesh* Mesh, image* Texture, v3 P, v3 Light) {
  for (u32 Index = 0; Index < Mesh->IndexCount; Index += 3) {
    v3 V[3];  // Vertices
    v2 T[3];  // UV coords
    v3 R[3];  // Resulting transformed vertices
    v3 Normal;

    V[0] = Mesh->Vertices[Mesh->Indices[Index + 0]];
    V[1] = Mesh->Vertices[Mesh->Indices[Index + 1]];
    V[2] = Mesh->Vertices[Mesh->Indices[Index + 2]];

    T[0] = Mesh->UV[Mesh->UVIndices[Index + 0]];
    T[1] = Mesh->UV[Mesh->UVIndices[Index + 1]];
    T[2] = Mesh->UV[Mesh->UVIndices[Index + 2]];

    Normal = Mesh->Normals[Mesh->NormalIndices[Index + 0]];

    R[0] = AddToV3(V[0], P);
    R[1] = AddToV3(V[1], P);
    R[2] = AddToV3(V[2], P);

    R[0] = MultiplyMatrixVector(Proj, R[0]);
    R[1] = MultiplyMatrixVector(Proj, R[1]);
    R[2] = MultiplyMatrixVector(Proj, R[2]);

    R[0].X += 1.0f; R[0].Y += 1.0f;
    R[1].X += 1.0f; R[1].Y += 1.0f;
    R[2].X += 1.0f; R[2].Y += 1.0f;

    R[0].X *= 0.5f * Win.Width; R[0].Y *= 0.5f * Win.Height;
    R[1].X *= 0.5f * Win.Width; R[1].Y *= 0.5f * Win.Height;
    R[2].X *= 0.5f * Win.Width; R[2].Y *= 0.5f * Win.Height;

    v3 LightNormal = NormalizeVec3(DifferenceV3(Light, R[0]));
    float LightFactor = Clamp(DotVec3(Normal, LightNormal) * LightStrength, 0, 1.0f);

    v3 CameraNormal = V3(0, 0, -1.0f);
    float DotValue = DotVec3(CameraNormal, Normal);
    if (DotValue < 0) {
      continue;
    }

    u8* At = &Texture->PixelBuffer[0 * 4];
    color Color = {
      *(At + 2),
      *(At + 1),
      *(At + 0),
      *(At + 3),
    };
    Color.R *= LightFactor;
    Color.G *= LightFactor;
    Color.B *= LightFactor;
    DrawFilledTriangle(FrameBuffer, ZBuffer, R[0], R[1], R[2], Color, T[0], T[1], T[2], V[0], V[1], V[2], Texture);
  }
}

i32 RendererInit(u32 Width, u32 Height) {
  (void)LoadImage; (void)StoreImage; (void)DrawLine;
  render_state* State = &RenderState;
  FrameBufferCreate(&State->FrameBuffer, Width, Height);
  State->ZBuffer = calloc(Width * Height, sizeof(float));

  WindowOpen(Width, Height, WINDOW_TITLE);
 
  Proj = Perspective(50, (float)Win.Width / Win.Height, 0.1f, 500);
  return 0;
}


static void RendererSwapBuffers() {
  WindowSwapBuffers(&RenderState.FrameBuffer);
}

static void ZBufferClear(float* ZBuffer, u32 Width, u32 Height) {
  u32 Count = Width * Height;
  float FillValue = -10000;
#if USE_SSE
  __m128* Dest = (__m128*)ZBuffer;
  __m128 Value = _mm_set_ps1(FillValue);
  u32 ChunkSize = 4;
  Count /= ChunkSize;
  for (u32 Index = 0; Index < Count; ++Index) {
    *(Dest++) = Value;
  }
#else
  for (u32 Index = 0; Index < Count; ++Index) {
    *(ZBuffer++) = FillValue;
  }
#endif
}

static void RendererClear(u8 ColorR, u8 ColorG, u8 ColorB) {
  FrameBufferClear(&RenderState.FrameBuffer, ColorR, ColorG, ColorB);
  ZBufferClear(RenderState.ZBuffer, RenderState.FrameBuffer.Width, RenderState.FrameBuffer.Height);
}

void RendererDestroy() {
  FrameBufferDestroy(&RenderState.FrameBuffer);
  free(RenderState.ZBuffer);
  WindowClose();
}
