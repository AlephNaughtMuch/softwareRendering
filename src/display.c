#include "display.h"

// Init SDL Window and Renderer
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

// Init SDL texture
static SDL_Texture* color_buffer_texture = NULL;

// Init a color buffer
static uint32_t* color_buffer = NULL;

// Init a depth buffer
static float* z_buffer = NULL;

// Init window resolution
static int window_size_factor = 2;
static int window_width = 1920;
static int window_height = 1080;

// Init render and cull methods
static int render_method = 0;
static int cull_method = 0;

// Define render method and culling
// render_method_t render_method = RENDER_FILL_TRIANGLE;
// cull_method_t cull_method = CULL_BACKFACE;



// Setters and getters
int get_window_width(void) {
    return window_width;
}

int get_window_height(void) {
    return window_height;
}

void set_render_method(int method) {
    render_method = method;
}

void set_cull_method(int method) {
    cull_method = method;
}

float get_zbuffer_at(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0;
    }
    return z_buffer[(window_width * y) + x];
}

void update_zbuffer_at(int x, int y, float value) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    z_buffer[(window_width * y) + x] = value;
}

// Checks
bool is_cull_backface(void) {
    return cull_method == CULL_BACKFACE;
}

bool should_render_filled_triangles(void) {
    return (
        render_method == RENDER_FILL_TRIANGLE ||
        render_method == RENDER_FILL_TRIANGLE_WIRE
    );
}

bool should_render_textured_triangles(void) {
    return (
        render_method == RENDER_TEXTURED ||
        render_method == RENDER_TEXTURED_WIRE
    );
}

bool should_render_wireframe(void) {
    return (
        render_method == RENDER_WIRE               ||
        render_method == RENDER_WIRE_VERTEX        ||
        render_method == RENDER_FILL_TRIANGLE_WIRE ||
        render_method == RENDER_TEXTURED_WIRE
    );
}

bool should_render_wire_vertex(void) {
    return (
        render_method == RENDER_WIRE_VERTEX
    );
}

// Initialize SDL properties, and a window
bool initialize_window(void) {
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Dynamically get monitor resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width  = display_mode.w / window_size_factor;
    window_height = display_mode.h / window_size_factor;


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

    // Allocate the required memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

    // Allocate the required memory in bytes to hold the depth buffer
    z_buffer = (float*) malloc(sizeof(float) * window_width * window_height);

    // Create a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );


    return true;

}

// Drawing into the color buffer functions
void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    color_buffer[(window_width * y) + x] = color;
}

void draw_grid(void) {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = 0xFF333333;
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

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float) side_length;
    float y_inc = delta_y / (float) side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void render_color_buffer(void) {
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
         (int) (window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


// Cleanup functions
void clear_color_buffer(uint32_t color) {
    for (int i = 0; i < window_width * window_height; i++) {
        color_buffer[i] = color;
    }
}

void clear_z_buffer() {
    for (int i = 0; i < window_width * window_height; i++) {
        z_buffer[i] = 1.0;
    }
}

void destroy_window(void) {
    free(color_buffer);
    free(z_buffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
