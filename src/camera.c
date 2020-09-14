// camera.c

static camera Camera;

void CameraInit(camera* Camera, v3 Position) {
  Camera->P = Position;
}
