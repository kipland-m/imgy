#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include "helpers.h"

typedef struct {
    int width;
    int height;
  } OutputSize;

/* imgy - Written by Kipland Melton
 *
 * 05/08/25
 */

void save_jpeg(unsigned char *resize_buffer, JSAMPARRAY row_pointers, char *outfilepath, OutputSize size ) {
  struct jpeg_compress_struct comp;
  jpeg_create_compress(&comp);

  struct jpeg_error_mgr jpeg_err;
  comp.err = jpeg_std_error(&jpeg_err); 

  comp.image_height = size.height;
  comp.image_width = size.width;
  comp.input_components = 3;

  printf("%p\n", row_pointers[0]);

  jpeg_stdio_dest(&comp, write_file(outfilepath));
  comp.in_color_space = JCS_RGB; 

  jpeg_set_defaults(&comp);
  jpeg_start_compress(&comp, TRUE);

  int i = 0; 
  while (comp.next_scanline < comp.image_height) {
    jpeg_write_scanlines(&comp, &row_pointers[i], 1);
    i++;
  }

  jpeg_finish_compress(&comp);
  jpeg_destroy_compress(&comp);
}


void resize_jpeg(struct jpeg_decompress_struct decomp, unsigned char *input_buffer, unsigned char *resize_buffer, char *outfilepath) {
  /*
   * Using Nearest Neighbor algorithm for resizing.
   */
  int source_x;
  int source_y;
  int offset;
  int resize_offset;
  float OUTPUT_WIDTH = 400.00; 
  float OUTPUT_HEIGHT = 400.00;

  int row_stride = OUTPUT_WIDTH * decomp.output_components;
  JSAMPARRAY row_pointers = malloc(OUTPUT_HEIGHT * sizeof(unsigned char *)); 

  /*
   * Precalculate source_x before nested while loop (source_x is row indepenent)
   */

  resize_buffer = malloc(OUTPUT_WIDTH * OUTPUT_HEIGHT * decomp.output_components);

  int i;
  int j;
  for (i = 0; i < OUTPUT_HEIGHT; i++) {
    row_pointers[i] = resize_buffer + (i * row_stride);

    source_y = round(i / OUTPUT_HEIGHT * decomp.output_height); 

    for (j = 0; j < OUTPUT_WIDTH; j++) {
      source_x = round(j / OUTPUT_WIDTH * decomp.output_width);

      offset = source_y * decomp.output_width * 3 + source_x * 3;
      resize_offset = i * row_stride + j * 3;

      resize_buffer[resize_offset] = input_buffer[offset];
      resize_buffer[resize_offset + 1] = input_buffer[offset + 1];
      resize_buffer[resize_offset + 2] = input_buffer[offset + 2];
    }
  }

  save_jpeg(resize_buffer, row_pointers, outfilepath); 
}


/* do_read_jpeg will handle reading any data related to our image.
 */
unsigned char *do_read_jpeg(struct jpeg_decompress_struct decomp, char *infilepath) {
  struct jpeg_error_mgr jpeg_err;
  decomp.err = jpeg_std_error(&jpeg_err); 

  int row_stride;
  unsigned char *full_buffer = NULL;
  JSAMPARRAY row_pointers = malloc(decomp.output_height * sizeof(unsigned char *)); 

  jpeg_stdio_src(&decomp, open_file(infilepath));

  (void) jpeg_read_header(&decomp, TRUE);
  /*
   * DEBUG
   */
  printf("%d\n", decomp.image_height);
  printf("%d\n", decomp.image_width);
  printf("%d\n", decomp.num_components); 

  (void) jpeg_start_decompress(&decomp);

  /* row_stride = the length of a row of pixels in our image.
   * a pixel will be represented by 3 sets of digits (R,G,B)
   *
   * ex. MoonKilledCthun.jpg (3360x2100)
   * total amount of pixels in row_stride: 3360 * 3 = 10080
   */
  row_stride = decomp.output_width * decomp.output_components;

  full_buffer = malloc(decomp.output_width * decomp.output_height * decomp.output_components);

  int i;
  for (i = 0; i < decomp.output_height; i++) {
    row_pointers[i] = full_buffer + (i * row_stride);
  }
 
  while (decomp.output_scanline < decomp.output_height) {
    (void) jpeg_read_scanlines(&decomp, &row_pointers[decomp.output_scanline], 1);
  }

  return full_buffer;
}


int main(int argc, char *argv[]) {
  usage_validation(argc);

  OutputSize size;
  parse_size_arg(argv[1], &size.width, &size.height);

  struct jpeg_decompress_struct decomp;
  jpeg_create_decompress(&decomp);
  unsigned char *input_buffer = do_read_jpeg(decomp, argv[2]);

  unsigned char *resize_buffer = NULL;
  JSAMPARRAY row_pointers = NULL;

  resize_jpeg(decomp, input_buffer, resize_buffer, argv[4]);

  return 0;
}
