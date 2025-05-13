#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include "helpers.h"

void save_jpeg(unsigned char *resize_buffer, JSAMPARRAY row_pointers, char *outfilepath,
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


void resize_jpeg(struct jpeg_decompress_struct decomp, unsigned char *input_buffer, char *outfilepath) {
  int source_x;
  int source_y;
  int offset;
  int resize_offset;
  float OUTPUT_WIDTH = 400.00;  
  float OUTPUT_HEIGHT = 400.00;

  unsigned char *resize_buffer = NULL;
  int row_stride = OUTPUT_WIDTH * decomp.output_components;
  JSAMPARRAY row_pointers = malloc(OUTPUT_HEIGHT * sizeof(unsigned char *)); 

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

  save_jpeg(resize_buffer, row_pointers, outfilepath, OUTPUT_WIDTH, OUTPUT_HEIGHT);
}


int do_read_jpeg(struct jpeg_decompress_struct decomp, char *infilepath, char *outfilepath) {
  struct jpeg_error_mgr jpeg_err;
  decomp.err = jpeg_std_error(&jpeg_err); 

  int i;
  int row_stride;
  unsigned char *full_buffer = NULL;
  JSAMPARRAY row_pointers = malloc(decomp.output_height * sizeof(unsigned char *));  

  jpeg_stdio_src(&decomp, open_file(infilepath));

  (void) jpeg_read_header(&decomp, TRUE);
  (void) jpeg_start_decompress(&decomp);

  row_stride = decomp.output_width * decomp.output_components;

  full_buffer = malloc(decomp.output_width * decomp.output_height * decomp.output_components);

  for (i = 0; i < decomp.output_height; i++) {
    row_pointers[i] = full_buffer + (i * row_stride);
  }
 
  while (decomp.output_scanline < decomp.output_height) {
    (void) jpeg_read_scanlines(&decomp, &row_pointers[decomp.output_scanline], 1);
  }

  (void) resize_jpeg(decomp, full_buffer, outfilepath);

  return 0;
}


int read_jpeg(char *infile, char *outfile) {
  struct jpeg_decompress_struct decomp;
  jpeg_create_decompress(&decomp);

  return do_read_jpeg(decomp, infile, outfile); 
}


int main(int argc, char *argv[]) {
  read_jpeg(argv[1], argv[2]);

  return 0;
}

