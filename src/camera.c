// camera.c

static camera Camera;

static double LastX = 0;
static double LastY = 0;
static double XOffset = 0;
static double YOffset = 0;

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

// TODO(lucas): There seem to be something off with the camera movement. Panning the camera to the left seems easier to do than to the right. Wat dis? Some floating point error thingy?
void CameraUpdate(camera* Camera) {
  Camera->Right.Y = 0;
  Camera->Forward.Y = 0;

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

#if 1
  // NOTE(lucas): Temporary!
  if (Camera->P.X < WorldMin.X + 1) {
    Camera->P.X = WorldMin.X + 1;
  }
  if (Camera->P.X > WorldMax.X - 1) {
    Camera->P.X = WorldMax.X - 1;
  }
  if (Camera->P.Z < WorldMin.Z + 1) {
    Camera->P.Z = WorldMin.Z + 1;
  }
  if (Camera->P.Z > WorldMax.Z - 1) {
    Camera->P.Z = WorldMax.Z - 1;
  }
#endif
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

  XOffset = MouseX - LastX;
  YOffset = LastY - MouseY;
  LastX = MouseX;
  LastY = MouseY;

  XOffset *= G_Sensitivity;
  YOffset *= G_Sensitivity;

  Camera->Yaw += XOffset;
  Camera->Pitch += YOffset;

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
  const v3 WorldUp = V3(0.0f, 1.0f, 0.0f);
  Camera->Right = NormalizeVec3(Cross(Camera->Forward, WorldUp));
  Camera->Up = NormalizeVec3(Cross(Camera->Right, Camera->Forward));
}
