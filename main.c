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
  0x0044  16 * CELL     Return stack initial content (16 cells)
  0x0064  256 bytes     Scratch buffer (TIB, PAD, temporary space)
  0x0164  ...           Heap / Dictionary (grows upward)
*/

int verbose = 0;
int mapping_enabled = 0;
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

uint8_t *map_file(const char* path) {
  int fd = open(path, O_RDWR);
  if (fd != -1) {
    long size = fd_size(fd);
    if (size != MEM_SIZE) {
      close(fd);
      breach("Image size must be: %d got: %d\n", MEM_SIZE, size);
    }
    uint8_t *data = (uint8_t*) fd_map(fd, size);
    if (data == MAP_FAILED) {
      close(fd);
      breach("Cannot mmap image: %s\n", path);
    }
    close(fd);
    return data;
  } else {
    breach("Could not open image: %s\n", path);
    return NULL;
  }
}

void load_file(const char *path, uint8_t* buffer) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
      breach("Cannot open image: %s\n", path);
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
      fclose(fp);
      breach("Cannot get size of image: %s\n", path);
    }
    if (ftell(fp) != MEM_SIZE) {
      fclose(fp);
      breach("Image size must be: %d\n", MEM_SIZE);
    }
    rewind(fp);
    size_t read = fread(buffer, 1, MEM_SIZE, fp);
    fclose(fp);
    if (read != (size_t)MEM_SIZE) {
      fprintf(stderr, "Short read: expected %d, got %zu\n", MEM_SIZE, read);
      free(buffer);
    }
}

void parse_args(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      verbose = 1;
    } else if (strcmp(argv[i], "-m") == 0) {
      mapping_enabled = 1;
    } else {
      image_path = argv[i];
    }
  }
}
typedef struct {
    int major;
    int minor;
} Ver;

Ver read_version(uint8_t *image) {
  Ver ver;
  ver.major = image[0] >> 4;
  ver.minor = image[0] & 15;
  return ver;
}

int main(int argc, char **argv) {
  parse_args(argc, argv);
  dprint("Loading %s..\n", image_path);

  uint8_t mem[MEM_SIZE];
  uint8_t *mapped_mem = NULL;
  if (mapping_enabled) {
    dprint("MMAP image: %s\n", image_path);
    mapped_mem = map_file(image_path);
  } else {
    load_file(image_path, mem);
  }

  Ver ver = read_version(mapping_enabled ? mapped_mem : mem);
  
  if (ver.major == 1) {
    dprint("Image version: %d.%d.\n", ver.major, ver.minor);
    cell_t result = engage(mapping_enabled ? mapped_mem : mem, 0x01, 0x04, 0x44, 0x164);
    msync(mem, MEM_SIZE, MS_SYNC);
    return result;
  } else {
    breach("Unsupported image version: %d.%d\n", ver.major, ver.minor);
  }
}
