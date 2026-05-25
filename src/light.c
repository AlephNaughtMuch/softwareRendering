#include "light.h"
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

// Initialize a directional light
static light_t light;

void init_light(vec3_t direction) {
    light.direction = direction;
}

vec3_t get_light_direction(void) {
    return light.direction;
}

float light_alignment_factor(vec3_t surface_normal, vec3_t light_direction) {
    vec3_t normalized_normal = surface_normal;
    vec3_t normalized_light = light_direction;

    vec3_normalize(&normalized_normal);
    vec3_normalize(&normalized_light);

    float alignment = -vec3_dot(
        normalized_normal,
        normalized_light
    );


    return alignment;
}

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {

    // Clamp the percentage factor from 0 to 1
    if (percentage_factor < 0) { percentage_factor = 0; }
    if (percentage_factor > 1) { percentage_factor = 1; }

    uint32_t a =  original_color & 0xFF000000;
    uint32_t r = (original_color & 0x00FF0000) * percentage_factor;
    uint32_t g = (original_color & 0x0000FF00) * percentage_factor;
    uint32_t b = (original_color & 0x000000FF) * percentage_factor;

    uint32_t new_color = a               |
                        (r & 0x00FF0000) |
                        (g & 0x0000FF00) |
                        (b & 0x000000FF);

    return new_color;
}


uint32_t add_colors(uint32_t color1, uint32_t color2) {
    // 1. Extract individual channels using masks and bit shifting
    // Assuming standard ARGB format (A=Alpha, R=Red, G=Green, B=Blue)
    uint32_t a1 = (color1 >> 24) & 0xFF;
    uint32_t r1 = (color1 >> 16) & 0xFF;
    uint32_t g1 = (color1 >> 8) & 0xFF;
    uint32_t b1 = color1 & 0xFF;

    uint32_t a2 = (color2 >> 24) & 0xFF;
    uint32_t r2 = (color2 >> 16) & 0xFF;
    uint32_t g2 = (color2 >> 8) & 0xFF;
    uint32_t b2 = color2 & 0xFF;

    // 2. Add channels and clamp them at 255 (to prevent overflow/wrap-around)
    uint32_t a = MIN(a1 + a2, 255U);
    uint32_t r = MIN(r1 + r2, 255U);
    uint32_t g = MIN(g1 + g2, 255U);
    uint32_t b = MIN(b1 + b2, 255U);

    // 3. Recombine channels into a single uint32_t
    return (a << 24) | (r << 16) | (g << 8) | b;
}


uint32_t multiply_colour(uint32_t color, float scalar) {
    // 1. Unpack components using bit shifts
    uint32_t r = (color >> 16) & 0xFF;
    uint32_t g = (color >> 8)  & 0xFF;
    uint32_t b = color         & 0xFF;

    // 2. Multiply by scalar and cast to integer
    uint32_t newR = MIN(MAX((int) (r * scalar), 0), 255);
    uint32_t newG = MIN(MAX((int) (g * scalar), 0), 255);
    uint32_t newB = MIN(MAX((int) (b * scalar), 0), 255);

    // 3. Repack components (keep original Alpha channel if using ARGB)
    uint32_t a = color & 0xFF000000; // Preserves Alpha
    return a | (newR << 16) | (newG << 8) | newB;
}
