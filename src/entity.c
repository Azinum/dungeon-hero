// entity.c

static entity* EntityInit(entity* E, v2 P) {
  E->P = P;
}

static void UpdateAndDrawEntities(entity* Entities, u32 EntityCount, framebuffer* FrameBuffer) {
  triangle T = {
    .C = V2(20, 0),
    .B = V2(50, 30),
    .A = V2(0, 50),
  };

  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    Entity->P.X += Entity->Speed.X;
    Entity->P.Y += Entity->Speed.Y;

    v2 P = Entity->P;
    if (P.X <= 0 || P.X >= FrameBuffer->Width)
      Entity->Speed.X = -Entity->Speed.X;
    if (P.Y <= 0 || P.Y >= FrameBuffer->Height)
      Entity->Speed.Y = -Entity->Speed.Y;

    DrawFilledTriangle(FrameBuffer,
      V2(T.A.X + P.X, T.A.Y + P.Y),
      V2(T.B.X + P.X, T.B.Y + P.Y),
      V2(T.C.X + P.X, T.B.Y + P.Y),
      255 * (1.0f / Entity->Speed.X) * 100,
      255 * (1.0f / Entity->Speed.X) * 20,
      255 * (1.0f / Entity->Speed.X) * 10);
  }
}
