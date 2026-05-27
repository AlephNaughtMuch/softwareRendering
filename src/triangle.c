#include "triangle.h"
#include "display.h"
#include "light.h"
#include "material.h"
#include "swap.h"
#include "texture.h"
#include "upng.h"
#include "utils.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>

#define EPSILON 0.001
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))


vec3_t get_triangle_normal(vec4_t vertices[3]) {
    vec3_t vector_a = vec3_from_vec4(vertices[0]);   /*   A    */
    vec3_t vector_b = vec3_from_vec4(vertices[1]);   /*  / \  */
    vec3_t vector_c = vec3_from_vec4(vertices[2]);   /* C--B  */

    vec3_t vector_ab = vec3_sub(vector_b, vector_a);
    vec3_t vector_ac = vec3_sub(vector_c, vector_a);
    vec3_t normal = vec3_cross(vector_ab, vector_ac);
    vec3_normalize(&vector_ac);
    vec3_normalize(&vector_ab);
    vec3_normalize(&normal);

    return normal;
}

void draw_filled_triangle (triangle_t* t) {
    // Work on a local copy so we can sort without mutating the original
    triangle_t tri = *t;
    uint32_t color = tri.color;

    // Extract y values for sorting
    int y0 = (int)tri.vertices[0].position.y;
    int y1 = (int)tri.vertices[1].position.y;
    int y2 = (int)tri.vertices[2].position.y;

    int x0 = (int)tri.vertices[0].position.x;
    int x1 = (int)tri.vertices[1].position.x;
    int x2 = (int)tri.vertices[2].position.x;

    float z0 = tri.vertices[0].position.z;
    float z1 = tri.vertices[1].position.z;
    float z2 = tri.vertices[2].position.z;

    float w0 = tri.vertices[0].position.z;
    float w1 = tri.vertices[1].position.z;
    float w2 = tri.vertices[2].position.z;

    //////////////////////////////////////////////////////////
    // Sort the vertices (y0 < y1 <y2) ///////////////////////
    //////////////////////////////////////////////////////////
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        float_swap(&z0, &z1); float_swap(&w0, &w1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2); int_swap(&x1, &x2);
        float_swap(&z1, &z2); float_swap(&w1, &w2);
        vertex_swap(&tri.vertices[1], &tri.vertices[2]);
        vec4_swap(&tri.cam_vertices[1], &tri.cam_vertices[2]);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        float_swap(&z0, &z1); float_swap(&w0, &w1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }

    //////////////////////////////////////////////////////////
    // Create vectors a, b, c ////////////////////////////////
    //////////////////////////////////////////////////////////
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    //////////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom) ///
    //////////////////////////////////////////////////////////

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) { inv_slope_1 = (float) (x1 - x0) / abs(y1 - y0); }
    if (y2 - y0 != 0) { inv_slope_2 = (float) (x2 - x0) / abs(y2 - y0); }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {

            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end = x0 + ((y - y0) * inv_slope_2);

            if (x_end < x_start) { int_swap(&x_start, &x_end); }

            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
            }

        }
    }

    //////////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top) /////
    //////////////////////////////////////////////////////////
    inv_slope_1 = 0;

    if (y2 - y1 != 0) { inv_slope_1 = (float) (x2 - x1) / abs(y2 - y1); }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {

            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end = x0 + ((y - y0) * inv_slope_2);

            if (x_end < x_start) { int_swap(&x_start, &x_end); }

            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
            }

        }
    }


}

void draw_filled_triangle_dither(triangle_t* t) {
    // Work on a local copy so we can sort without mutating the original
    triangle_t tri = *t;

    // Extract y values for sorting
    int y0 = (int)tri.vertices[0].position.y;
    int y1 = (int)tri.vertices[1].position.y;
    int y2 = (int)tri.vertices[2].position.y;

    int x0 = (int)tri.vertices[0].position.x;
    int x1 = (int)tri.vertices[1].position.x;
    int x2 = (int)tri.vertices[2].position.x;

    // Sort vertices by Y (y0 < y1 < y2) using bubble sort passes
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2); int_swap(&x1, &x2);
        vertex_swap(&tri.vertices[1], &tri.vertices[2]);
        vec4_swap(&tri.cam_vertices[1], &tri.cam_vertices[2]);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }

    // Slopes
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    // Flat-bottom (upper) half
    if (y1 - y0 != 0) { inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0); }
    if (y2 - y0 != 0) { inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0); }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end   = x0 + ((y - y0) * inv_slope_2);
            if (x_end < x_start) { int_swap(&x_start, &x_end); }
            for (int x = x_start; x < x_end; x++) {
                draw_triangle_dither_pixel(x, y, &tri);
            }
        }
    }

    // Flat-top (lower) half
    inv_slope_1 = 0;
    if (y2 - y1 != 0) { inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1); }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end   = x0 + ((y - y0) * inv_slope_2);
            if (x_end < x_start) { int_swap(&x_start, &x_end); }
            for (int x = x_start; x < x_end; x++) {
                draw_triangle_dither_pixel(x, y, &tri);
            }
        }
    }
}

