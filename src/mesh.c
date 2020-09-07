// mesh.c

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
  buffer Buffer = {0};
  MeshInit(Mesh);

  ReadFile(Path, &Buffer);
  u8 Word[WORD_SIZE] = {0};
  u8* Iter = Buffer.Data;

  do {
    i32 Result = sscanf(Iter, "%s", Word);
    Iter += Result;
    if (Result == EOF) {
      break;
    }

    // Vertex
    if (!strncmp(Word, "v", WORD_SIZE)) {
      v3 V;
      Iter += sscanf(Iter, "%f %f %f\n", &V.X, &V.Y, &V.Z);
      ListPush(Mesh->Vertices, Mesh->VertexCount, V);
    }
    // Texture Vertex
    else if (!strncmp(Word, "vt", WORD_SIZE)) {
      v2 V;
      Iter += sscanf(Iter, "%f %f\n", &V.X, &V.Y);
    }
    // Vertex Normal
    else if (!strncmp(Word, "vn", WORD_SIZE)) {
      v3 V;
      Iter += sscanf(Iter, "%f %f %f\n", &V.X, &V.Y, &V.Z);
    }
    // Face
    else if (!strncmp(Word, "f", WORD_SIZE)) {
      i32 X[3], Y[3], Z[3];
      Iter += sscanf(
        Iter,
        "%i/%i/%i %i/%i/%i %i/%i/%i\n",
        &X[0], &Y[0], &Z[0],
        &X[1], &Y[1], &Z[1],
        &X[2], &Y[2], &Z[2]
      );
    }
  } while (1);

#if 0
  printf(
    "Mesh Loaded ('%s'):\n"
    "  Vertex Count: %i\n"
    "  Index Count: %i\n"
    ,
    Path,
    Mesh->VertexCount,
    Mesh->IndexCount
  );
#endif

  free(Buffer.Data);
  Buffer.Count = 0;

  return 0;
}

static void MeshUnload(mesh* Mesh) {
  Assert(Mesh);
  ListFree(Mesh->Vertices, Mesh->VertexCount);
}
