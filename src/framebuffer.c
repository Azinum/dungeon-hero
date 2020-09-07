// framebuffer.c

static void FrameBufferCreate(framebuffer* FrameBuffer, u32 Width, u32 Height) {
  FrameBuffer->Data = malloc(Width * Height * 4);
  FrameBuffer->Width = Width;
  FrameBuffer->Height = Height;
}

static void FrameBufferClear(framebuffer* FrameBuffer, u8 ColorR, u8 ColorG, u8 ColorB) {
  color Zero = {0};
  color* Iter = FrameBuffer->Color;
  u32 Count = FrameBuffer->Width * FrameBuffer->Height;

  for (u32 Index = 0; Index < Count; ++Index) {
    *(Iter)++ = Zero;
  }
}

static void FrameBufferDestroy(framebuffer* FrameBuffer) {
  Assert(FrameBuffer);
  if (FrameBuffer->Data) {
    free(FrameBuffer->Data);
    FrameBuffer->Data = NULL;
    FrameBuffer->Width = 0;
    FrameBuffer->Height = 0;
  }
}

