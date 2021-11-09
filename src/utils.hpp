#ifndef UTILS_H // include guard
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PGSIZE sysconf(_SC_PAGE_SIZE)

int getFileSize(const char* fileName);
int getFileSize(int fd);

int seq(int start, int end, char** argv);
int parseNJob(int argc, char* argv[]);

#endif