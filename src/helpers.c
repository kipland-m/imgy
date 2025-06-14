#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>

/*
 * 	i love you and binky and bubble sso much ::))
 */

FILE* open_file(char *filepath) { 
  if (filepath == NULL) {
    exit(1);
  }

  FILE* infptr = fopen(filepath, "rb");
  if (infptr == NULL) {
    printf("File was not found\n");

    exit(1);
  } else {
    return infptr;
  }
}

FILE* write_file(char *filepath) { 
  if (filepath == NULL) {
    printf("write_file - Missing filepath?\n");

    exit(1);
  }

  FILE* infptr = fopen(filepath, "wb");
  if (infptr == NULL) {
    printf("File was not found\n");

    exit(1);
  } else {
    return infptr;
  }
}

int usage_validation(int argc) {
  if (argc < 4 || argc > 4) {
    printf("Usage:\timgy 800x600 </path/to/sourceImage> <path/to/resizedImage\n");
  }

  return 0;
}

int parse_size_arg(char *size_str, int *width, int *height) {
  sscanf(size_str, "%dx%d", width, height);

  return 0;
}
