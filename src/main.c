#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

/* open_file will return infptr (infile pointer)
 */
FILE* open_file(char *filepath) { 
  if (filepath == NULL) {
    printf("Missing filepath?\n");
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
 * (header data, pixel data)
 */
int do_read_jpeg(struct jpeg_decompress_struct decomp, char *infilepath, char *outfilepath) {
  struct jpeg_error_mgr jpeg_err;
  JSAMPARRAY buffer = NULL; /* defining buffer for 8-bit jpeg */

  /* we are initialzing jpeg_err with jpeg_std_error(),
   * and sets decomp's "err" to point to our jpeg_err struct
   */
  decomp.err = jpeg_std_error(&jpeg_err);

  /* providing infile pointer to decompression struct */
  jpeg_stdio_src(&decomp, open_file(infilepath));

  /* we are giving our decompression struct some header data */
  (void) jpeg_read_header(&decomp, TRUE);

  /* DEBUG
   */
  printf("%d\n", decomp.image_height);
  printf("%d\n", decomp.image_width);
  printf("%d\n", decomp.num_components); 

  (void) jpeg_start_decompress(&decomp);

  int row_stride = decomp.output_width * decomp.output_components;

  return 0;
}

int read_jpeg(char *infile, char *outfile) {
  /* Creating and initializing decompression struct
   */
  struct jpeg_decompress_struct decomp;
  jpeg_create_decompress(&decomp);

  return do_read_jpeg(decomp, infile, outfile); 
}

int main(int argc, char *argv[]) {
  read_jpeg(argv[1], argv[2]);

  return 0;
}
