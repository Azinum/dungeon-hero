// camera.c

static camera Camera;

static i32 LastX = 0;
static i32 LastY = 0;
#define MOUSE_SENSITIVITY 0.5f

void CameraInit(camera* Camera, v3 Position) {
  Camera->P = Position;
  Camera->TargetP = Position;
  Camera->Up = V3(0.0f, 1.0f, 0.0f);
  Camera->Forward = V3(0.0f, 0.0f, 1.0f);
  Camera->Pitch = 0.0f;
  Camera->Yaw = 90.0f;
}

void CameraUpdate(camera* Camera) {
  if (KeyDown[KEY_D]) {
    Camera->P.X += 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_A]) {
    Camera->P.X -= 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_W]) {
    Camera->P.Z -= 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_S]) {
    Camera->P.Z += 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_Z]) {
    Camera->P.Y -= 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_X]) {
    Camera->P.Y += 5.0f * GameState.DeltaTime;
  }

  if (KeyDown[KEY_1]) {
    Camera->Yaw -= 10.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_2]) {
    Camera->Yaw += 10.0f * GameState.DeltaTime;
  }

  float XOffset = MouseX - LastX;
  float YOffset = MouseY - LastY;

#if 0
  XOffset *= MOUSE_SENSITIVITY;
  YOffset *= MOUSE_SENSITIVITY;

  Camera->Yaw += XOffset;
  Camera->Pitch += YOffset;
#endif

  // Camera->Yaw += 12.0f * GameState.DeltaTime;

  if (Camera->Pitch >= 89.0f)
    Camera->Pitch = 90.0f;
  if (Camera->Pitch <= -89.0f)
    Camera->Pitch = -90.0f;

  v3 Direction = V3(
    cos(ToRadians(Camera->Yaw)) * cos(ToRadians(Camera->Pitch)),
    sin(ToRadians(Camera->Pitch)),
    sin(ToRadians(Camera->Yaw)) * cos(ToRadians(Camera->Pitch))
  );
  Camera->Forward = NormalizeVec3(Direction);
  // printf("Forward: %g, %g, %g\n", Camera->Forward.X, Camera->Forward.Y, Camera->Forward.Z);
}