void draw_filled_triangle_phong(triangle_t* t) {
    // Work on a local copy so we can sort without mutating the original
    triangle_t tri = *t;

    // Extract y values for sorting
    int y0 = (int)tri.vertices[0].position.y;
    int y1 = (int)tri.vertices[1].position.y;
    int y2 = (int)tri.vertices[2].position.y;

    int x0 = (int)tri.vertices[0].position.x;
    int x1 = (int)tri.vertices[1].position.x;
    int x2 = (int)tri.vertices[2].position.x;

    // Sort vertices by Y (y0 < y1 < y2) using bubble sort passes
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2); int_swap(&x1, &x2);
        vertex_swap(&tri.vertices[1], &tri.vertices[2]);
        vec4_swap(&tri.cam_vertices[1], &tri.cam_vertices[2]);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }

    // Slopes
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    // Flat-bottom (upper) half
    if (y1 - y0 != 0) { inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0); }
    if (y2 - y0 != 0) { inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0); }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end   = x0 + ((y - y0) * inv_slope_2);
            if (x_end < x_start) { int_swap(&x_start, &x_end); }
            for (int x = x_start; x < x_end; x++) {
                draw_triangle_phong_pixel(x, y, &tri, t->material);
            }
        }
    }

    // Flat-top (lower) half
    inv_slope_1 = 0;
    if (y2 - y1 != 0) { inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1); }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end   = x0 + ((y - y0) * inv_slope_2);
            if (x_end < x_start) { int_swap(&x_start, &x_end); }
            for (int x = x_start; x < x_end; x++) {
                draw_triangle_phong_pixel(x, y, &tri, t->material);
            }
        }
    }
}

void draw_textured_triangle (triangle_t* t) {
    // Work on a local copy so we can sort without mutating the original
    triangle_t tri = *t;
    upng_t* texture = tri.texture;

    // Extract y values for sorting
    int y0 = (int)tri.vertices[0].position.y;
    int y1 = (int)tri.vertices[1].position.y;
    int y2 = (int)tri.vertices[2].position.y;

    int x0 = (int)tri.vertices[0].position.x;
    int x1 = (int)tri.vertices[1].position.x;
    int x2 = (int)tri.vertices[2].position.x;

    float z0 = tri.vertices[0].position.z;
    float z1 = tri.vertices[1].position.z;
    float z2 = tri.vertices[2].position.z;

    float w0 = tri.vertices[0].position.z;
    float w1 = tri.vertices[1].position.z;
    float w2 = tri.vertices[2].position.z;

    float u0 = tri.texcoords[0].u;
    float u1 = tri.texcoords[1].u;
    float u2 = tri.texcoords[2].u;

    float v0 = tri.texcoords[0].v;
    float v1 = tri.texcoords[1].v;
    float v2 = tri.texcoords[2].v;

    //////////////////////////////////////////////////////////
    // Sort the vertices (y0 < y1 <y2) ///////////////////////
    //////////////////////////////////////////////////////////
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        float_swap(&z0, &z1); float_swap(&w0, &w1);
        float_swap(&u0, &u1); float_swap(&v0, &v1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2); int_swap(&x1, &x2);
        float_swap(&z1, &z2); float_swap(&w1, &w2);
        float_swap(&u1, &u2); float_swap(&v1, &v2);
        vertex_swap(&tri.vertices[1], &tri.vertices[2]);
        vec4_swap(&tri.cam_vertices[1], &tri.cam_vertices[2]);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1); int_swap(&x0, &x1);
        float_swap(&z0, &z1); float_swap(&w0, &w1);
        float_swap(&u0, &u1); float_swap(&v0, &v1);
        vertex_swap(&tri.vertices[0], &tri.vertices[1]);
        vec4_swap(&tri.cam_vertices[0], &tri.cam_vertices[1]);
    }


    // Flip the V component of the texture to account for
    // inverted UV coordinate system
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    //////////////////////////////////////////////////////////
    // Create vectors a, b, c ////////////////////////////////
    //////////////////////////////////////////////////////////
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

    //////////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom) ///
    //////////////////////////////////////////////////////////

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) { inv_slope_1 = (float) (x1 - x0) / abs(y1 - y0); }
    if (y2 - y0 != 0) { inv_slope_2 = (float) (x2 - x0) / abs(y2 - y0); }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {

            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end = x0 + ((y - y0) * inv_slope_2);

            if (x_end < x_start) { int_swap(&x_start, &x_end); }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }

        }
    }

    //////////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top) /////
    //////////////////////////////////////////////////////////
    inv_slope_1 = 0;

    if (y2 - y1 != 0) { inv_slope_1 = (float) (x2 - x1) / abs(y2 - y1); }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {

            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end = x0 + ((y - y0) * inv_slope_2);

            if (x_end < x_start) { int_swap(&x_start, &x_end); }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }

        }
    }

}

