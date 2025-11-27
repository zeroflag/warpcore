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

uint8_t *load(char* fname, long *out_size) {
  int fd = open(fname, O_RDWR);
  if (fd != -1) {
    *out_size = fd_size(fd);
    if (*out_size == -1) {
      close(fd);
      breach("Cannot get fstat on image: %s\n", fname);
    }

    uint8_t *data = (uint8_t*) fd_map(fd, *out_size);
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
  uint8_t *mem = load(imgfile, &size);
  printf("Loaded %ld byes\n", size);
  if (size != MEM_SIZE) {
    printf("Image size must be: %d\n", MEM_SIZE);
    exit(1);
  }
  return engage(mem, 0xE0, 0, 0x1C);
}
