#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "vm.h"

/*
  Image Format:
  OFFSET  SIZE          DESCRIPTION
  ------  ------------  -----------------------------------------
  0x0000  BYTE          Version:
                        high 4bit = major: 1
                        low  4bit = minor: 0

  0x0001  BYTE          OP_LJMP
  0x0002  WORD          Absolute address to jump

  0x0004  32 * CELL     Data stack initial content (32 cells)
                        Typically 32 * 2 bytes = 64 bytes

  0x0044  16 * CELL     Return stack initial content (16 cells)
                        Typically 16 * 2 bytes = 32 bytes

  0x0064  256 bytes     Scratch buffer (TIB, PAD, temporary space)

  0x0164  ...           Heap / Dictionary (grows upward)
*/

int verbose = 0;
char* image_path = "image.dat";

void dprint(const char *format, ...) {
  if (!verbose) return;
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

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

void parse_args(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
       verbose = 1;
    } else {
       image_path = argv[i];
    }
  }
}

int main(int argc, char **argv) {
  parse_args(argc, argv);
  long size;
  dprint("Loading %s..\n", image_path);
  uint8_t *mem = load(image_path, &size);
  dprint("Loaded %ld byes\n", size);
  if (size != MEM_SIZE) {
    fprintf(stderr, "Image size must be: %d\n", MEM_SIZE);
    exit(1);
  }

  int major = mem[0] >> 4;
  int minor = mem[0] & 15;

  if (major == 1) {
    dprint("Image version: %d.%d\n", major, minor);
    return engage(mem, 0x01, 0x04, 0x44, 0x164);
  } else {
    fprintf(stderr, "Unsupported image version: %d.%d\n", major, minor);
    exit(1);
  }
}