/////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights alpha, beta and agamma for a point p //////
/////////////////////////////////////////////////////////////////////////////
vec3_t barycenteric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    // Find the vectors between the vertices ABC and point P
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);
    vec2_t ap = vec2_sub(p, a);

    // Area of the full parallelogram || AC X AB||
    float area_parallelogram_abc = ((ac.x * ab.y) - (ac.y * ab.x));
    // Calculate alpha, beta and gamma (areal coordinates for the point P)
    float alpha = ((pc.x * pb.y) - (pc.y * pb.x)) / area_parallelogram_abc;
    float beta = ((ac.x * ap.y) - (ac.y * ap.x)) / area_parallelogram_abc;
    float gamma =  1.0 - alpha - beta;

    // Return the areal coordinates as weights
    vec3_t weights = {alpha, beta, gamma};
    return weights;

}

/////////////////////////////////////////////////////////////////////////////
// Function to draw  pixel at position x and y //////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void draw_triangle_pixel(
    int x,
    int y,
    vec4_t point_a,
    vec4_t point_b,
    vec4_t point_c,
    uint32_t color
) {
    // Get weights for P
    vec2_t p = {x,y};

    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycenteric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Calculated the interpolated 1/w for the current pixel
    float interpolated_reciprocal_w;

    // Find the reciprocal of the interpolated depth for the current pixel
    interpolated_reciprocal_w = ((1 / point_a.w) * alpha) + ((1 / point_b.w) * beta) + ((1 / point_c.w) * gamma);

    // Only draw the pixel if the depth value is less than the one
    // previously stored in the z-buffer
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)){
        draw_pixel(x, y, color);

        // Update the z buffer value with 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);

    }
}
/////////////////////////////////////////////////////////////////////////////
// Function to draw  Bayer dithering based pixel at position x and y ////////
/////////////////////////////////////////////////////////////////////////////
void draw_triangle_dither_pixel(int x, int y, triangle_t* t) {
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(t->vertices[0].position);
    vec2_t b = vec2_from_vec4(t->vertices[1].position);
    vec2_t c = vec2_from_vec4(t->vertices[2].position);

    vec3_t weights = barycenteric_weights(a, b, c, p);
    float alpha = weights.x;
    float beta  = weights.y;
    float gamma = weights.z;

    // Depth test
    float inv_w =
        ((1.0f / t->vertices[0].position.w) * alpha) +
        ((1.0f / t->vertices[1].position.w) * beta)  +
        ((1.0f / t->vertices[2].position.w) * gamma);
    float depth = 1.0f - inv_w;
    if (depth >= get_zbuffer_at(x, y)) return;

    // Interpolate normal
    vec3_t n = vec3_new(
        (t->vertices[0].normal.x * alpha) + (t->vertices[1].normal.x * beta) + (t->vertices[2].normal.x * gamma),
        (t->vertices[0].normal.y * alpha) + (t->vertices[1].normal.y * beta) + (t->vertices[2].normal.y * gamma),
        (t->vertices[0].normal.z * alpha) + (t->vertices[1].normal.z * beta) + (t->vertices[2].normal.z * gamma)
    );
    vec3_normalize(&n);

    // Light direction
    vec3_t light_dir = get_light_direction();
    light_dir = vec3_new(-light_dir.x, -light_dir.y, -light_dir.z);
    vec3_normalize(&light_dir);
    // Diffuse intensity
    float intensity = MAX(vec3_dot(n, light_dir), 0.0f);
    intensity = smoothstep(0.1f, 0.9f, intensity); // compress midtones


    static const float bayer8x8[8][8] = {
        {  0/64.0f, 32/64.0f,  8/64.0f, 40/64.0f,  2/64.0f, 34/64.0f, 10/64.0f, 42/64.0f },
        { 48/64.0f, 16/64.0f, 56/64.0f, 24/64.0f, 50/64.0f, 18/64.0f, 58/64.0f, 26/64.0f },
        { 12/64.0f, 44/64.0f,  4/64.0f, 36/64.0f, 14/64.0f, 46/64.0f,  6/64.0f, 38/64.0f },
        { 60/64.0f, 28/64.0f, 52/64.0f, 20/64.0f, 62/64.0f, 30/64.0f, 54/64.0f, 22/64.0f },
        {  3/64.0f, 35/64.0f, 11/64.0f, 43/64.0f,  1/64.0f, 33/64.0f,  9/64.0f, 41/64.0f },
        { 51/64.0f, 19/64.0f, 59/64.0f, 27/64.0f, 49/64.0f, 17/64.0f, 57/64.0f, 25/64.0f },
        { 15/64.0f, 47/64.0f,  7/64.0f, 39/64.0f, 13/64.0f, 45/64.0f,  5/64.0f, 37/64.0f },
        { 63/64.0f, 31/64.0f, 55/64.0f, 23/64.0f, 61/64.0f, 29/64.0f, 53/64.0f, 21/64.0f }
    };

    float threshold = bayer8x8[(y) % 8][(x) % 8];
    // Quantize intensity to N tones before dithering
    int num_tones = 2;
    float quantized = floor(intensity * num_tones) / num_tones;

    // Then dither between quantized and next tone up
    float next_tone = quantized + (1.0f / num_tones);
    float dither_intensity = (intensity - quantized) * num_tones;

    float final_intensity = (dither_intensity > threshold) ? next_tone : quantized;

    uint32_t paper_color = 0xFF939944;
    uint32_t ink_color   = 0xFF070707;
    uint32_t final_color = (final_intensity > 0.5f) ? paper_color : ink_color;

    draw_pixel(x, y, final_color);
    update_zbuffer_at(x, y, depth);
}

