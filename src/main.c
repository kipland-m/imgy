#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include "helpers.h"

/* imgy - Written by Kipland Melton
 *
 * 05/08/25
 */

void save_jpeg(unsigned char *resize_buffer, JSAMPARRAY row_pointers, char *outfilepath,
                                                float resize_width, float resize_height) {
  struct jpeg_compress_struct comp;
  jpeg_create_compress(&comp);

  struct jpeg_error_mgr jpeg_err;
  comp.err = jpeg_std_error(&jpeg_err); 

  comp.image_height = resize_height;
  comp.image_width = resize_width;
  comp.input_components = 3;

  printf("%p\n", row_pointers[0]);

  jpeg_stdio_dest(&comp, write_file(outfilepath));
  comp.in_color_space = JCS_RGB; /* ADDING THIS FIXED THE GREEN GOOP */

  jpeg_set_defaults(&comp);
  jpeg_start_compress(&comp, TRUE);

  int i = 0; 
  while (comp.next_scanline < comp.image_height) {
      /* jpeg_write_scanlines expects an array of pointers to scanlines.
       * Here the array is only one element long, but you could pass
       * more than one scanline at a time if that's more convenient.
       */
    jpeg_write_scanlines(&comp, &row_pointers[i], 1);
    i++;
    }

  jpeg_finish_compress(&comp);

  /*
   * DEBUG
   */
  printf("%d\n", comp.image_height);
  printf("%d\n", comp.image_width);

}


void resize_jpeg(struct jpeg_decompress_struct decomp, unsigned char *input_buffer, char *outfilepath) {
  /*
   * Using Nearest Neighbor algorithm for resizing.
   */
  int source_x;
  int source_y;
  int offset;
  int resize_offset;
  float OUTPUT_WIDTH = 400.00; /* WIDTH AND HEIGHT HARDCODED FOR DEVELOPMENT PURPOSES */
  float OUTPUT_HEIGHT = 400.00;

  unsigned char *resize_buffer = NULL;
  int row_stride = OUTPUT_WIDTH * decomp.output_components;
  JSAMPARRAY row_pointers = malloc(OUTPUT_HEIGHT * sizeof(unsigned char *)); 

  resize_buffer = malloc(OUTPUT_WIDTH * OUTPUT_HEIGHT * decomp.output_components);

  /*
   * THOUGHT NOTE
   *
   * IN order to resize.. we want to find an rgb value to fit inside of a new pixel position.
   * SO, we must look at choosing a pixel mathematically to fit in a new home inside of our 
   * resize_buffer.
   */

  int i;
  int j;
  for (i = 0; i < OUTPUT_HEIGHT; i++) {
    row_pointers[i] = resize_buffer + (i * row_stride);

    source_y = round(i / OUTPUT_HEIGHT * decomp.output_height); 

    /* NOTE FOR THE FUTURE: WE DO NOT NEED TO CALCULATE THIS EACH TIME..
     * WE CAN BUILD AND ARRAY AND ITERATE THROUGH IT *ONCE* TO KNOW WHICH X PIXELS MAPS TO WHICH.
     *
     * SOURCE_X IS ROW INDEPENDENT
     */
    for (j = 0; j < OUTPUT_WIDTH; j++) {
      source_x = round(j / OUTPUT_WIDTH * decomp.output_width);

      /*
       * Calculating offset values to map input_buffer[rgb_value_index]
       * to its corresponding resize_buffer[rgb_value_index]
       */
      offset = source_y * decomp.output_width * 3 + source_x * 3;
      resize_offset = i * row_stride + j * 3;

      /* So each 'i' will represent the current row we are operating on.
       * 'j' will repeat with each row iterated ('i'). but, 'j' represents
       * which pixel in our resize_buffer needs to be input_buffer[source_x]
       */

      resize_buffer[resize_offset] = input_buffer[offset];
      resize_buffer[resize_offset + 1] = input_buffer[offset + 1];
      resize_buffer[resize_offset + 2] = input_buffer[offset + 2];

      /*
       * DEBUG
       */
      if (i < 2 && j < 2){
        printf("%d\n", resize_buffer[resize_offset]);
        printf("%d\n", resize_buffer[resize_offset + 1]);
        printf("%d\n", resize_buffer[resize_offset + 2]);
      }

    }
  }

  save_jpeg(resize_buffer, row_pointers, outfilepath, OUTPUT_WIDTH, OUTPUT_HEIGHT); 
}


/* do_read_jpeg will handle reading any data related to our image.
 */
unsigned char * do_read_jpeg(struct jpeg_decompress_struct decomp, char *infilepath) {
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


void read_jpeg(char *desired_size, char *infile, char *outfile) {
  struct jpeg_decompress_struct decomp;
  jpeg_create_decompress(&decomp);

  unsigned char *input_buffer;

  /* takes decomp struct, and infile */
  input_buffer = do_read_jpeg(decomp, infile);
  printf("%p\n", input_buffer);

  resize_jpeg(decomp, input_buffer, outfile);
}


int main(int argc, char *argv[]) {
  int width;
  int height;

  parse_size_arg(argv[1], &width, &height);
  printf("width: %d\nheight: %d\n", width, height);

  read_jpeg(argv[1], argv[2], argv[3]);

  return 0;
}
