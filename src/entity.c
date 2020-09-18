// entity.c

static entity* EntityInit(entity* E, v3 P, mesh_id MeshId, texture_id TextureId) {
  memset(E, 0, sizeof(entity));
  E->P = P;
  E->MeshId = MeshId;
  E->TextureId = TextureId;
  E->Rotation = 0;
  E->Scaling = V3(1, 1, 1);
  return E;
}

static void UpdateAndDrawEntities(entity* Entities, u32 EntityCount, framebuffer* FrameBuffer, float* ZBuffer, assets* Assets, v3 Light, camera* Camera) {
  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    v3 P = Entity->P;
    Entity->P.X += Entity->Speed.X * GameState.DeltaTime;
    Entity->P.Y += Entity->Speed.Y * GameState.DeltaTime;
    Entity->P.Z += Entity->Speed.Z * GameState.DeltaTime;

    switch (Entity->Type) {
      case ENTITY_ROTATOR: {
        Entity->Rotation += 50.0f * GameState.DeltaTime;
        break;
      }
    }
#if 1
#define YMax 1
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
    mesh Mesh = Assets->Meshes[Entity->MeshId];
    image Texture = Assets->Textures[Entity->TextureId];
    DrawMesh(FrameBuffer, ZBuffer, &Mesh, &Texture, P, Light, Entity->Rotation, Entity->Scaling, Camera);
  }
}
