#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "image.h"
#include "display.h"

const char VER[] = "0.2";

int verbose = 0;
int mapping_enabled = 0;
int gui_enabled = 0;
char* image_path = "image.dat";
char* vm_params = "";

VMHooks hooks = { NULL };

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
    } else if (strcmp(argv[i], "-g") == 0) {
      gui_enabled = 1;
    } else if (strncmp(argv[i], "-P", 2) == 0) {
      vm_params = argv[i] + 2;
      dprint("VM param: %s\n", vm_params);
    } else if (strncmp(argv[i], "-g", 2) == 0) {
    } else {
      image_path = argv[i];
    }
  }
}

void write_vm_params(uint8_t *mem) {
  unsigned len  = strlen(vm_params);
  if (len > 0) {
    unsigned size = len > 128 ? 128 : len;
    memcpy(&mem[VMPARAMS], vm_params, size);
    mem[VMPARAMS + size] = 0;
  }
}

int main(int argc, char **argv) {
  uint8_t *mem = NULL;
  uint8_t static_mem[MEM_SIZE];

  parse_args(argc, argv);

  dprint("Warpcore ver: %s\n", VER);
  if (gui_enabled) {
    dprint("Initializing SDL..");
    sdl_init();
    hooks.display = sdl_display;
  }
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
    write_vm_params(mem);
    cell_t result = engage(mem, MAIN, STACK, RSTACK, hooks);
    if (mapping_enabled) {
      sync_mapped_image(mem);
    }
    return result;
  } else {
    breach("Unsupported image version: %d.%d\n", ver.major, ver.minor);
  }
}
