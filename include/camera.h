// camera.h

typedef struct camera {
  v3 P;
  v3 TargetP;
  v3 Up;
  v3 Forward;

  float Pitch;
  float Yaw;
} camera;

