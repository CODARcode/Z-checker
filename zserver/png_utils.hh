#include <cassert>
#if HAVE_PNG
#include <png.h>
#endif

struct png_mem_buffer {
  char *buffer = NULL; 
  size_t size = 0;
};

#if HAVE_PNG
static void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  /* with libpng15 next line causes pointer deference error; use libpng12 */
  struct png_mem_buffer* p=(struct png_mem_buffer*)png_get_io_ptr(png_ptr); /* was png_ptr->io_ptr */
  size_t nsize = p->size + length;

  /* allocate or grow buffer */
  if(p->buffer)
    p->buffer = (char*)realloc(p->buffer, nsize);
  else
    p->buffer = (char*)malloc(nsize);

  if(!p->buffer)
    png_error(png_ptr, "Write Error");

  memcpy(p->buffer + p->size, data, length); // copy new bytes to the end of buffer
  p->size += length;
}

static png_mem_buffer save_png(int width, int height,
                     int bitdepth, int colortype,
                     unsigned char* data, int pitch, int transform)
{
  int i = 0;
  int r = 0;

  assert(data);
  assert(pitch);

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  assert(png_ptr);

  png_infop info_ptr = png_create_info_struct(png_ptr);
  assert(info_ptr);

  png_set_IHDR(png_ptr,
      info_ptr,
      width,
      height,
      bitdepth,                 /* e.g. 8 */
      colortype,                /* PNG_COLOR_TYPE_{GRAY, PALETTE, RGB, RGB_ALPHA, GRAY_ALPHA, RGBA, GA} */
      PNG_INTERLACE_NONE,       /* PNG_INTERLACE_{NONE, ADAM7 } */
      PNG_COMPRESSION_TYPE_BASE,
      PNG_FILTER_TYPE_BASE);

  png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

  for (i = 0; i < height; ++i) {
    row_pointers[i] = data + (height - i - 1) * pitch;
  }

  struct png_mem_buffer state;
  state.buffer = NULL;
  state.size = 0;

  png_set_write_fn(png_ptr, &state, my_png_write_data, NULL);
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, transform, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(row_pointers);

  return state;
}
#endif

