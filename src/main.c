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

int save_jpeg(unsigned char *resize_buffer,
                          char *outfilepath,
                          JSAMPARRAY row_pointers,
                          int arg_width, 
                          int arg_height) {

  struct jpeg_compress_struct comp;
  jpeg_create_compress(&comp);

  struct jpeg_error_mgr jpeg_err;
  comp.err = jpeg_std_error(&jpeg_err); 

  comp.image_height = arg_height;
  comp.image_width = arg_width; 
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

  return (0);
}


int resize_jpeg(struct jpeg_decompress_struct *decomp,
                            unsigned char *input_buffer,
                            JSAMPARRAY *row_pointers,
                            unsigned char **resize_buffer,
                            int arg_width, 
                            int arg_height) {

  int i;
  int j;
  int source_y;
  int source_x;
  int offset;
  int resize_offset;
  float source_x_array[arg_width];

  int row_stride = arg_width * decomp->output_components;
  *row_pointers = malloc(arg_height * sizeof(unsigned char *)); 
  *resize_buffer = malloc(arg_width * arg_height * decomp->output_components);

  for (i = 0; i < arg_width; i++) {
    source_x_array[i] = (int)round((float)i / arg_width * decomp->output_width);
  }

  for (i = 0; i < arg_height; i++) {
    (*row_pointers)[i] = *resize_buffer + (i * row_stride);

    source_y = (int)((float)i * decomp->output_height / arg_height);
    if (source_y >= decomp->output_height) source_y = decomp->output_height - 1;

    for (j = 0; j < arg_width; j++) {
      source_x = (int)((float)j * decomp->output_width / arg_width);
      if (source_x >= decomp->output_width) source_x = decomp->output_width - 1;

      offset = source_y * decomp->output_width * 3 + source_x * 3;
      resize_offset = i * row_stride + j * 3;

      (*resize_buffer)[resize_offset] = input_buffer[offset];
      (*resize_buffer)[resize_offset + 1] = input_buffer[offset + 1];
      (*resize_buffer)[resize_offset + 2] = input_buffer[offset + 2];
    }
  }

  return (0);
}


int do_read_jpeg(struct jpeg_decompress_struct *decomp,
                                      char *infilepath, 
                                      unsigned char **full_buffer) {

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
  
  return (0);
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
  unsigned char *resize_buffer = NULL;
  JSAMPARRAY row_pointers = NULL; 
  
  usage_validation(argc);
  parse_size_arg(argv[1], &arg_dimensions.width, &arg_dimensions.height);

  (void) do_read_jpeg(&decomp, argv[2], &full_buffer);
  (void) resize_jpeg(&decomp, full_buffer, &row_pointers, &resize_buffer, arg_dimensions.width, arg_dimensions.height);
  (void) save_jpeg(resize_buffer, argv[3], row_pointers, arg_dimensions.width, arg_dimensions.height);

  printf("pixel 0: R%d G%d B%d\n", full_buffer[0], full_buffer[1], full_buffer[2]); /* first pixel first row */
  printf("pixel 3360: R%d G%d B%d\n", full_buffer[10078], full_buffer[10079], full_buffer[10080]); /* last pixel first row */

  return (0);
}

