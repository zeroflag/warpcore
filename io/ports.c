#include <SDL2/SDL.h>
#include "ports.h"

int16_t scancode;
Uint64 last_tick = 0;

int16_t io_port_read(int16_t port) {
  switch (port) {
    case 100: {
      Uint64 ticks = SDL_GetTicks64();
      return (int16_t)(ticks & 0xFFFF);
    }
    case 200: {
      const Uint8 *keystate = SDL_GetKeyboardState(NULL);
      Uint8 pressed = keystate[scancode];
      return pressed ? -1 : 0;
    }
    case 300: {
      Uint64 ticks = SDL_GetTicks64();
      Uint64 result = ticks - last_tick;
      last_tick = ticks; 
      return (int16_t)(result & 0xFFFF);
    }
    default:
      // TOOD
  }
  return 0;
}

void io_port_write(int16_t port, int16_t data) {
  switch (port) {
    case 200:
      scancode = data;
    default:
      // TODO
  }
}
