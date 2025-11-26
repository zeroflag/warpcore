#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "vm.h"

int fd_size(int fd) {
  struct stat fst;
  return fstat(fd, &fst) == -1 ? -1 : fst.st_size;
}

char *fd_map(int fd, int size) {
  return mmap(NULL, size,
              PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

char *load(char* fname, long *out_size) {
  int fd = open(fname, O_RDWR);
  if (fd != -1) {
    *out_size = fd_size(fd);
    if (*out_size == -1) {
      close(fd);
      breach("Cannot get fstat on image: %s\n", fname);
    }

    char *data = fd_map(fd, *out_size);
    if (data == MAP_FAILED) {
      close(fd);
      breach("Cannot mmap image: %s\n", fname);
    }

    close(fd);
    return data;
  } else {
    breach("Could not open image: %s\n", fname);
    return NULL;
  }
}

int main(int argc, char **argv) {
  long size;
  char *imgfile = argc == 2 ? argv[1] : "image.dat";
  printf("Loading %s..\n", imgfile);
  char *heap = load(imgfile, &size);
  printf("Loaded %ld byes\n", size);
  return engage(heap, size, 0xE0, 0, 0x1C);
}
