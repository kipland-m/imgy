#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <libpng.h>


FILE* fptr;

int main(int argc, char *argv[]) {
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
