#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

struct jpeg_decompress_struct decomp;
struct jpeg_error_mgr jpeg_err;
FILE* fptr;

void load_jpeg() {

}

int main(int argc, char *argv[]) {
  // initializing error handler
  decomp.err = jpeg_std_error(&jpeg_err);
  jpeg_create_decompress(&decomp);

  printf("%d\n", argc);

  fptr = fopen("test-images/MoonKilledCthun.jpg", "rb");

  if (fptr == NULL) {
    printf("File was not found\n");
    exit(0);
  } else {
    printf("File loaded successfully\n");
  }

  return 0;
}
