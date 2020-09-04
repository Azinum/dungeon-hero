// image.c

#include <png.h>

static i32 LoadPNGFromSource(u8* Source, u32 Size, image* Image) {
  i32 Result = 0;

  (void)Source; (void)Size; (void)Image;

  return Result;
}

static i32 StorePNGFromSource(const char* Path, image* Image) {
  i32 Result = -1;

  FILE* File = fopen(Path, "wb");
  if (!File) {
    fprintf(stderr, "Failed to open file '%s'\n", Path);
    goto done;
  }

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    goto done;
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
    goto done;

  if (setjmp(png_jmpbuf(png)))
    goto done;

  png_init_io(png, File);

  png_set_IHDR(
    png,
    info,
    Image->Width, Image->Height,
    8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_BASE
  );
  png_set_filter(png, PNG_FILTER_TYPE_BASE, PNG_FILTER_NONE);

  png_write_info(png, info);
  png_set_filler(png, 0, PNG_FILLER_AFTER); // NOTE(lucas): Remove the alpha channel

  png_bytep row = (png_bytep)malloc(4 * Image->Width * sizeof(png_byte));

  for (u32 Y = 0; Y < Image->Height; ++Y) {
    for (u32 X = 0; X < Image->Width; ++X) {
      png_byte* Pixel = &(row[X * 4]);
      memcpy(Pixel, &Image->PixelBuffer[Image->BytesPerPixel * ((Y * Image->Width) + X)], Image->BytesPerPixel);
    }
    png_write_row(png, row);
  }

  free(row);
  png_write_end(png, NULL);

  Result = 0;
done:
  if (Result != 0) {
    fprintf(stderr, "Failed to write PNG file '%s'\n", Path);
  }
  if (png && info) {
    png_destroy_write_struct(&png, &info);
  }
  fclose(File);
  return Result;
}

static i32 LoadImage(const char* Path, image* Image) {
  i32 Result = 0;
  Assert(Image);

  u8* Source = NULL;

  (void)Path;

  Result = LoadPNGFromSource(Source, 0, Image);

  return Result;
}

static i32 StoreImage(const char* Path, image* Image) {
  i32 Result = 0;

  Assert(Image);
  Result = StorePNGFromSource(Path, Image);

  return Result;
}
