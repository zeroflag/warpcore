#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "image.h"

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

int main(int argc, char **argv) {
  uint8_t *mem = NULL;
  uint8_t static_mem[MEM_SIZE];

  parse_args(argc, argv);
  if (mapping_enabled) {
    dprint("MMAP image: %s\n", image_path);
    mem = map_file(image_path);
  } else {
    dprint("Loading %s..\n", image_path);
    load_file(image_path, static_mem);
    mem = static_mem;
  }

  Ver ver = read_version(mem);
  if (ver.major == 1) {
    dprint("Image version: %d.%d.\n", ver.major, ver.minor);
    cell_t result = engage(mem, 0x01, 0x04, 0x44, 0x164);
    if (mapping_enabled) {
      sync_mapped_image(mem);
    }
    return result;
  } else {
    breach("Unsupported image version: %d.%d\n", ver.major, ver.minor);
  }
}
