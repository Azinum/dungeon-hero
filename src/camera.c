// camera.c

static camera Camera;
static const v3 WorldUp = V3(0.0f, 1.0f, 0.0f);

static double LastX = 0;
static double LastY = 0;
static double XOffset = 0;
static double YOffset = 0;

#define MOVEMENT_SPEED 8.0f
#define LOOK_SENSITIVITY 150.0f

void CameraInit(camera* Camera, v3 Position) {
  Camera->P = Position;
  Camera->TargetP = Position;
  Camera->Up = V3(0.0f, 1.0f, 0.0f);
  Camera->Right = V3(1.0f, 0.0f, 0.0f);
  Camera->Forward = V3(0.0f, 0.0f, 1.0f);
  Camera->Pitch = 0.0f;
  Camera->Yaw = 90.0f;
  Projection = Perspective(G_Fov, (float)Win.Width / Win.Height, 0.01f, 100.0f);
  OrthoProjection = Orthographic(0.0f, (float)Win.Width, (float)Win.Height, 0, -1.0f, 1.0f);
}

void CameraUpdate(camera* Camera) {
  Camera->Right.Y = 0;
  Camera->Forward.Y = 0;

#if 1
  v3 Min = WorldMin;
  v3 Max = WorldMax;
  if (Camera->P.X < Min.X + 1) {
    Camera->P.X = Min.X + 1;
  }
  if (Camera->P.X > Max.X - 1) {
    Camera->P.X = Max.X - 1;
  }
  if (Camera->P.Z < Min.Z + 1) {
    Camera->P.Z = Min.Z + 1;
  }
  if (Camera->P.Z > Max.Z - 1) {
    Camera->P.Z = Max.Z - 1;
  }
#endif

  if (KeyDown[KEY_D]) {
    Camera->P = AddToV3(Camera->P, V3(
      Camera->Right.X * MOVEMENT_SPEED * GameState.DeltaTime,
      Camera->Right.Y * MOVEMENT_SPEED * GameState.DeltaTime,
      Camera->Right.Z * MOVEMENT_SPEED * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_A]) {
    Camera->P = AddToV3(Camera->P, V3(
      -Camera->Right.X * MOVEMENT_SPEED * GameState.DeltaTime,
      -Camera->Right.Y * MOVEMENT_SPEED * GameState.DeltaTime,
      -Camera->Right.Z * MOVEMENT_SPEED * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_W]) {
    Camera->P = AddToV3(Camera->P, V3(
      Camera->Forward.X * MOVEMENT_SPEED * GameState.DeltaTime,
      Camera->Forward.Y * MOVEMENT_SPEED * GameState.DeltaTime,
      Camera->Forward.Z * MOVEMENT_SPEED * GameState.DeltaTime
    ));
  }
  if (KeyDown[KEY_S]) {
    Camera->P = AddToV3(Camera->P, V3(
      -Camera->Forward.X * MOVEMENT_SPEED * GameState.DeltaTime,
      -Camera->Forward.Y * MOVEMENT_SPEED * GameState.DeltaTime,
      -Camera->Forward.Z * MOVEMENT_SPEED * GameState.DeltaTime
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
  Camera->Right = NormalizeVec3(Cross(Camera->Forward, WorldUp));
  Camera->Up = NormalizeVec3(Cross(Camera->Right, Camera->Forward));
}

void CameraUpdateViewMatrix(camera* Camera) {
  View = LookAt(Camera->P, AddToV3(Camera->P, Camera->Forward), Camera->Up);
}
