#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>

#include "vector.h"

typedef struct
{
    vec3_t direction;
} light_t;

extern light_t light;


float light_alignment_factor(vec3_t surface_normal, vec3_t light_direction);
uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor);

#endif