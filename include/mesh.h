// mesh.h

typedef struct mesh {
  v3* Vertices;
  u32 VertexCount;

  u32* Indices;
  u32 IndexCount;

  u32* NormalIndices;
  u32 NormalIndexCount;

  v3* Normals;
  u32 NormalCount;

  v2* UV;
  u32 UVCount;

} mesh;
