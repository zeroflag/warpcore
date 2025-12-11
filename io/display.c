#include "display.h"

SDL_Renderer *renderer;
SDL_Window   *window;

const char TITLE[] = "WarpCore";
const int FPS = 60;

const int TILE_WIDTH  = 8;
const int TILE_HEIGHT = 8;
const int TILE_SIZE   = TILE_WIDTH * TILE_HEIGHT;

const int N_TILES_X = 40;
const int N_TILES_Y = 30;

const int WIDTH  = N_TILES_X * TILE_WIDTH;
const int HEIGHT = N_TILES_Y * TILE_HEIGHT;

const int VRAM = 0x3000;
const int TRAM = 0x4000;

Uint32 last_rendered;
Uint32 threshold = 1000 / FPS;

SDL_Texture* framebuffer;

void sdl_init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      printf("SDL_Init Error: %s\n", SDL_GetError());
      return;
  }

  window = SDL_CreateWindow(
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

  framebuffer = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_STREAMING,
      WIDTH, HEIGHT
  );
}

void draw_tile(uint8_t* mem, int tile_base, int x, int y) {
  uint8_t* pixels;
  int pitch;

  SDL_LockTexture(framebuffer, NULL, (void**)&pixels, &pitch);

  for (int ty = 0; ty < TILE_HEIGHT; ty++) {
    for (int tx = 0; tx < TILE_WIDTH; tx++) {
      uint8_t color = mem[tile_base + ty * TILE_WIDTH + tx];
      uint32_t* p = (uint32_t*)(pixels + (y + ty) * pitch)
                    + (x + tx);
      *p = color;
    }
  }

  SDL_UnlockTexture(framebuffer);
}

void sdl_render(uint8_t* mem) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  for (int tx = 0; tx < N_TILES_X; tx++) {
    for (int ty = 0; ty < N_TILES_Y; ty++) {
      uint8_t tile_index = mem[VRAM + (ty * N_TILES_X) + tx];
      draw_tile(mem, TRAM + (tile_index * TILE_SIZE), tx, ty);
    }
  }
  SDL_RenderPresent(renderer);
}


void sdl_tick(uint8_t* mem) {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }
  }
  if (SDL_GetTicks() - last_rendered > threshold) {
    sdl_render(mem);
    last_rendered = SDL_GetTicks();
  }
}

void sdl_destroy() {
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}
