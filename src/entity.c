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
#if 0
  if (KeyPressed[KEY_D]) {
    Camera->TargetP.X += 1;
  }
  if (KeyPressed[KEY_A]) {
    Camera->TargetP.X -= 1;
  }
  if (KeyPressed[KEY_W]) {
    Camera->TargetP.Z -= 1;
  }
  if (KeyPressed[KEY_S]) {
    Camera->TargetP.Z += 1;
  }
  if (KeyPressed[KEY_Z]) {
    Camera->TargetP.Y += 1;
  }
  if (KeyPressed[KEY_X]) {
    Camera->TargetP.Y -= 1;
  }

  Camera->P.X = Lerp(Camera->P.X, Camera->TargetP.X, 10.0f * GameState.DeltaTime);
  Camera->P.Y = Lerp(Camera->P.Y, Camera->TargetP.Y, 10.0f * GameState.DeltaTime);
  Camera->P.Z = Lerp(Camera->P.Z, Camera->TargetP.Z, 10.0f * GameState.DeltaTime);
#else

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
    Camera->P.Y += 5.0f * GameState.DeltaTime;
  }
  if (KeyDown[KEY_X]) {
    Camera->P.Y -= 5.0f * GameState.DeltaTime;
  }
#endif
  for (u32 EntityIndex = 0; EntityIndex < EntityCount; ++EntityIndex) {
    entity* Entity = &Entities[EntityIndex];
    v3 P = Entity->P;
    Entity->P.X += Entity->Speed.X * GameState.DeltaTime;
    Entity->P.Y += Entity->Speed.Y * GameState.DeltaTime;
    Entity->P.Z += Entity->Speed.Z * GameState.DeltaTime;

    switch (Entity->Type) {
      case ENTITY_ROTATOR: {
        Entity->Rotation += 25.0f * GameState.DeltaTime;
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
    DrawMesh(FrameBuffer, ZBuffer, &Mesh, &Texture, P, Light, Entity->Rotation, Entity->Scaling);
  }
}
