#include "clipping.h"
#include "texture.h"
#include "vector.h"
#include <math.h>

#define NUM_PLANES 6

plane_t frustum_planes[NUM_PLANES];

plane_t* get_frustum_planes() {
    return frustum_planes;
}

void init_frustum_planes(float fovx, float fovy, float z_near, float z_far) {
    float cos_half_fovx = cos(fovx / 2);
    float sin_half_fovx = sin(fovx / 2);

    float cos_half_fovy = cos(fovy / 2);
    float sin_half_fovy = sin(fovy / 2);


    frustum_planes[LEFT_FRUSTUM_PLANE].point    = vec3_new(0, 0, 0);
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fovx;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point    = vec3_new(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fovx;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[TOP_FRUSTUM_PLANE].point      = vec3_new(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x   = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y   = -cos_half_fovy;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z   = sin_half_fovy;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point    = vec3_new(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fovy;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[NEAR_FRUSTUM_PLANE].point    = vec3_new(0, 0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point    = vec3_new(0, 0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;

}

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
) {
    polygon_t polygon = {
        .vertices = {
            { .position = v0, .normal = n0},
            { .position = v1, .normal = n1},
            { .position = v2, .normal = n2},
        },
        .texcoords = {t0, t1, t2},
        .num_vertices = 3
    };

    return polygon;
}

void triangles_from_polygon(polygon_t *polygon, triangle_t *triangles, int *num_triangles) {
    for (int i = 0; i < polygon->num_vertices - 2; i++) {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].vertices[0].position = polygon->vertices[index0].position;
        triangles[i].vertices[1].position = polygon->vertices[index1].position;
        triangles[i].vertices[2].position = polygon->vertices[index2].position;

        triangles[i].texcoords[0] = polygon->texcoords[index0];
        triangles[i].texcoords[1] = polygon->texcoords[index1];
        triangles[i].texcoords[2] = polygon->texcoords[index2];

        triangles[i].vertices[0].normal = polygon->vertices[index0].normal;
        triangles[i].vertices[1].normal = polygon->vertices[index1].normal;
        triangles[i].vertices[2].normal = polygon->vertices[index2].normal;
    }

    *num_triangles = polygon->num_vertices - 2;

}

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    // The array of inside vertices will be the part of the final
    // polygon return via parameter
    vec4_t inside_vertices[MAX_NUM_POLY_VERTICES];
    tex2_t inside_texcoords[MAX_NUM_POLY_VERTICES];
    vec3_t inside_normals[MAX_NUM_POLY_VERTICES];

    int num_inside_vertices = 0;

    vec4_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1].position;
    tex2_t* previous_texcoord = &polygon->texcoords[polygon->num_vertices - 1];
    vec3_t* previous_normal = &polygon->vertices[polygon->num_vertices - 1].normal;

    // Start the current and previous dot products
    float current_dot = 0;
    float previous_dot = vec4_dot(
        vec4_sub(*previous_vertex, vec4_from_vec3(plane_point)),
        vec4_from_vec3(plane_normal)
    );

    for (int i = 0; i < polygon->num_vertices; i++) {
        // Start current and previous vertex with the first and last polygon vertices
        vec4_t* current_vertex = &polygon->vertices[i].position;
        tex2_t* current_texcoord = &polygon->texcoords[i];
        vec3_t* current_normal = &polygon->vertices[i].normal;

        current_dot = vec4_dot(
            vec4_sub(*current_vertex, vec4_from_vec3(plane_point)),
            vec4_from_vec3(plane_normal)
        );

        // If we changed from inside to outside or vice-versa
        if (current_dot * previous_dot < 0) {
            // Calculate the interpolation factor t
            float t = previous_dot / (previous_dot - current_dot);

            // Calculate the intersection point
            vec4_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z = float_lerp(previous_vertex->z, current_vertex->z, t),
                .w = float_lerp(previous_vertex->w, current_vertex->w, t)
            };

            // Use the lerp formula to get the interpolated U and V texture coordinates
            tex2_t interpolated_texcoord = {
                .u = float_lerp(previous_texcoord->u, current_texcoord->u, t),
                .v = float_lerp(previous_texcoord->v, current_texcoord->v, t)
            };

            // Calculate the intersection point normal
            vec3_t intersection_normal = {
                .x = float_lerp(previous_normal->x, current_normal->x, t),
                .y = float_lerp(previous_normal->y, current_normal->y, t),
                .z = float_lerp(previous_normal->z, current_normal->z, t)
            };

            // Insert the new intersection point in the list of inside vertices
            inside_vertices[num_inside_vertices] = vec4_clone(&intersection_point);
            inside_texcoords[num_inside_vertices] = tex2_clone(&interpolated_texcoord);
            inside_normals[num_inside_vertices] = vec3_clone(&intersection_normal);
            num_inside_vertices++;
        }

        // If the current point is inside the plane
        if (current_dot > 0) {
            // Insert the current vertex in the list of inside vertices
            inside_vertices[num_inside_vertices] = vec4_clone(current_vertex);
            inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
            inside_normals[num_inside_vertices] = vec3_clone(current_normal);
            num_inside_vertices++;
        }

        // Move to the next vertex
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
        previous_normal = current_normal;
        previous_dot = current_dot;
    }




    // Loop while the current vertex is different than the last vertex
    // while (current_vertex != &polygon->vertices[polygon->num_vertices].position) {
    //     current_dot = vec4_dot(vec4_sub(*current_vertex, vec4_from_vec3(plane_point)), vec4_from_vec3(plane_normal));






    //     current_vertex++;
    //     current_texcoord++;
    // }

    // Copy all the vertices from the inside vertices into the destination polygon
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i].position = vec4_clone(&inside_vertices[i]);
        polygon->vertices[i].normal = vec3_clone(&inside_normals[i]);
        polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
    }

    polygon->num_vertices = num_inside_vertices;

}

void clip_polygon(polygon_t* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}
