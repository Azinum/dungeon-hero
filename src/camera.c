// camera.c

static camera Camera;

#if 1
static float LastX = 0;
static float LastY = 0;
#endif

#define MOUSE_SENSITIVITY 0.05f
#define LOOK_SENSITIVITY 150.0f

void CameraInit(camera* Camera, v3 Position) {
  Camera->P = Position;
  Camera->TargetP = Position;
  Camera->Up = V3(0.0f, 1.0f, 0.0f);
  Camera->Right = V3(1.0f, 0.0f, 0.0f);
  Camera->Forward = V3(0.0f, 0.0f, 1.0f);
  Camera->Pitch = 0.0f;
  Camera->Yaw = 90.0f;

  Projection = Perspective(G_Fov, (float)Win.Width / Win.Height, 0.05f, 500);
}

void CameraUpdate(camera* Camera) {
  if (KeyDown[KEY_D]) {
    Camera->P = AddToV3(Camera->P, V3(
      Camera->Right.X * 5.0f * GameState.DeltaTime,
      Camera->Right.Y * 5.0f * GameState.DeltaTime,
      Camera->Right.Z * 5.0f * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_A]) {
    Camera->P = AddToV3(Camera->P, V3(
      -Camera->Right.X * 5.0f * GameState.DeltaTime,
      -Camera->Right.Y * 5.0f * GameState.DeltaTime,
      -Camera->Right.Z * 5.0f * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_W]) {
    Camera->P = AddToV3(Camera->P, V3(
      Camera->Forward.X * 5.0f * GameState.DeltaTime,
      Camera->Forward.Y * 5.0f * GameState.DeltaTime,
      Camera->Forward.Z * 5.0f * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_S]) {
    Camera->P = AddToV3(Camera->P, V3(
      -Camera->Forward.X * 5.0f * GameState.DeltaTime,
      -Camera->Forward.Y * 5.0f * GameState.DeltaTime,
      -Camera->Forward.Z * 5.0f * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_Z]) {
    Camera->P.Y += 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_X]) {
    Camera->P.Y -= 5.0f * GameState.DeltaTime;
  }

  if (KeyDown[KEY_H]) {
    Camera->Yaw -= LOOK_SENSITIVITY * GameState.DeltaTime;
  }
  if (KeyDown[KEY_L]) {
    Camera->Yaw += LOOK_SENSITIVITY * GameState.DeltaTime;
  }
  if (KeyDown[KEY_J]) {
    Camera->Pitch -= LOOK_SENSITIVITY * GameState.DeltaTime;
  }
  if (KeyDown[KEY_K]) {
    Camera->Pitch += LOOK_SENSITIVITY * GameState.DeltaTime;
  }

#if 1
  float XOffset = MouseX - LastX;
  float YOffset = MouseY - LastY;
  LastX = MouseX;
  LastY = MouseY;

  XOffset *= MOUSE_SENSITIVITY;
  YOffset *= -MOUSE_SENSITIVITY;

  Camera->Yaw += XOffset;
  Camera->Pitch += YOffset;
#endif

  if (Camera->Pitch >= 89.0f)
    Camera->Pitch = 89.0f;
  if (Camera->Pitch <= -89.0f)
    Camera->Pitch = -89.0f;

  v3 Direction = V3(
    cos(ToRadians(Camera->Yaw)) * cos(ToRadians(Camera->Pitch)),
    sin(ToRadians(Camera->Pitch)),
    sin(ToRadians(Camera->Yaw)) * cos(ToRadians(Camera->Pitch))
  );
  Camera->Forward = NormalizeVec3(Direction);
  v3 WorldUp = V3(0.0f, 1.0f, 0.0f);
  Camera->Right = NormalizeVec3(Cross(Camera->Forward, WorldUp));
  Camera->Up = NormalizeVec3(Cross(Camera->Right, Camera->Forward));
}
