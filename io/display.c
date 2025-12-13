#include "display.h"

/*
** VRAM: 32x32 x 8BIT (TILE-INDEX) = 1K
** TILESET:
**   One tile: 8x8 16 colors: 32b total
**   256 tiles total = 8K
** SPRITES
**   256 sprites. 8K total
** 8+8 + 1 = 17K
**
**      -$2000 CODE
** $2000-$4000 SPRITES
** $4000-$6000 TILES
** $6000-$7000 SCREEN
** 
*/

SDL_Renderer *renderer;
SDL_Window   *window;

const char TITLE[] = "WarpCore";
const int FPS = 60;

const int TILE_WIDTH  = 8;
const int TILE_HEIGHT = 8;
const int TILE_SIZE   = TILE_WIDTH * TILE_HEIGHT;

const int N_TILES_X = 32;
const int N_TILES_Y = 32;

const int WIDTH  = N_TILES_X * TILE_WIDTH;
const int HEIGHT = N_TILES_Y * TILE_HEIGHT;

const int VRAM = 0x6000;
const int TILESET = 0x4000;

const int SCALE = 3;

Uint32 last_rendered;
Uint32 threshold = 1000 / FPS;

SDL_Texture* framebuffer;

uint32_t palette[16] = {
  // grayscale
  0xFF000000,  // black
  0xFF555555,  // dark gray
  0xFFAAAAAA,  // light gray
  0xFFFFFFFF,  // white
  // primary colors
  0xFFFF0000,  // red
  0xFF00FF00,  // green
  0xFF0000FF,  // blue
  0xFFFFFF00,  // yellow
  // secondary / mixed colors
  0xFFFF00FF,  // magenta
  0xFF00FFFF,  // cyan
  0xFF800000,  // dark red
  0xFF008000,  // dark green
  0xFF000080,  // dark blue
  0xFF808000,  // olive
  0xFF800080,  // purple
  0xFF008080   // teal
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
      // Each row has TILE_WIDTH pixels; two pixels per byte
      int byte_index = row * (TILE_WIDTH / 2) + col / 2;
      uint8_t packed = tile[byte_index];

      uint8_t index;
      if (col % 2 == 0) {
          // high nibble
          index = (packed >> 4) & 0xF;
      } else {
          // low nibble
          index = packed & 0xF;
      }
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
