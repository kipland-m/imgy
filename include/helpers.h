#ifndef HELPERS_H
#define HELPERS_H
#include <stdio.h>

FILE* open_file(char *filepath);
FILE* write_file(char *filepath);
int parse_size_arg(char *size_str, int *width, int *height);

#endif
