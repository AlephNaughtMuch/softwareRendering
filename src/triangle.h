#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "material.h"
#include "vector.h"
#include "texture.h"
#include "upng.h"
#include "vertex.h"

typedef struct
{
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    vec3_t a_normal;
    vec3_t b_normal;
    vec3_t c_normal;
    uint32_t color;
} face_t;

typedef struct
{
    vertex_t vertices[3];
    vec4_t cam_vertices[3];
    tex2_t texcoords[3];
    uint32_t color;
    upng_t* texture;
    material_t material;
} triangle_t;

vec3_t get_triangle_normal(vec4_t vertices[3]);

void draw_filled_triangle (
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
);

void draw_filled_triangle_phong(triangle_t* t);
void draw_filled_triangle_dither(triangle_t* t);

void draw_textured_triangle (
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    upng_t* texture
);

void draw_triangle_pixel(
    int x,
    int y,
    vec4_t point_a,
    vec4_t point_b,
    vec4_t point_c,
    uint32_t color
);

void draw_triangle_phong_pixel(int x, int y, triangle_t* t, material_t material);
void draw_triangle_dither_pixel(int x, int y, triangle_t* t);

void draw_texel(
    int x,
    int y,
    upng_t* texture,
    vec4_t point_a,
    vec4_t point_b,
    vec4_t point_c,
    tex2_t a_uv,
    tex2_t b_uv,
    tex2_t c_uv
);

vec3_t barycenteric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

#endif
