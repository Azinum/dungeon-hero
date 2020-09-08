// entity.c

static entity* EntityInit(entity* E, v3 P) {
  memset(E, 0, sizeof(entity));
  E->P = P;
}

static void UpdateAndDrawEntities(entity* Entities, u32 EntityCount, framebuffer* FrameBuffer, i32* ZBuffer, mesh* Mesh, v2 Light) {
  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    Entity->P.X += Entity->Speed.X;
    Entity->P.Y += Entity->Speed.Y;

    v3 P = Entity->P;
    if (P.X <= 0 || P.X >= FrameBuffer->Width)
      Entity->Speed.X = -Entity->Speed.X;
    if (P.Y <= 0 || P.Y >= FrameBuffer->Height)
      Entity->Speed.Y = -Entity->Speed.Y;

    DrawMesh(FrameBuffer, ZBuffer, Mesh, P, Light);
  }
}
