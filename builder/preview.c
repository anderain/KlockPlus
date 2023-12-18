#include <SDL/SDL.h>
#include <math.h>
#include <windows.h>
#include "preview.h"

#define WHITE   0xffffffff
#define GREY    0xffaaaaaa
#define BLACK   0xff000000
#define swap_int(a, b) { int t = (a); (a) = (b); (b) = t; } NULL

int quit;
SDL_Surface* calc_screen;

#define WINDOW_WIDTH        640
#define WINDOW_HEIGHT       480
#define WINDOW_COLOR_DEPTH  32
#define CALC_SCREEN_WIDTH   128
#define CALC_SCREEN_HEIGHT  64
#define CALC_SCREEN_ZOOM    3

int is_graphics_ready() {
    return calc_screen != NULL;
}

SDL_Surface* create_calc_surface() {
    SDL_Surface *surface;
    unsigned int rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    surface = SDL_CreateRGBSurface(
        SDL_HWSURFACE,
        CALC_SCREEN_WIDTH * CALC_SCREEN_ZOOM,
        CALC_SCREEN_HEIGHT * CALC_SCREEN_ZOOM,
        WINDOW_COLOR_DEPTH,
        rmask, gmask, bmask, amask
    );

    if (surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(1);
    }

    return surface;
}

void set_real_pixel(int x, int y, unsigned int color) {
    unsigned char *row8;
    unsigned short *row16;
    unsigned int *row32;
    int bytes_per_pixel = calc_screen->format->BytesPerPixel;
    int pitch = calc_screen->pitch;

    switch (bytes_per_pixel) {
    case 1:
        row8 = (unsigned char *)((unsigned char *)calc_screen->pixels + y * pitch + x * bytes_per_pixel);
        *row8 = (unsigned char) color;
        break;

    case 2:
        row16 = (unsigned short *)((unsigned char *)calc_screen->pixels + y * pitch + x * bytes_per_pixel);
        *row16 = (unsigned short) color;
        break;

    case 4:
        row32 = (unsigned int *)((unsigned char *)calc_screen->pixels + y * pitch + x * bytes_per_pixel);
        *row32 = (unsigned int) color;
        break;
    }
}

void set_pixel(int x, int y, unsigned int color) {
    int i, j;
    if (x < 0 || x >= CALC_SCREEN_WIDTH || y < 0 || y >= CALC_SCREEN_HEIGHT) return;
#if CALC_SCREEN_ZOOM == 1
    set_real_pixel(x, y);
#else
    for (i = 0; i < CALC_SCREEN_ZOOM; ++i) {
        for (j = 0; j < CALC_SCREEN_ZOOM; ++j) {
            set_real_pixel(x * CALC_SCREEN_ZOOM + i, y * CALC_SCREEN_ZOOM + j, color);
        }
    }
#endif
}

void plot_line (int x0, int y0, int x1, int y1, unsigned int color)
{
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; 
 
  for (;;){ 
    set_pixel (x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } 
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void draw_image (int dx, int dy, int w, int h, int rev, const unsigned char * raw) {
    const pitch = w / 8 + (w % 8 ? 1 : 0);
    int x, y;
    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            const K_BYTE eight_pixel = *(raw + y * pitch + x / 8);
            const int dot = (eight_pixel >> (7 - x % 8)) & 1;
            set_pixel(dx + x, dy + y, dot ^ rev ? BLACK : WHITE);
        }
    }
}

void get_time(int *hh, int *mm, int *ss, int *ms) {
    SYSTEMTIME lt;
    GetLocalTime(&lt);
    *hh = lt.wHour;
    *mm = lt.wMinute;
    *ss = lt.wSecond;
    *ms = lt.wMilliseconds;
}

int start_preview(kb_machine_t * machine, kb_runtime_error_t *error_ret) {
    SDL_Surface* screen;
    SDL_Event event;
    SDL_Rect calc_screen_pos;
    int ret = 1;
    int hh, mm, ss, ms;

    calc_screen_pos.x = (WINDOW_WIDTH - CALC_SCREEN_WIDTH * CALC_SCREEN_ZOOM) / 2;
    calc_screen_pos.y = (WINDOW_HEIGHT - CALC_SCREEN_HEIGHT * CALC_SCREEN_ZOOM) / 2;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Klock Preview", "Preview");

    screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_COLOR_DEPTH, SDL_HWSURFACE);
    calc_screen = create_calc_surface();
    quit = 0;

    // clear screen
    SDL_FillRect(screen, NULL, GREY);
    SDL_FillRect(calc_screen, NULL, WHITE);

    while (!quit) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = 1;
                    break;
                }
            break;
            }
        }
    
        // fetch time and assign to machine
        get_time(&hh, &mm, &ss, &ms);
        machine_var_assign_num(machine, 0, (KB_FLOAT)hh);
        machine_var_assign_num(machine, 1, (KB_FLOAT)mm);
        machine_var_assign_num(machine, 2, (KB_FLOAT)ss);
        machine_var_assign_num(machine, 3, (KB_FLOAT)ms);

        // execute
        ret = machine_exec(machine, error_ret);
        // printf("stack: %d\n", machine->stack->size);
        if (!ret) {
            quit = 0;
            break;
        }

        // redraw
        SDL_BlitSurface(calc_screen, NULL, screen, &calc_screen_pos);

        // update screen
        SDL_Flip(screen);

        SDL_Delay(60);
    }

    SDL_Quit();

    screen = NULL;
    calc_screen = NULL;

    return ret;
}

unsigned char *read_binary_file(const char *filename) {
    FILE *fp;
    int length = 0;
    unsigned char *buf;

    fp = fopen(filename, "rb");

    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);

    buf = (unsigned char *)malloc(length);
    memset(buf, 0, length);
    fseek(fp, 0, SEEK_SET);
    fread(buf, length, 1, fp);

    fclose(fp);
    return buf;
}

int provider_clr() {
    if (!is_graphics_ready()) {
        return 0;
    }
    SDL_FillRect(calc_screen, NULL, WHITE);
    return 0;
}

int provider_set_pixel(int x, int y, int dot) {
    if (!is_graphics_ready()) {
        return 0;
    }
    set_pixel(x, y, dot ? BLACK : WHITE);
    return 0;
}

int provider_plot_line(int x1, int y1, int x2, int y2, int dot) {
    if (!is_graphics_ready()) {
        return 0;
    }
    plot_line(x1, y1, x2, y2, dot ? BLACK : WHITE);
    return 0;
}

int provider_draw_image(int x, int y, int w, int h, int rev, const unsigned char *raw) {
    if (!is_graphics_ready()) {
        return 0;
    }
    draw_image(x, y, w, h, rev, raw);
    return 0;
}

kb_machine_t* load_app(const char *filename) {
    unsigned char *raw;
    kb_machine_t *app;

    raw = read_binary_file(filename);

    if (!raw) {
        return NULL;
    }

    app = machine_create(raw);

    app->graph_clr          = provider_clr;
    app->graph_set_pixel    = provider_set_pixel;
    app->graph_plot_line    = provider_plot_line;
    app->graph_draw_image   = provider_draw_image;

    return app;
}

