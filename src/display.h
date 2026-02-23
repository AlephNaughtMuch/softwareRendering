#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;

// Init a color buffer
extern uint32_t* color_buffer;

// Init window resolution
extern int window_size_factor;
extern int window_width;
extern int window_height;

// Init SDL texture
extern SDL_Texture* color_buffer_texture;

bool initialize_window(void);
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void destroy_window(void);

#endif 