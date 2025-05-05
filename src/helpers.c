#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

FILE* open_file(char *filepath) { 
  if (filepath == NULL) {
    printf("open_file - Missing filepath?\n");
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
    printf("File loaded successfully\n");
    return infptr;
  }
}

