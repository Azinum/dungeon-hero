// misc.c

i32 ReadFile(const char* Path, buffer* Buffer) {
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
