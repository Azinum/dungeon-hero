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

  ListFree(Mesh->NormalIndices, Mesh->NormalIndexCount);

  ListFree(Mesh->Normals, Mesh->NormalCount);

  ListFree(Mesh->UV, Mesh->UVCount);
}

static i32 ReadFile(const char* Path, buffer* Buffer) {
  i32 Result = 0;

  FILE* File = fopen(Path, "rb");
  if (!File) {
    fprintf(stderr, "Failed to open file '%s'\n", Path);
    return -1;
  }

  fseek(File, 0, SEEK_END);
  u32 Size = ftell(File);
  fseek(File, 0, SEEK_SET);
  Buffer->Data = malloc(Size);
  Buffer->Count = Size;
  if (!Buffer->Data) {
    Buffer->Count = 0;
    Result = -1;
    goto done;
  }

  u32 BytesRead = fread(Buffer->Data, 1, Size, File);
  if (BytesRead != Size) {
    fprintf(stderr, "Failed to read file '%s'\n", Path);
    Result = -1;
    goto done;
  }

done:
  fclose(File);
  return Result;
}

static void MeshInit(mesh* Mesh) {
  memset(Mesh, 0, sizeof(mesh));
  Mesh->Vertices = NULL;
  Mesh->VertexCount = 0;
  Mesh->Indices = NULL;
  Mesh->IndexCount = 0;
}

#define WORD_SIZE 128

static i32 MeshLoadOBJ(const char* Path, mesh* Mesh) {
  i32 Status = 0;

  buffer Buffer = {0};
  MeshInit(Mesh);

  ReadFile(Path, &Buffer);
  u8 Word[WORD_SIZE] = {0};
  u8* Iter = Buffer.Data;
  i32 Result = 0;

  do {
    u32 BytesRead = 0;
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
    "  Normal Index Count: %i\n"
    "  Normal Count: %i\n"
    ,
    Path,
    Mesh->VertexCount,
    Mesh->IndexCount,
    Mesh->NormalIndexCount,
    Mesh->NormalCount
  );
#endif

done:
  free(Buffer.Data);
  Buffer.Count = 0;

  return Status;
}