/////////////////////////////////////////////////////////////////////////////
// Function to draw  phong based pixel at position x and y //////////////////
/////////////////////////////////////////////////////////////////////////////
void draw_triangle_phong_pixel(int x, int y, triangle_t* t, material_t material) {
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(t->vertices[0].position);
    vec2_t b = vec2_from_vec4(t->vertices[1].position);
    vec2_t c = vec2_from_vec4(t->vertices[2].position);

    vec3_t weights = barycenteric_weights(a, b, c, p);
    float alpha = weights.x;
    float beta  = weights.y;
    float gamma = weights.z;

    // Interpolated 1/w for depth test
    float inv_w =
        ((1.0f / t->vertices[0].position.w) * alpha) +
        ((1.0f / t->vertices[1].position.w) * beta)  +
        ((1.0f / t->vertices[2].position.w) * gamma);

    float depth = 1.0f - inv_w;
    if (depth >= get_zbuffer_at(x, y)) return;

    // Interpolate normal
    vec3_t n = vec3_new(
        (t->vertices[0].normal.x * alpha) + (t->vertices[1].normal.x * beta) + (t->vertices[2].normal.x * gamma),
        (t->vertices[0].normal.y * alpha) + (t->vertices[1].normal.y * beta) + (t->vertices[2].normal.y * gamma),
        (t->vertices[0].normal.z * alpha) + (t->vertices[1].normal.z * beta) + (t->vertices[2].normal.z * gamma)
    );
    vec3_normalize(&n);
    // Invert to match camera-space lighting convention
    // n = vec3_new(-n.x, -n.y, -n.z);

    // Interpolate camera-space surface point
    vec3_t surface_point = vec3_new(
        (t->cam_vertices[0].x * alpha) + (t->cam_vertices[1].x * beta) + (t->cam_vertices[2].x * gamma),
        (t->cam_vertices[0].y * alpha) + (t->cam_vertices[1].y * beta) + (t->cam_vertices[2].y * gamma),
        (t->cam_vertices[0].z * alpha) + (t->cam_vertices[1].z * beta) + (t->cam_vertices[2].z * gamma)
    );

    // Light direction (directional — negate stored direction)
    vec3_t light_dir = get_light_direction();
    light_dir = vec3_new(-light_dir.x, -light_dir.y, -light_dir.z);
    vec3_normalize(&light_dir);

    // Camera direction (camera is at origin in camera space)
    vec3_t cam_dir = vec3_sub(vec3_new(0, 0, 0), surface_point);
    vec3_normalize(&cam_dir);

    // Reflection vector: R = 2(N·L)N - L
    float NdotL = vec3_dot(n, light_dir);
    if (NdotL <= 0.0f) {
        draw_pixel(x, y, multiply_colour(t->color, material.ka));
        update_zbuffer_at(x, y, depth);
        return;
    }
    vec3_t reflection = vec3_sub(
        vec3_mul(n, 2.0f * NdotL),
        light_dir
    );
    vec3_normalize(&reflection);

    // Phong components
    float ambient_factor  = material.ka;
    float diffuse_factor  = material.kd * MAX(NdotL, 0.0f);

    float rv_dot = MAX(vec3_dot(reflection, cam_dir), 0.0f);
    float rv_dot_clamped = MAX(rv_dot, 0.0f);
    float pow_result = (float)pow((double)rv_dot_clamped, (double)material.shininess);
    float specular_factor = material.ks * pow_result;

    uint32_t ambient  = multiply_colour(t->color, ambient_factor);
    uint32_t diffuse  = multiply_colour(t->color, diffuse_factor);
    uint32_t specular = multiply_colour(0xFFFFFFFF, specular_factor);

    uint32_t final_color = add_colors(add_colors(ambient, diffuse), specular);

    draw_pixel(x, y, final_color);
    update_zbuffer_at(x, y, depth);
}

