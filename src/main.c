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

  JSAMPARRAY buffer = NULL; /* defining buffer for 8-bit jpeg */
  unsigned char *full_buffer; /* this buffer will hold our entire jpeg */

  /* providing infile pointer to decompression struct */
  jpeg_stdio_src(&decomp, open_file(infilepath));

  /* we are giving our decompression struct some header data */
  (void) jpeg_read_header(&decomp, TRUE);

  /* DEBUG */
  printf("%d\n", decomp.image_height);
  printf("%d\n", decomp.image_width);
  printf("%d\n", decomp.num_components); 

  (void) jpeg_start_decompress(&decomp);

  int row_stride = decomp.output_width * decomp.output_components;
  buffer = (*decomp.mem->alloc_sarray)((j_common_ptr)&decomp, JPOOL_IMAGE, row_stride, 1);
  full_buffer = malloc(decomp.output_width * decomp.output_height * decomp.output_components);
  printf("full_buffer size: %d\n", decomp.output_width * decomp.output_height * decomp.output_components);

  while (decomp.output_scanline < decomp.output_height) {
    (void) jpeg_read_scanlines(&decomp, buffer, 1);
    printf("%d", buffer[0][0]);
    /* fwrite(buffer[0], 1, row_stride, tmpfile()); */
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
