#include "display.h"

/*
** VRAM: 32x32 x 8BIT (TILE-INDEX) = 1K
** TILESET:
**   One tile: 8x8 16 colors: 32byte / tile
**   384 tiles total
** SPRITES
**   128 sprites. 8K total
** 8+8 + 1 = 17K
**
**      -$2000 CODE
** $2000-$3000 SPRITES  ( 4K  )
** $3000-$6000 TILES    ( 12K )
** $6000-$6800 SCREEN   ( 2K )
** 
*/

SDL_Renderer *renderer;
SDL_Window   *window;

const char TITLE[] = "WarpCore";
const int FPS = 60;

const int TILE_WIDTH  = 8;
const int TILE_HEIGHT = 8;
const int TILE_SIZE_B = TILE_WIDTH / 2 * TILE_HEIGHT;
const int MAX_TILES = 384;

const int N_TILES_X = 32;
const int N_TILES_Y = 32;

const int WIDTH  = N_TILES_X * TILE_WIDTH;
const int HEIGHT = N_TILES_Y * TILE_HEIGHT;

const int VRAM = 0x6000;
const int TILESET = 0x3000;

const int SCALE = 3;

Uint32 last_rendered;
Uint32 threshold = 1000 / FPS;

SDL_Texture* framebuffer;

uint32_t palette[16] = {
  0xFF000000, // black
  0xFFFFFFFF, // white
  0xFF62748E, // gray
  0xFFFFDF20, // yellow
  0xFF8A0194, // purple
  0xFF9F0712, // dark red
  0xFFFB2C36, // light red
  0xFF3C6301, // dark green
  0xFF7CCF35, // light green
  0xFF34A6F4, // light blue
  0xFF432DD7, // dark blue
  0xFF18786F, // cyan
  0xFFA86A3E, // brown
  0xFFE12AFB, // magenta
  0xFFF5DEB3, // wheat
  0xFF8B4513, // saddle brown
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
    for (int col = 0; col < TILE_WIDTH / 2; col++) {
      uint8_t packed = tile[row * (TILE_WIDTH / 2) + col];
      uint8_t hi = (packed >> 4) & 0xF;
      uint8_t lo = packed & 0xF;
      *dst++ = palette[hi];
      *dst++ = palette[lo];
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
  uint16_t* screen = (uint16_t*) (mem + VRAM);

  for (int ty = 0; ty < N_TILES_Y; ty++) {
    for (int tx = 0; tx < N_TILES_X; tx++) {
      uint16_t tile_index = screen[ty * N_TILES_X + tx];
      if (tile_index >= MAX_TILES)
        tile_index = 0;
      uint8_t* tile = mem + TILESET + (tile_index * TILE_SIZE_B);
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
