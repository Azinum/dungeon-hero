// entity.c

static entity* EntityInit(entity* E, v3 P) {
  memset(E, 0, sizeof(entity));
  E->P = P;
  return E;
}

static void UpdateAndDrawEntities(entity* Entities, u32 EntityCount, framebuffer* FrameBuffer, i32* ZBuffer, mesh* Mesh, image* Texture, v3 Light) {
  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    v3 P = Entity->P;
#if 1
    Entity->P.X += Entity->Speed.X;
    Entity->P.Y += Entity->Speed.Y;

#define YMax 6

    if (P.Y < -YMax) {
      Entity->P.Y = -YMax;
      Entity->Speed.Y = -Entity->Speed.Y;
    }
    else if (P.Y > YMax) {
      Entity->P.Y = YMax;
      Entity->Speed.Y = -Entity->Speed.Y;
    }
#else
#endif
    DrawMesh(FrameBuffer, ZBuffer, Mesh, Texture, P, Light);
  }
}
