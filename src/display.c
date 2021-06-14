#include <math.h>
#include "display.h"

int window_width = 800;
int window_height = 600;
SDL_Window *window = 0;
SDL_Renderer *renderer = 0;
SDL_Texture *color_buffer_texure = 0;
uint32_t *color_buffer = 0;

bool initialize_window(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    
    // Use SDL to query the monitor resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;

    window = SDL_CreateWindow(0, 
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              window_width, window_height,
                              SDL_WINDOW_BORDERLESS);
    if(!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear_color_buffer(uint32_t color)
{
    for(int y = 0; y < window_height; ++y)
    {
        for(int x = 0; x < window_width; ++x)
        {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void draw_grid(void)
{
    uint32_t color = 0xFF333333;
    for(int y = 0; y < window_height; y+=10)
    {
        for(int x = 0; x < window_width; x+=10)
        {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color)
{
    if((x >= 0 && x < window_width) && (y >= 0 && y < window_height))
    {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);
    
    int abs_delta_x = abs(delta_x);
    int abs_delta_y = abs(delta_y);

    int side_lenght = abs_delta_x >= abs_delta_y ? abs_delta_x : abs_delta_y;
    float x_inc = (float)delta_x / (float)side_lenght;
    float y_inc = (float)delta_y / (float)side_lenght;
    
    float current_x = (float)x0;
    float current_y = (float)y0;
    
    for(int i = 0; i <= side_lenght; ++i)
    {
        draw_pixel(current_x, current_y, color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color)
{
    for(int j = 0; j < height; ++j)
    {
        for(int i = 0; i < width; ++i)
        {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void render_color_buffer(void)
{
    int color_buffer_pitch = window_width*sizeof(uint32_t);
    SDL_UpdateTexture(color_buffer_texure, 
                      0, 
                      color_buffer, 
                      color_buffer_pitch);
    SDL_RenderCopy(renderer, color_buffer_texure,0, 0);
}

