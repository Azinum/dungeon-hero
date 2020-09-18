// camera.c

static camera Camera;

void CameraInit(camera* Camera, v3 Position) {
  Camera->P = Position;
  Camera->TargetP = Position;
  Camera->Up = V3(0.0f, 1.0f, 0.0f);
  Camera->Forward = V3(0.0f, 0.0f, 1.0f);
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
}
