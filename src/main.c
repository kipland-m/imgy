#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

/* open_file will return infptr (infile pointer) */
FILE* open_file(char *filepath) { 
  if (filepath == NULL) {
    printf("Missing filepath?\n");
    exit(1);
  }

  FILE* infptr = fopen(filepath, "rb");

  if (infptr == NULL) {
    printf("File was not found\n");
    exit(1);
  } else {
    printf("File loaded successfully\n");
    return infptr;
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
    /* scary pointer arithmetic, jumping forward <row length> amount in bytes */
    /* so, I had to revisit this, because my lack of understanding was bothering me.
     * we have to realize that full_buffer is a POINTER to an ARRAY of bytes. 
     * full_buffer[0] will refer to our FIRST byte in our allocated buffer to store
     * a provided image.*/
    row_pointers[i] = full_buffer + (i * row_stride);
    /*
    printf("row %d: %p\n", i, (void *)row_pointers[i]);
    */
  }
 
  /* write into buffer here */
  while (decomp.output_scanline < decomp.output_height) {
    int rows_read = jpeg_read_scanlines(&decomp, &row_pointers[decomp.output_scanline], 1);
  }
  printf("pixel 0: R%d G%d B%d\n", full_buffer[0], full_buffer[1], full_buffer[2]); /* first pixel first row */
  printf("pixel 3360: R%d G%d B%d\n", full_buffer[10078], full_buffer[10079], full_buffer[10080]); /* last pixel first row */

  return 0;
}

void resize_jpeg(unsigned char *input_buffer) {
  /* testing implementation of nearest neighbor with hardcoded
   * output size
   */
  int OUTPUT_WIDTH = 800;
  int OUTPUT_HEIGHT = 600;



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
