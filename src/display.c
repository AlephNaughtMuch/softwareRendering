#include "display.h"


// Init SDL Window and Renderer
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Init SDL texture
SDL_Texture* color_buffer_texture = NULL;

// Init a color buffer
uint32_t* color_buffer = NULL;

// Init window resolution
int window_size_factor = 4;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
    
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Dynamically get monitor resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = (int) (display_mode.w / window_size_factor);
    window_height = (int) (display_mode.h / window_size_factor);


    // Create a SDL Window
    window = SDL_CreateWindow("SoftwareRenderer", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }
    
    // Create a SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    
    return true;

}

void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < window_width &&  y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_grid(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            if (y % 10 == 0 || x % 10 == 0) {
                color_buffer[(window_width * y) + x] = 0xFF333333;
            }
        }
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    int x0 = x;
    int y0 = y;
    int x1 = x + width;
    int y1 = y + height;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > window_width)  x1 = window_width;
    if (y1 > window_height) y1 = window_height;

    for (int py = y0; py < y1; py++) {
        int row = py * window_width;
        for (int px = x0; px < x1; px++) {
            // faster than calling draw_pixel
            color_buffer[row + px] = color; 
        }
    }
}

void render_color_buffer(void) {
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
         (int) (window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
