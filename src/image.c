// image.c

#include <png.h>

// NOTE(lucas): Close file directly here or after the fact?
static i32 LoadPNGFromFile(FILE* File, image* Image) {
  i32 Result = 0;
  png_structp PNG;
  png_infop Info;

  u8 Signature[8];
  if (fread(Signature, 1, sizeof(Signature), File) < 8) {
    Result = -1;
    goto done;
  }

  if (png_sig_cmp(Signature, 0, 8)) {
    Result = -1;
    goto done;
  }

  PNG = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!PNG) {
    Result = -1;
    goto done;
  }

  Info = png_create_info_struct(PNG);
  if (!Info) {
    Result = -1;
    goto done;
  }
  if (setjmp(png_jmpbuf(PNG))) {
    png_destroy_read_struct(&PNG, &Info, NULL);
    Result = -1;
    goto done;
  }

  png_init_io(PNG, File);
  png_set_sig_bytes(PNG, sizeof(Signature));
  png_read_info(PNG, Info);

  Image->Width = png_get_image_width(PNG, Info);
  Image->Height = png_get_image_height(PNG, Info);
  Image->Depth = png_get_bit_depth(PNG, Info);

  if (Image->Depth < 8) {
    png_set_packing(PNG);
  }

  if (png_get_valid(PNG, Info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(PNG);
  }

  // TODO(lucas): Support more color types when need be!
  switch (png_get_color_type(PNG, Info)) {
    case PNG_COLOR_TYPE_GRAY: {
      Image->Pitch = 3 * Image->Width;
      png_set_gray_to_rgb(PNG);
      break;
    }
    case PNG_COLOR_TYPE_RGB: {
      Image->Pitch = 3 * Image->Width;
      break;
    }
    case PNG_COLOR_TYPE_RGBA: {
      Image->Pitch = 4 * Image->Width;
      break;
    }
    default:
      Assert(0);
  }

  Image->BytesPerPixel = png_get_rowbytes(PNG, Info) / Image->Width;

  png_set_interlace_handling(PNG);
  png_read_update_info(PNG, Info);

  Image->PixelBuffer = malloc(sizeof(u8) * Image->Width * Image->Height * Image->BytesPerPixel);

  png_bytep* Rows = malloc(sizeof(png_bytep) * Image->Height);
  u8* Pixels = Image->PixelBuffer;
  for (u32 Row = 0; Row < Image->Height; ++Row) {
    Rows[Row] = Pixels;
    Pixels += Image->Width * Image->BytesPerPixel;
  }

  png_read_image(PNG, Rows);
  png_read_end(PNG, NULL);
  png_destroy_read_struct(&PNG, &Info, NULL);
  free(Rows);

done:
  fclose(File);
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
  png_set_filler(png, 0, PNG_FILLER_AFTER); // NOTE(lucas): This removes the alpha channel

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
  memset(Image, 0, sizeof(image));
  FILE* File = fopen(Path, "rb");
  if (!File) {
    fprintf(stderr, "Failed to open file '%s'\n", Path);
    return -1;
  }

  Result = LoadPNGFromFile(File, Image);

  return Result;
}

static i32 StoreImage(const char* Path, image* Image) {
  i32 Result = 0;

  Assert(Image);
  Result = StorePNGFromSource(Path, Image);

  return Result;
}

static void UnloadImage(image* Image) {
  Assert(Image);
  if (Image->PixelBuffer != NULL) {
    free(Image->PixelBuffer);
  }
  memset(Image, 0, sizeof(image));
}
