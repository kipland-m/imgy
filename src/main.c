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
 * (header data, pixel data) */
int do_read_jpeg(struct jpeg_decompress_struct decomp, char *infilepath, char *outfilepath) {
  struct jpeg_error_mgr jpeg_err;
  decomp.err = jpeg_std_error(&jpeg_err); /* pointing decomp's 'err' to our jpeg_err struct */

  int i;
  int row_stride;
  unsigned char * full_buffer = NULL;
  JSAMPARRAY row_pointers = malloc(decomp.output_height * sizeof(unsigned char *));

  /* providing infile pointer to decompression struct */
  jpeg_stdio_src(&decomp, open_file(infilepath));

  /* we are giving our decompression struct some header data */
  (void) jpeg_read_header(&decomp, TRUE);

  /* DEBUG */
  printf("%d\n", decomp.image_height);
  printf("%d\n", decomp.image_width);
  printf("%d\n", decomp.num_components); 

  (void) jpeg_start_decompress(&decomp);

  row_stride = decomp.output_width * decomp.output_components;

  /* allocating big ass buffer to store whole image
   * in the case of MoonKilledCthun.jpg, it is ~21 million bytes large
   */
  full_buffer = malloc(decomp.output_width * decomp.output_height * decomp.output_components);
  printf("full_buffer size: %d\n", decomp.output_width * decomp.output_height * decomp.output_components);

  for (i = 0; i < decomp.output_height; i++) {
    row_pointers[i] = full_buffer + (i * row_stride);
    printf("%p\n", row_pointers[i]);
  }
 
  /* write into buffer here */
  while (decomp.output_scanline < decomp.output_height) {
    printf("%d\n", decomp.output_scanline);
    (void) jpeg_read_scanlines(&decomp, &row_pointers[decomp.output_scanline], 1);
    printf("%d %d %d\n", full_buffer[0], full_buffer[10080], full_buffer[20160]);
  }
  printf("\n");

  return 0;
}

int read_jpeg(char *infile, char *outfile) {
  /* Creating and initializing decompression struct */
  struct jpeg_decompress_struct decomp;
  jpeg_create_decompress(&decomp);

  return do_read_jpeg(decomp, infile, outfile); 
}

int main(int argc, char *argv[]) {
  read_jpeg(argv[1], argv[2]);

  return 0;
}
