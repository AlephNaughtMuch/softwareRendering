#include "light.h"

// Initialize a directional light
light_t light = { 
    .direction = { .x = 0, .y = 0 , .z = 1}
};


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
