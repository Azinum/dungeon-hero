// entity.c

static entity* EntityInit(entity* E, v3 P, mesh_id MeshId, texture_id TextureId) {
  memset(E, 0, sizeof(entity));
  E->P = P;
  E->MeshId = MeshId;
  E->TextureId = TextureId;
  return E;
}

static void UpdateAndDrawEntities(entity* Entities, u32 EntityCount, framebuffer* FrameBuffer, float* ZBuffer, assets* Assets, v3 Light) {
  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    v3 P = Entity->P;
    Entity->P.X += Entity->Speed.X * GameState.DeltaTime;
    Entity->P.Y += Entity->Speed.Y * GameState.DeltaTime;
    Entity->P.Z += Entity->Speed.Z * GameState.DeltaTime;

#if 1
#define YMax 1
    if (P.Y < -YMax) {
      Entity->TextureId = TEXTURE_UV;
      Entity->MeshId = MESH_CUBE;
      Entity->P.Y = -YMax;
      Entity->Speed.Y = -Entity->Speed.Y;
    }
    else if (P.Y > YMax) {
      Entity->TextureId = TEXTURE_TEST;
      Entity->MeshId = MESH_STONE;
      Entity->P.Y = YMax;
      Entity->Speed.Y = -Entity->Speed.Y;
    }
#else
#endif
    mesh Mesh = Assets->Meshes[Entity->MeshId];
    image Texture = Assets->Textures[Entity->TextureId];
    DrawMesh(FrameBuffer, ZBuffer, &Mesh, &Texture, P, Light);
  }
}
