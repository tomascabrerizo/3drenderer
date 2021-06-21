#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

typedef enum
{
    RENDER_DOTS         = (1 << 0),
    RENDER_WIREFRAME    = (1 << 1),
    RENDER_TRIANGLE     = (1 << 2),
    RENDER_FACE_CULLING = (1 << 3),
    RENDER_TEXTURE      = (1 << 4),
} render_mode_t;
extern render_mode_t render_mode;

extern int window_width;
extern int window_height;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texure;
extern uint32_t *color_buffer;

bool initialize_window(void);
void destroy_window(void);

void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

#endif //DISPLAY_H
