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
    for (int p_y = y; p_y < window_height; p_y++) {
        for (int p_x = x; p_x < window_width; p_x++) {
            if ((p_x <= x + width) && (p_y <= y + height)) {
                color_buffer[(window_width * p_y) + p_x] = color;
            }
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
