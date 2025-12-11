#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>

void sdl_init();
void sdl_tick(uint8_t* mem);
 
#endif // DISPLAY_H
