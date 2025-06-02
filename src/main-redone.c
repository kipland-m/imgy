#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>

struct Dimensions {
    int width;
    int height;
  };

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
  
  usage_validation(argc);
  parse_size_arg(argv[1], &arg_dimensions.width, &arg_dimensions.height);

  // infile = argv[2]
  // outfile = argv[3]

  return 0;
}

