// entity.h

enum entity_type {
  ENTITY_NONE,
  ENTITY_ROTATOR,
};

typedef struct entity {
  v3 P;
  v3 Speed;
  v3 Scaling;
  v3 Rotation;

  u32 Type;

  u32 MeshId;
  u32 TextureId;
} entity;
