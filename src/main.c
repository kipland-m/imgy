#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

struct jpeg_decompress_struct decomp;
struct jpeg_error_mgr jpeg_err;
FILE* fptr;

void read_jpeg_file(struct jpeg_decompress_struct decomp, char *filepath) {
  /* initializing error handler */
  decomp.err = jpeg_std_error(&jpeg_err);
  jpeg_create_decompress(&decomp);

  if (filepath == NULL) {
    printf("Missing filepath?\n");
  }

  fptr = fopen(filepath, "rb");
  if (fptr == NULL) {
    printf("File was not found\n");
    exit(0);
  } else {
    printf("File loaded successfully\n");
  }
  
  /* providing file pointer to decompression struct */
  jpeg_stdio_src(&decomp, fptr);

  (void) jpeg_read_header(&decomp, TRUE);

  printf("%d\n", decomp.image_height);
  printf("%d\n", decomp.image_width);
  printf("%d\n", decomp.num_components); 
}

int main(int argc, char *argv[]) {
  read_jpeg_file(decomp, argv[1]);

  return 0;
}
