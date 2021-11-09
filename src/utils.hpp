#ifndef UTILS_H // include guard
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int getFileSize(const char* fileName);
int seq(int start, int end, char** argv);

#endif