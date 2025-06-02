#include "helpers.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>

struct Dimensions {
    int width;
    int height;
  };

void save_jpeg(unsigned char *resize_buffer,
                JSAMPARRAY row_pointers, char *outfilepath,
                float resize_width, float resize_height) {

  struct jpeg_compress_struct comp;
  jpeg_create_compress(&comp);

  struct jpeg_error_mgr jpeg_err;
  comp.err = jpeg_std_error(&jpeg_err); 

  comp.image_height = resize_height;
  comp.image_width = resize_width;
  comp.input_components = 3;

  jpeg_stdio_dest(&comp, write_file(outfilepath));
  comp.in_color_space = JCS_RGB; 

  jpeg_set_defaults(&comp);
  jpeg_start_compress(&comp, TRUE);

  while (comp.next_scanline < comp.image_height) {
    jpeg_write_scanlines(&comp, &row_pointers[comp.next_scanline], 1);
  }

  jpeg_finish_compress(&comp);
  jpeg_destroy_compress(&comp);
}


void resize_jpeg(struct jpeg_decompress_struct *decomp,
                  unsigned char *input_buffer, char *outfilepath,
                  int arg_width, int arg_height) {

  int OUTPUT_WIDTH = arg_width;  
  int OUTPUT_HEIGHT = arg_height;

  int i;
  int j;
  float source_x;
  float source_y;
  int offset;
  int resize_offset;
  float source_x_array[OUTPUT_WIDTH];

  unsigned char *resize_buffer = NULL;
  int row_stride = OUTPUT_WIDTH * decomp->output_components;
  JSAMPARRAY row_pointers = malloc(OUTPUT_HEIGHT * sizeof(unsigned char *)); 

  resize_buffer = malloc(OUTPUT_WIDTH * OUTPUT_HEIGHT * decomp->output_components);

  for (i = 0; i < OUTPUT_WIDTH; i++) {
    source_x_array[i] = round((float)i / OUTPUT_WIDTH * decomp->output_width);
    printf("%f\n", source_x_array[i]);
  }

  for (i = 0; i < OUTPUT_HEIGHT; i++) {
    row_pointers[i] = resize_buffer + (i * row_stride);

    source_y = round((float)i / OUTPUT_HEIGHT * decomp->output_height); 

    for (j = 0; j < OUTPUT_WIDTH; j++) {
      offset = source_y * decomp->output_width * 3 + source_x_array[j] * 3;
      resize_offset = i * row_stride + j * 3;

      resize_buffer[resize_offset] = input_buffer[offset];
      resize_buffer[resize_offset + 1] = input_buffer[offset + 1];
      resize_buffer[resize_offset + 2] = input_buffer[offset + 2];
    }
  }

  save_jpeg(resize_buffer, row_pointers, outfilepath, OUTPUT_WIDTH, OUTPUT_HEIGHT);
}


void do_read_jpeg(struct jpeg_decompress_struct *decomp,
                  char *infilepath, unsigned char **full_buffer) {

  struct jpeg_error_mgr jpeg_err;
  decomp->err = jpeg_std_error(&jpeg_err); 

  int i;
  int row_stride;
  JSAMPARRAY row_pointers = malloc(decomp->output_height * sizeof(unsigned char *));  

  jpeg_stdio_src(decomp, open_file(infilepath));
  (void) jpeg_read_header(decomp, TRUE);
  (void) jpeg_start_decompress(decomp);

  row_stride = decomp->output_width * decomp->output_components;

  *full_buffer = malloc(decomp->output_width * decomp->output_height * decomp->output_components);

  for (i = 0; i < decomp->output_height; i++) {
    row_pointers[i] = *full_buffer + (i * row_stride);
  }
  while (decomp->output_scanline < decomp->output_height) {
    (void) jpeg_read_scanlines(decomp, &row_pointers[decomp->output_scanline], 1);
  }

}

int main(int argc, char *argv[]) {
  /*
   * The 3 main functions that make up the program:
   * do_read_jpeg:
   *    struct jpeg_decompress_struct decomp, char *infilepath, char *outfilepath
   * resize_jpeg:
   *    struct jpeg_decompress_struct decomp, unsigned char *input_buffer, char *outfilepath
   * save_jpeg:
   *    unsigned char *resize_buffer, JSAMPARRAY row_pointers, char *outfilepath, float resize_width, float resize_height
   */
  
  struct Dimensions arg_dimensions;
  struct jpeg_decompress_struct decomp;

  jpeg_create_decompress(&decomp);

  unsigned char *full_buffer = NULL;
  
  usage_validation(argc);
  parse_size_arg(argv[1], &arg_dimensions.width, &arg_dimensions.height);

  (void) do_read_jpeg(&decomp, argv[2], &full_buffer);
  (void) resize_jpeg(&decomp, full_buffer, argv[3], arg_dimensions.width, arg_dimensions.height);

  printf("%d\n", decomp.output_width);

  printf("pixel 0: R%d G%d B%d\n", full_buffer[0], full_buffer[1], full_buffer[2]); /* first pixel first row */
  printf("pixel 3360: R%d G%d B%d\n", full_buffer[10078], full_buffer[10079], full_buffer[10080]); /* last pixel first row */

  return 0;
}

