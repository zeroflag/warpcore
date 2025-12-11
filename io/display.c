#include "display.h"

SDL_Renderer *renderer;
SDL_Window   *window;

const char TITLE[] = "WarpCore";
const int WIDTH  = 512;
const int HEIGHT = 512;

void sdl_init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      printf("SDL_Init Error: %s\n", SDL_GetError());
      return;
  }

  SDL_Window *window = SDL_CreateWindow(
    TITLE,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WIDTH,
    HEIGHT,
    SDL_WINDOW_SHOWN
  );

  if (!window) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return;
  }

  // Create a renderer (optional, for drawing)
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }
}

void sdl_display(uint8_t* mem) {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int x = 0; x < WIDTH / 8; x++) {
    for (int y = 0; y < HEIGHT / 8; y++) {

      uint8_t color = mem[0x3000 + y * (WIDTH / 8) + x];
      uint8_t r = (color >> 5) & 0x07; // 3 bits
      uint8_t g = (color >> 2) & 0x07; // 3 bits
      uint8_t b = color & 0x03;        // 2 bits

      r = r * 255 / 7;
      g = g * 255 / 7;
      b = b * 255 / 3;

      SDL_Rect outline = {x * 8, y * 8, 8, 8};
      SDL_SetRenderDrawColor(renderer, 0, r, g, b);
      SDL_RenderFillRect(renderer, &outline);
    }
  }
   
  SDL_RenderPresent(renderer);
}

void sdl_destroy() {
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}
