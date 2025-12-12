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
const int TILESET = 0x4000;

const int SCALE = 3;

Uint32 last_rendered;
Uint32 threshold = 1000 / FPS;

SDL_Texture* framebuffer;

uint32_t palette[32] = {
  0xFF000000, // 0  black
  0xFFFFFFFF, // 1  white
  0xFFFF0000, // 2  red
  0xFF00FF00, // 3  green
  0xFF0000FF, // 4  blue
  0xFFFFFF00, // 5  yellow
  0xFFFF00FF, // 6  magenta
  0xFF00FFFF, // 7  cyan

  0xFF7F0000, // 8  dark red
  0xFF007F00, // 9  dark green
  0xFF00007F, // 10 dark blue
  0xFF7F7F00, // 11 olive
  0xFF7F007F, // 12 purple
  0xFF007F7F, // 13 teal
  0xFF7F7F7F, // 14 gray
  0xFF3F3F3F, // 15 dark gray

  0xFFFF7F7F, // 16 light red
  0xFF7FFF7F, // 17 light green
  0xFF7F7FFF, // 18 light blue
  0xFFFFFF7F, // 19 light yellow
  0xFFFF7FFF, // 20 light magenta
  0xFF7FFFFF, // 21 light cyan
  0xFFBFBFBF, // 22 light gray

  0xFF3F0000, // 23 very dark red
  0xFF003F00, // 24 very dark green
  0xFF00003F, // 25 very dark blue
  0xFF3F3F00, // 26 dark olive
  0xFF3F003F, // 27 dark purple
  0xFF003F3F, // 28 dark teal
  0xFF1F1F1F  // 29 near-black
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
