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
const int TILESET = 0x5000;

const int SCALE = 3;

Uint32 last_rendered;
Uint32 threshold = 1000 / FPS;

SDL_Texture* framebuffer;

uint32_t palette[32] = {
  // grayscale
  0xFF000000, 0xFF555555, 0xFFAAAAAA, 0xFFFFFFFF,
  // primary
  0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00,
  0xFFFF00FF, 0xFF00FFFF, 0xFF800000, 0xFF008000,
  0xFF000080, 0xFF808000, 0xFF800080, 0xFF008080,
  // bright versions
  0xFFFF8080, 0xFF80FF80, 0xFF8080FF, 0xFFFFFF80,
  0xFFFF80FF, 0xFF80FFFF, 0xFFC0C0C0, 0xFF404040,
  // retro/dark tones
  0xFF804000, 0xFF408000, 0xFF400080, 0xFF008040,
  0xFF804080, 0xFF408080, 0xFF808040, 0xFF804040
};

void sdl_init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      printf("SDL_Init Error: %s\n", SDL_GetError());
      return;
  }

  window = SDL_CreateWindow(
    TITLE,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WIDTH * SCALE,
    HEIGHT * SCALE,
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
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      WIDTH, HEIGHT
  );

  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // nearest-neighbor
}

void draw_tile(uint8_t* tile, int tx, int ty, uint8_t* pixels, int pitch) {
  int px = tx * TILE_WIDTH;
  int py = ty * TILE_HEIGHT;
  for (int row = 0; row < TILE_HEIGHT; row++) {
    uint32_t* dst = (uint32_t*)(pixels + (py + row) * pitch + px * 4);
    for (int col = 0; col < TILE_WIDTH; col++) {
      uint8_t index = tile[row * TILE_WIDTH + col];
      dst[col] = palette[index];
    }
  }
}

inline void clear_screen() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

void render(uint8_t* mem) {
  uint8_t* pixels;
  int pitch;
  clear_screen();
  SDL_LockTexture(framebuffer, NULL, (void**)&pixels, &pitch);

  for (int ty = 0; ty < N_TILES_Y; ty++) {
    for (int tx = 0; tx < N_TILES_X; tx++) {
      uint8_t tile_index = mem[VRAM + (ty * N_TILES_X) + tx];
      uint8_t* tile = mem + TILESET + (tile_index * TILE_SIZE);
      draw_tile(tile, tx, ty, pixels, pitch);
    }
  }
  SDL_UnlockTexture(framebuffer);

  SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
  SDL_RenderPresent(renderer);
  last_rendered = SDL_GetTicks();
}

inline int should_render() {
  return SDL_GetTicks() - last_rendered > threshold;
}

void sdl_tick(uint8_t* mem) {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }
  }
  if (should_render()) {
    render(mem);
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
