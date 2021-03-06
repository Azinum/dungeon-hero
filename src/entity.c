// entity.c

static entity* EntityInit(entity* E, v3 P, mesh_id MeshId, texture_id TextureId) {
  memset(E, 0, sizeof(entity));
  E->P = P;
  E->MeshId = MeshId;
  E->TextureId = TextureId;
  E->Rotation = V3(0, 0, 0);
  E->Scaling = V3(1, 1, 1);
  return E;
}

static void UpdateAndDrawEntities(entity* Entities, u32 EntityCount, render_state* RenderState, assets* Assets, v3 Light, float LightStrength, camera* Camera) {
  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    v3 P = Entity->P;
    Entity->P.X += Entity->Speed.X * GameState.DeltaTime;
    Entity->P.Y += Entity->Speed.Y * GameState.DeltaTime;
    Entity->P.Z += Entity->Speed.Z * GameState.DeltaTime;

    switch (Entity->Type) {
      case ENTITY_ROTATOR: {
        Entity->Rotation.Y += 50.0f * GameState.DeltaTime;
        break;
      }
    }
    DrawMesh(RenderState, Assets, Entity->MeshId, Entity->TextureId, P, Light, LightStrength, Entity->Rotation, Entity->Scaling, Camera);
  }
}