/////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at position x and y //////////////////
/////////////////////////////////////////////////////////////////////////////
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
) {

    // Get weights for P
    vec2_t p = {x,y};

    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycenteric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Calculated the interpolated UVs and 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;


    // Find the reciprocal of the interpolated depth for the current pixel
    interpolated_reciprocal_w = ((1 / point_a.w) * alpha) + ((1 / point_b.w) * beta) + ((1 / point_c.w) * gamma);


    // Find the UVs with the depth divide and barycentric weights for the current pixel
    interpolated_u = ((a_uv.u / point_a.w) * alpha) + ((b_uv.u / point_b.w) * beta) + ((c_uv.u / point_c.w) * gamma);
    interpolated_v = ((a_uv.v / point_a.w) * alpha) + ((b_uv.v / point_b.w) * beta) + ((c_uv.v / point_c.w) * gamma);

    // Undo the perspective divide for the interpolated UVs by dividing by the interpolated depth
    if (fabsf(interpolated_reciprocal_w) < 1e-6f) { return; } // skip degenerate pixel
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Get the mesh texture width and height
    int texture_width = upng_get_width(texture);
    int texture_height = upng_get_height(texture);

    // Grab the texture cooridnates according to the texture's resolution
    int tex_x = (int) (interpolated_u * texture_width);
    int tex_y = (int) (interpolated_v * texture_height);

    tex_x = (tex_x % texture_width  + texture_width)  % texture_width;
    tex_y = (tex_y % texture_height + texture_height) % texture_height;

    // Only draw the pixel if the depth value is less than the one
    // previously stored in the z-buffer
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)){
        // Get the buffer of colours from the texture
        uint32_t* texture_buffer = (uint32_t*) upng_get_buffer(texture);
        // Grab the texture and draw a pixel with the texture's information
        // by linearly accessing the texture array
        draw_pixel(x, y, texture_buffer[(texture_width * tex_y) + tex_x]);

        // Update the z buffer value with 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);

    }

}
