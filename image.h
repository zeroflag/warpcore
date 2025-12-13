#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

/*
  Image Format:
  OFFSET  SIZE          DESCRIPTION
  ------  ------------  -----------------------------------------
  0x0000  BYTE          Version:
                        high 4bit = major: 1
                        low  4bit = minor: 0
  0x0001  BYTE          Reserved: 0xFF
  0x0002  32 * CELL     Data stack initial content (32 cells)
  0x0042  128 bytes     Space for CLI parameters
  0x00C2  32 * CELL     Return stack initial content (32 cells)
  0x0102  128 bytes     Scratch buffer (TIB, PAD, temporary space)
  0x0182  14  bytes     Memory mapped registers (DP)
  0x0190  ...           Initialization (JMP)
  0x0200  ...           User memory (dictionary / code start)
*/

extern const int MAIN;
extern const int STACK;
extern const int RSTACK;
extern const int VMPARAMS;

typedef struct {
    int major;
    int minor;
} Ver;

Ver read_version(uint8_t *image);

uint8_t *map_file(const char* path);
void load_file(const char *path, uint8_t* buffer);
void sync_mapped_image(uint8_t* mem);
void dump_image(const uint8_t* mem, const char* path);

#endif // IMAGE_H
