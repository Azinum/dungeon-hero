// mesh.h

typedef struct mesh {
  v3* Vertices;
  u32 VertexCount;
  u32* Indices;
  u32 IndexCount;

  v3* Normals;
  u32 NormalCount;
  u32* NormalIndices;
  u32 NormalIndexCount;

  v2* UV;
  u32 UVCount;
  u32* UVIndices;
  u32 UVIndexCount;
} mesh;
