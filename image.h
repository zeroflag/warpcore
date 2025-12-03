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
  0x0001  BYTE          OP_LJMP
  0x0002  WORD          Absolute address to jump
  0x0004  32 * CELL     Data stack initial content (32 cells)
  0x0044  16 * CELL     Return stack initial content (16 cells)
  0x0064  256 bytes     Scratch buffer (TIB, PAD, temporary space)
  0x0164  ...           Heap / Dictionary (grows upward)
*/

typedef struct {
    int major;
    int minor;
} Ver;

Ver read_version(uint8_t *image);

uint8_t *map_file(const char* path);
void load_file(const char *path, uint8_t* buffer);
void sync_mapped_image(uint8_t* mem);
int dump(const uint8_t* mem);

#endif // IMAGE_H
