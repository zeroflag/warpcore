#include "display.h"
#include "ports.h"

/*
** Resolution: 
** Resolution: 256x224 pixel
** VRAM: 32x28 x 8BIT (TILE-INDEX) = 896 byte
** TILESET:
**   One tile: 8x8 16 colors: 32byte / tile
**   256 tiles total
** SPRITES:
**   [TileIndex X Y Props] 4 byte per sprite
**   64 Sprites Total = 256B
**
** $0000-$0200 RESERVED  ( 512B  )
** $0200-$4000 USER CODE ( 15.5K )
** $4000-$6000 TILES     ( 8K )
** $6000-$6200 Palette   ( 512B )
** $6200-$6300 Sprites   ( 256B )
** $6300-$6680 SCREEN1   ( 896B )
** $6680-$6A00 SCREEN2   ( 896B )
** $7000-$8000 COMPILER
*/

#define XCHG(a, b) do { \
    __typeof__(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)

SDL_Renderer *renderer;
SDL_Window   *window;

const char TITLE[] = "WarpCore";
const int FPS = 60;

const int TILE_WIDTH  = 8;
const int TILE_HEIGHT = 8;
const int TILE_SIZE_B = TILE_WIDTH * TILE_HEIGHT / 2;

const int N_TILES_X = 32;
const int N_TILES_Y = 28;

const int WIDTH  = N_TILES_X * TILE_WIDTH;
const int HEIGHT = N_TILES_Y * TILE_HEIGHT;

const int VRAM = 0x6300;
const int TILESET = 0x4000;
const int PALETTE = 0x6000;
const int SPRITES = 0x6200;
const int SPRITES_MAX = 64;

const int SCALE = 4;

Uint64 last_rendered;
Uint32 threshold = 1000 / FPS;

SDL_Texture* framebuffer;

inline uint8_t* screen(const uint8_t *mem) {
  return (uint8_t*) (mem + VRAM);
}

inline uint32_t* palette(const uint8_t *mem) {
  return (uint32_t*) (mem + PALETTE);
}

inline uint32_t* sprites(const uint8_t *mem) {
  return (uint32_t*) (mem + SPRITES);
}

inline const uint8_t* tile_at(const uint8_t* mem,
                              uint8_t index)
{
  return mem + TILESET + (index * TILE_SIZE_B);
}

void sdl_init(uint8_t *mem) {
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
  /* SDL_SetTextureBlendMode(framebuffer, SDL_BLENDMODE_BLEND); */

  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // nearest-neighbor

  /* memset(screen(mem), 0, sizeof(uint8_t) * N_TILES_X * N_TILES_Y); */
  memset(sprites(mem), 0, sizeof(uint32_t) * SPRITES_MAX);
}

void draw_tile(const uint8_t* tile,
               const uint32_t* palette,
               int dst_x,
               int dst_y,
               uint8_t* pixels,
               int pitch,
               int transparency,
               int flip,
               uint8_t tile_offset_x,
               uint8_t tile_len)
{
  for (int row = 0; row < TILE_HEIGHT; row++) {
    uint32_t* dst = (uint32_t*)(pixels + (dst_y + row) * pitch + dst_x * 4);

    int start_sub_x = tile_offset_x % 2;
    int col;
    for (col = tile_offset_x / 2; col < tile_len / 2; col++) {
      uint8_t packed = flip
        ? tile[row * (TILE_WIDTH / 2) + (TILE_WIDTH / 2 - col -1)]
        : tile[row * (TILE_WIDTH / 2) + col];

      uint8_t hi = (packed >> 4) & 0xF;
      uint8_t lo = packed & 0xF;

      if (flip) XCHG(hi, lo);
      
      if (transparency) {
        if (hi != 0 && !start_sub_x)
          *dst = palette[hi];
        dst++;
        if (lo != 0)
          *dst = palette[lo];
        dst++;
      } else {
        if (!start_sub_x)
          *dst++ = palette[hi];
        *dst++ = palette[lo];
      }

      start_sub_x = 0;
    }

    // remainder last column
    if (tile_len % 2) {
      uint8_t packed = flip
        ? tile[row * (TILE_WIDTH / 2) + (TILE_WIDTH / 2 - col -1)]
        : tile[row * (TILE_WIDTH / 2) + col];

      uint8_t hi = (packed >> 4) & 0xF;
      uint8_t lo = packed & 0xF;

      if (flip) XCHG(hi, lo);
      
      if (transparency && hi != 0) {
        *dst = palette[hi];
      } else {
        *dst = palette[hi];
      }
    }

  }
}

void draw_sprites(const uint8_t* mem, uint8_t* pixels, int pitch) {
  uint32_t *sprs = sprites(mem);
  for (int i = 0; i < SPRITES_MAX; i++) {
    uint32_t sprite = sprs[i];
    int attr = (sprite >> 24) & 0xFF;
    if (!(attr & 1))
      continue;
    draw_tile(tile_at(mem, sprite & 0xFF),
              palette(mem),
              (sprite >>  8) & 0xFF, // X
              (sprite >> 16) & 0xFF, // Y
              pixels,
              pitch,
              1,
              attr & 2,
              0,
              TILE_WIDTH);
  }
}

void render(const uint8_t* mem) {
  
  uint8_t* pixels;
  int pitch;
  SDL_LockTexture(framebuffer, NULL, (void**)&pixels, &pitch);

  const uint8_t* scr = screen(mem);

  for (int ty = 0; ty < N_TILES_Y; ty++) {

    uint16_t start_tile_x = scroll_x / TILE_WIDTH;
    uint16_t addr = ty * 2 * N_TILES_X + start_tile_x ;
    const uint8_t* tile = tile_at(mem, scr[addr]);

    draw_tile(tile,
              palette(mem),
              0,
              ty * TILE_HEIGHT,
              pixels,
              pitch,
              0,
              0,
              scroll_x % TILE_WIDTH,
              TILE_WIDTH);

    int tx;
    int dst_x;
    for (tx = 1; tx < N_TILES_X; tx++) {
      addr = ty * 2 * N_TILES_X + tx + start_tile_x;
      tile = tile_at(mem, scr[addr]);
      dst_x = tx * TILE_WIDTH - (scroll_x % TILE_WIDTH);
      int dst_y = ty * TILE_HEIGHT;
      draw_tile(tile,
                palette(mem),
                dst_x,
                dst_y,
                pixels,
                pitch,
                0,
                0,
                0,
                TILE_WIDTH);
    }

    if (scroll_x % TILE_WIDTH != 0) {
      dst_x += TILE_WIDTH;
      int len = N_TILES_X * TILE_WIDTH - dst_x;
      
      addr = ty * 2 * N_TILES_X + tx + start_tile_x;
      tile = tile_at(mem, scr[addr]);
      draw_tile(tile,
                palette(mem),
                dst_x,
                ty * TILE_HEIGHT,
                pixels,
                pitch,
                0,
                0,
                0,
                len);
    }

  }

  draw_sprites(mem, pixels, pitch);

  SDL_UnlockTexture(framebuffer);

  SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
  SDL_RenderPresent(renderer);
  last_rendered = SDL_GetTicks64();
}

inline int should_render() {
  return SDL_GetTicks64() - last_rendered > threshold;
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
    last_rendered = SDL_GetTicks64();
  }
}

void sdl_destroy() {
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}
