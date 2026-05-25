#ifndef CLIPPING_H
#define CLIPPING_H

#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include "vertex.h"
#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct {
    vec3_t point;
    vec3_t normal;
} plane_t;


typedef struct {
    vertex_t vertices[MAX_NUM_POLY_VERTICES];
    tex2_t texcoords[MAX_NUM_POLY_VERTICES];
    int num_vertices;
} polygon_t;

polygon_t create_polygon_from_triangle(
    vec4_t v0,
    vec4_t v1,
    vec4_t v2,
    vec3_t n0,
    vec3_t n1,
    vec3_t n2,
    tex2_t t0,
    tex2_t t1,
    tex2_t t2
);


float float_lerp(float a, float b, float c);

void init_frustum_planes(float fovx, float fovy, float z_near, float z_far);
void clip_polygon_against_plane(polygon_t* polygon, int plane);
void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);
void clip_polygon(polygon_t* polygon);
plane_t* get_frustum_planes(void);

#endif
