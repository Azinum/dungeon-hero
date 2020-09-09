// math_util.h

#ifndef NO_SSE

#if __SSE__
#define USE_SSE 1
#endif

#endif

#if USE_SSE
#include <xmmintrin.h>
#endif

#define Translate2D(MODEL, X, Y) { \
  MODEL = MultiplyMat4(MODEL, Translate(V3(X, Y, 0))); \
}

#define Rotate2D(MODEL, ANGLE) { \
  MODEL = MultiplyMat4(MODEL, Rotate(ANGLE, V3(0, 0, 1))); \
}

#define Scale2D(MODEL, X, Y) { \
  MODEL = MultiplyMat4(MODEL, Scale(V3(X, Y, 1))); \
}
typedef union mat4 {
  float Elements[4][4];
#if USE_SSE
  __m128 Rows[4];
#endif
} mat4;

inline mat4 Translate(v3 T) {
  mat4 Result = {0};

  Result.Elements[3][0] = T.X;
  Result.Elements[3][1] = T.X;
  Result.Elements[3][2] = T.X;

  return Result;
}

inline mat4 TranslateMat4(mat4 A, v3 T) {
  mat4 Result = A;

  Result.Elements[3][0] += T.X;
  Result.Elements[3][1] += T.Y;
  Result.Elements[3][2] += T.Z;

  return Result;
}

inline v3 TransformApply(mat4 M, v3 A) {
  v3 Result;
  float X = A.X, Y = A.Y, Z = A.Z;

  Result.X = X * M.Elements[0][0] + Y * M.Elements[1][0] + Z * M.Elements[2][0] + 1 * M.Elements[3][0];
  Result.Y = X * M.Elements[0][1] + Y * M.Elements[1][1] + Z * M.Elements[2][1] + 1 * M.Elements[3][1];
  Result.Z = X * M.Elements[0][2] + Y * M.Elements[1][2] + Z * M.Elements[2][2] + 1 * M.Elements[3][2];

  return Result;
}

inline float DotVec3(v3 A, v3 B) {
  float Result = 0;

  Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);

  return Result;
}

inline float LengthSquareVec3(v3 A) {
  float Result = 0;

  Result = DotVec3(A, A);

  return Result;
}

inline float LengthVec3(v3 A) {
  float Result = 0;

  Result = sqrtf(LengthSquareVec3(A));

  return Result;
}

inline v3 NormalizeVec3(v3 A) {
  v3 Result = {0};

  float Length = LengthVec3(A);

  if (Length != 0) {
    Result.X = A.X * (1.0f / Length);
    Result.Y = A.Y * (1.0f / Length);
    Result.Z = A.Z * (1.0f / Length);
  }

  return Result;
}

#if USE_SSE

inline mat4 Transpose(mat4 A) {
  mat4 Result = A;

  _MM_TRANSPOSE4_PS(Result.Rows[0], Result.Rows[1], Result.Rows[2], Result.Rows[3]);

  return Result;
}

inline __m128 LinearCombineSSE(__m128 Left, mat4 Right) {
  __m128 Result;

  Result = _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x00), Right.Rows[0]);
  Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x55), Right.Rows[1]));
  Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xaa), Right.Rows[2]));
  Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xff), Right.Rows[3]));

  return Result;
}

#endif  // USE_SSE

inline v3 MultiplyV3(v3 A, float Value) {
  v3 Result = A;

  Result.X *= Value;
  Result.Y *= Value;
  Result.Z *= Value;

  return Result;
}

inline v3 AddV3(v3 A, float Value) {
  v3 Result = A;

  Result.X += Value;
  Result.Y += Value;
  Result.Z += Value;

  return Result;
}

inline v3 AddToV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X += B.X;
  Result.Y += B.Y;
  Result.Z += B.Z;

  return Result;
}

inline v3 SubToV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X -= B.X;
  Result.Y -= B.Y;
  Result.Z -= B.Z;

  return Result;
}

inline mat4 Mat4D(float Diagonal) {
  mat4 Result = {0};

  Result.Elements[0][0] = Diagonal;
  Result.Elements[1][1] = Diagonal;
  Result.Elements[2][2] = Diagonal;
  Result.Elements[3][3] = Diagonal;

  return Result;
}

inline mat4 MultiplyMat4(mat4 A, mat4 B) {
  mat4 Result;

#if USE_SSE

  mat4 Left = Transpose(A);
  mat4 Right = Transpose(B);

  Result.Rows[0] = LinearCombineSSE(Left.Rows[0], Right);
  Result.Rows[1] = LinearCombineSSE(Left.Rows[1], Right);
  Result.Rows[2] = LinearCombineSSE(Left.Rows[2], Right);
  Result.Rows[3] = LinearCombineSSE(Left.Rows[3], Right);

#else
  for (i32 Column = 0; Column < 4; ++Column) {
    for (i32 Row = 0; Row < 4; ++Rows) {
      float Sum = 0;
      i32 CurrentMatrice;
      for (CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice) {
        Sum += A.Elements[CurrentMatrice][Row] * B.Elements[Column][CurrentMatrice];
      }
      Result.Elements[Column][Row] = Sum;
    }
  }
#endif
  return Result;
}

inline mat4 Rotate(float Angle, v3 Axis) {
  mat4 Result = Mat4D(1.0f);

  Axis = NormalizeVec3(Axis);

  return Result;
}

inline mat4 Scale(v3 A) {
  mat4 Result = Mat4D(1.0f);

  Result.Elements[0][0] = A.X;
  Result.Elements[1][1] = A.Y;
  Result.Elements[2][2] = A.Z;

  return Result;
}

inline mat4 Perspective(float Fov, float AspectRatio, float ZNear, float ZFar) {
  mat4 Result = {0};

  float TanThetaOver2 = tanf(Fov * (PI32 / 360.0f));

  Result.Elements[0][0] = 1.0f / TanThetaOver2;
  Result.Elements[1][1] = AspectRatio / TanThetaOver2;
  Result.Elements[2][3] = -1.0f;
  Result.Elements[2][2] = (ZNear + ZFar) / (ZNear - ZFar);
  Result.Elements[3][2] = (2.0f * ZNear * ZFar) / (ZNear - ZFar);
  Result.Elements[3][3] = 0.0f;

  return Result;
}

inline float ClampMax(float Value, float MaxValue) {
  if (Value > MaxValue) {
    return MaxValue;
  }
  return Value;
}

inline float Clamp(float Value, float MinValue, float MaxValue) {
  if (Value > MaxValue) {
    return MaxValue;
  }
  if (Value < MinValue) {
    return MinValue;
  }
  return Value;
}

inline float InnerV2(v2 A, v2 B) {
  float Result = 0;

  Result = (A.X * B.X) + (A.Y * B.Y);

  return Result;
}

inline v3 DifferenceV3(v3 A, v3 B) {
  v3 Result = A;

  Result.X -= B.X;
  Result.Y -= B.Y;
  Result.Z -= B.Z;

  return Result;
}

inline float Power(float A, float B) {
  float Result = 0;

  Result = (float)pow(A, B);

  return Result;
}

inline float SquareRoot(float A) {
  float Result = 0;
#if USE_SSE
  Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(A)));
#else
  Result = sqrt(A);
#endif
  return Result;
}

// NOTE(lucas): This only calculates the X and Y values
inline float Distance(v3 A, v3 B) {
  float Result = 0;
  v3 Delta = DifferenceV3(A, B);

  Result = SquareRoot(Power(Delta.X, 2) + Power(Delta.Y, 2));

  return Result;
}
