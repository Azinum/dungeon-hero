// mesh.c

#define SScanf(RESULT, IterBuffer, Format, ...) {\
  u32 ReadBytes = 0;\
  RESULT = sscanf(IterBuffer, Format "%n", __VA_ARGS__, &ReadBytes);\
  IterBuffer += ReadBytes;\
}

static void MeshUnload(mesh* Mesh) {
  Assert(Mesh);
  ListFree(Mesh->Vertices, Mesh->VertexCount);
  ListFree(Mesh->Indices, Mesh->IndexCount);

  ListFree(Mesh->Normals, Mesh->NormalCount);
  ListFree(Mesh->NormalIndices, Mesh->NormalIndexCount);

  ListFree(Mesh->UV, Mesh->UVCount);
  ListFree(Mesh->UVIndices, Mesh->UVIndexCount);
}

static void MeshInit(mesh* Mesh) {
  memset(Mesh, 0, sizeof(mesh));
  Mesh->VertexCount = 0;
  Mesh->IndexCount = 0;
  Mesh->NormalCount = 0;
  Mesh->NormalIndexCount = 0;
  Mesh->UVCount = 0;
  Mesh->UVIndexCount = 0;
}

#define WORD_SIZE 128

static i32 MeshLoadOBJ(const char* Path, mesh* Mesh) {
  i32 Status = 0;

  buffer Buffer = {0};
  MeshInit(Mesh);

  ReadFile(Path, &Buffer);
  if (!Buffer.Data) {
    return -1;
  }
  char Word[WORD_SIZE] = {0};
  char* Iter = Buffer.Data;
  i32 Result = 0;

  do {
    SScanf(Result, Iter, "%s", Word);
    if (Result == EOF) {
      break;
    }

    // Vertex
    if (!strncmp(Word, "v", WORD_SIZE)) {
      v3 V;
      SScanf(Result, Iter, "%f %f %f", &V.X, &V.Y, &V.Z);
      ListPush(Mesh->Vertices, Mesh->VertexCount, V);
    }
    // Texture Vertex
    else if (!strncmp(Word, "vt", WORD_SIZE)) {
      v2 V;
      SScanf(Result, Iter, "%f %f", &V.X, &V.Y);
      ListPush(Mesh->UV, Mesh->UVCount, V);
    }
    // Vertex Normal
    else if (!strncmp(Word, "vn", WORD_SIZE)) {
      v3 V;
      SScanf(Result, Iter, "%f %f %f", &V.X, &V.Y, &V.Z);
      ListPush(Mesh->Normals, Mesh->NormalCount, V);
    }
    // Face
    else if (!strncmp(Word, "f", WORD_SIZE)) {
      i32 X[3], Y[3], Z[3];
      SScanf(
        Result,
        Iter,
        "%i/%i/%i %i/%i/%i %i/%i/%i",
        &X[0], &Y[0], &Z[0],
        &X[1], &Y[1], &Z[1],
        &X[2], &Y[2], &Z[2]
      );
      if (Result != 9) {
        fprintf(stderr, "Invalid OBJ file '%s'\n", Path);
        MeshUnload(Mesh);
        Status = -1;
        goto done;
      }
      // NOTE(lucas): Indexes start at 1 in OBJ files
      ListPush(Mesh->Indices, Mesh->IndexCount, X[0] - 1);
      ListPush(Mesh->Indices, Mesh->IndexCount, X[1] - 1);
      ListPush(Mesh->Indices, Mesh->IndexCount, X[2] - 1);

      ListPush(Mesh->UVIndices, Mesh->UVIndexCount, Y[0] - 1);
      ListPush(Mesh->UVIndices, Mesh->UVIndexCount, Y[1] - 1);
      ListPush(Mesh->UVIndices, Mesh->UVIndexCount, Y[2] - 1);

      ListPush(Mesh->NormalIndices, Mesh->NormalIndexCount, Z[0] - 1);
      ListPush(Mesh->NormalIndices, Mesh->NormalIndexCount, Z[1] - 1);
      ListPush(Mesh->NormalIndices, Mesh->NormalIndexCount, Z[2] - 1);
    }
  } while (1);

#if 0
  printf(
    "Mesh Loaded ('%s'):\n"
    "  Vertex Count: %i\n"
    "  Index Count: %i\n"
    "  Normal Count: %i\n"
    "  Normal Index Count: %i\n"
    "  UV Coords Count: %i\n"
    "  UV Index Count: %i\n"
    ,
    Path,
    Mesh->VertexCount,
    Mesh->IndexCount,
    Mesh->NormalCount,
    Mesh->NormalIndexCount,
    Mesh->UVCount,
    Mesh->UVIndexCount
  );
#endif

done:
  free(Buffer.Data);
  Buffer.Count = 0;

  return Status;
}

i32 MeshSortIndexedData(mesh* Mesh) {
  v2* UV = NULL;
  u32 UVCount = 0;
  ListAlloc(UV, UVCount, Mesh->IndexCount, V2(0, 0));

  v3* Normals = NULL;
  u32 NormalCount = 0;
  ListAlloc(Normals, NormalCount, Mesh->IndexCount, V3(0, 0, 0));

  for (u32 I = 0; I < Mesh->IndexCount; ++I) {
    u32 Index = Mesh->Indices[I];
    u32 UVIndex = Mesh->UVIndices[I];
    u32 NormalIndex = Mesh->NormalIndices[I];

    v2 CurrentUV = Mesh->UV[UVIndex];
    UV[Index] = CurrentUV;
    v3 CurrentNormal = Mesh->Normals[NormalIndex];
    Normals[Index] = CurrentNormal;
  }

  ListFree(Mesh->UV, Mesh->UVCount);
  ListFree(Mesh->Normals, Mesh->NormalCount);

  Mesh->UV = UV;
  Mesh->UVCount = UVCount;
  Mesh->Normals = Normals;
  Mesh->NormalCount = NormalCount;

  for (u32 I = 0; I < Mesh->IndexCount; ++I) {
    u32 Index = Mesh->Indices[I];
    v3 V = Mesh->Vertices[Index];
    v2 UV = Mesh->UV[Index];
    v3 N = Mesh->Normals[Index];
    // printf("[%i] Vertex: (%g, %g, %g), (%g, %g), (%g, %g, %g)\n", I, V.X, V.Y, V.Z, UV.X, UV.Y, N.X, N.Y, N.Z);
  }
  return 0;
}
