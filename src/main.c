#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

/* open_file is just separating our file opening logic
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

  /* we are initialzing jpeg_err with jpeg_std_error(),
   * and sets decomp's "err" to our jpeg_err 
   */
  decomp.err = jpeg_std_error(&jpeg_err);
  jpeg_create_decompress(&decomp);

  /* providing file pointer to decompression struct */
  FILE* infptr = open_file(infilepath); 
  jpeg_stdio_src(&decomp, infptr);

  /* we are giving our decompression struct some header data */
  (void) jpeg_read_header(&decomp, TRUE);

  printf("%d\n", decomp.image_height);
  printf("%d\n", decomp.image_width);
  printf("%d\n", decomp.num_components); 

  (void) jpeg_start_decompress(&decomp);

  return 0;
}

int read_jpeg(char *infile, char *outfile) {
  struct jpeg_decompress_struct decomp;

  return do_read_jpeg(decomp, infile, outfile); 
}

int main(int argc, char *argv[]) {
  read_jpeg(argv[1], argv[2]);

  return 0;
}
