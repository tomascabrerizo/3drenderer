#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

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
