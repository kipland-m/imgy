#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include "helpers.h"

void save_jpeg(unsigned char *resize_buffer, char *outfilepath) {
  struct jpeg_compress_struct comp;
  struct jpeg_error_mgr jpeg_err;
  jpeg_create_compress(&comp);

  comp.err = jpeg_std_error(&jpeg_err); 

  /*
   * need to write to file
   */
  jpeg_stdio_dest(&comp, write_file(outfilepath));

}


void resize_jpeg(struct jpeg_decompress_struct decomp, unsigned char *input_buffer) {
  /* For each pixel in the resized image, determine the corresponding pixel in the original image.
   * This involves using the scale factor to calculate the coordinates of the nearest
   * pixel in the original image.
   *
   * Formula: sourceX = int(round( targetX / targetWidth * sourceWidth )) 
   *          sourceY = int(round( targetY / targetHeight * sourceHeight )) 
   * */

  int source_x;
  int source_y;
  int offset;
  int resize_offset;
  float OUTPUT_WIDTH = 800.00; /* WIDTH AND HEIGHT HARDCODED FOR DEVELOPMENT PURPOSES */
  float OUTPUT_HEIGHT = 600.00;

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

      if (i < 2 && j < 2){
        printf("%d\n", resize_buffer[resize_offset]);
        printf("%d\n", resize_buffer[resize_offset + 1]);
        printf("%d\n", resize_buffer[resize_offset + 2]);
      }

    }
  }

}

/* do_read_jpeg will handle reading any data related to our image.
 */
int do_read_jpeg(struct jpeg_decompress_struct decomp, char *infilepath, char *outfilepath) {
  struct jpeg_error_mgr jpeg_err;
  decomp.err = jpeg_std_error(&jpeg_err); /* pointing decomp's 'err' to our jpeg_err struct */

  int i;
  int row_stride;
  unsigned char *full_buffer = NULL;
  JSAMPARRAY row_pointers = malloc(decomp.output_height * sizeof(unsigned char *)); /* will store pointers of all rows of full_buffer */

  /* providing infile pointer to decompression struct */
  jpeg_stdio_src(&decomp, open_file(infilepath));

  (void) jpeg_read_header(&decomp, TRUE);
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

  /* allocating big ass buffer to store whole image RGB data
   */
  full_buffer = malloc(decomp.output_width * decomp.output_height * decomp.output_components);
  printf("full_buffer size: %d\n", decomp.output_width * decomp.output_height * decomp.output_components);

  for (i = 0; i < decomp.output_height; i++) {
    /* full_buffer is a POINTER to a SEQUENCE of bytes. 
     * full_buffer[0] will refer to our FIRST byte in our allocated buffer to store
     * a provided image.
     *
     * so, in this context- know that full_buffer is an ADDRESS. the way pointer math works
     * is that when we add an int to a pointer, it will represent the address 'x' amount of 
     * bytes away from the original address. (where in our case x = (i * row_stride))
     */
    row_pointers[i] = full_buffer + (i * row_stride);
  }
 
  /* keep in mind- since our row_pointers is an array of pointers that
   * are *pointing* at the rows of our full_buffer, we are writing into our full_buffer here.
   * */
  while (decomp.output_scanline < decomp.output_height) {
    (void) jpeg_read_scanlines(&decomp, &row_pointers[decomp.output_scanline], 1);
  }
  printf("pixel 0: R%d G%d B%d\n", full_buffer[0], full_buffer[1], full_buffer[2]); /* first pixel first row */
  printf("pixel 3360: R%d G%d B%d\n", full_buffer[10078], full_buffer[10079], full_buffer[10080]); /* last pixel first row */

  (void) resize_jpeg(decomp, full_buffer);

  return 0;
}


/* need to add desired output size, etc */
int read_jpeg(char *infile, char *outfile) {
  struct jpeg_decompress_struct decomp;
  jpeg_create_decompress(&decomp);

  return do_read_jpeg(decomp, infile, outfile); 
}

int main(int argc, char *argv[]) {
  read_jpeg(argv[1], argv[2]);

  return 0;
}
