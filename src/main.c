#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#include "array.h"
#include "clipping.h"
#include "display.h"
#include "material.h"
#include "triangle.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "light.h"
#include "camera.h"

// Define PI in case the compiler does not have a definition for it.
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

// Define max triangles per mesh
#define MAX_TRIANGLES_PER_MESH 500000

///////////////////////////////////////////////////////////////////////////////////////////////
// Declare an array of vectors/points /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
// Global variables for execution status and game loop ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
bool is_running = false;
float delta_time = 0;
int previous_frame_time = 0;

mat4_t world_matrix;
mat4_t view_matrix;
mat4_t proj_matrix;

char* mesh_location = "assets/bunny.obj";
char* texture_location = "assets/cube.png";

///////////////////////////////////////////////////////////////////////////////////////////////
// Define the renderer's setup, input, update, render and garbage clearing ////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void setup (void) {
    // Initialize render mode and culling method
    set_render_method(RENDER_WIRE);
    set_cull_method(CULL_BACKFACE);

    // Init the light
    init_light(vec3_new(1,-1,1));

    // Init the camera
    init_camera(vec3_new(0,0,0), vec3_new(0,0,1));

    // Initialize the perspective projection or NDC matrix
    // Define perpsective parameters
    float aspectx = get_window_width()  / (float) get_window_height();
    float aspecty = get_window_height() / (float) get_window_width();

    float fovy = M_PI / 3.0; // 60 degrees
    float fovx = 2 * atan(tan(fovy / 2) * aspectx);

    float znear = 0.1;
    float zfar  = 100;

    proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

    // Init the frustum planes with a point and a normal
    init_frustum_planes(fovx, fovy, znear, zfar);

   // Load a mesh with an obj file path, and a png file path for the texture
    load_mesh(
        mesh_location,
        texture_location,
        vec3_new(1,1,1),
        vec3_new(0,0,8),
        vec3_new(0,0,0),
        material_new(0.1, 0.7, .8, 32)
    );

    // load_mesh(
    //     mesh_location,
    //     texture_location,
    //     vec3_new(1,1,1),
    //     vec3_new(-2,0,8),
    //     vec3_new(0,0,0),
    //     material_new(0.1, 0.7, .8, 32)
    // );


    // After load_mesh
    mesh_t* bunny = get_mesh(0);
    uint32_t bunny_color = 0xFF939933; // warm brown
    int face_count = array_length(bunny->faces);
    for (int i = 0; i < face_count; i++) {
        bunny->faces[i].color = bunny_color;
    }

}

void process_input(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    break;
                }

                // Render method
                if (event.key.keysym.sym == SDLK_1) {
                    set_render_method(RENDER_WIRE_VERTEX);
                    break;
                }
                if (event.key.keysym.sym == SDLK_2) {
                    set_render_method(RENDER_WIRE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_3) {
                    set_render_method(RENDER_FILL_TRIANGLE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_4) {
                    set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_5)  {
                    set_render_method(RENDER_TEXTURED);
                    break;
                }
                if (event.key.keysym.sym == SDLK_6) {
                    set_render_method(RENDER_TEXTURED_WIRE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_7) {
                    set_render_method(RENDER_FILL_TRIANGLE_PHONG);
                    break;
                }
                if (event.key.keysym.sym == SDLK_8) {
                    set_render_method(RENDER_FILL_TRIANGLE_PHONG_WIRE);
                    break;
                }

                // Cull Method
                if (event.key.keysym.sym == SDLK_c) {
                    set_cull_method(CULL_BACKFACE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_x) {
                    set_cull_method(CULL_NONE);
                    break;
                }

                // FPS Camera movements
                if (event.key.keysym.sym == SDLK_UP) {
                    update_camera_position(vec3_new(
                        0,
                        3.0 * delta_time,
                        0
                    ));
                    break;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    update_camera_position(vec3_new(
                        0,
                        -3.0 * delta_time,
                        0
                    ));
                    break;
                }
                if (event.key.keysym.sym == SDLK_a) {
                    update_camera_yaw(delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_d) {
                    update_camera_yaw(-delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_w) {
                    set_camera_forward_velocity(vec3_mul(
                        get_camera_direction(),
                        5.0 * delta_time
                    ));
                    set_camera_position(vec3_add(
                        get_camera_position(),
                        get_camera_forward_velocity()
                    ));
                    break;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    set_camera_forward_velocity(vec3_mul(
                        get_camera_direction(),
                        5.0 * delta_time
                    ));
                    set_camera_position(vec3_sub(
                        get_camera_position(),
                        get_camera_forward_velocity()
                    ));
                    break;
                }
                if (event.key.keysym.sym == SDLK_j) {
                    update_camera_pitch(delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_k) {
                    update_camera_pitch(-delta_time);
                    break;
                }
                break;

        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model Space <-- Original Mesh Vertices
// |
// -> World Space <-- Multiplied by the world matrix
//    |
//    -> Camera Space <-- Multiplied by the view matrix
//       |
//       -> Clipping  <-- Clip against the six frustum planes
//          |
//          -> Projection <-- Multiply by the projection matrix
//             |
//             -> Image Space <-- Apply perspective divide
//                |
//                -> Screen Space <-- Ready to render
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pipeline helpers ////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
static vec4_t project_vertex(vec4_t v, mat4_t proj_matrix, int width, int height) {
    vec4_t p = mat4_mul_vec4_project(proj_matrix, v);
    p.x *= (width  / 2.0);
    p.y *= (height / 2.0);
    p.y *= -1;
    p.x += (width  / 2.0);
    p.y += (height / 2.0);
    return p;
}

static uint32_t calculate_triangle_color(vec3_t face_normal, uint32_t base_color) {
    if (get_render_method() == RENDER_FILL_TRIANGLE_PHONG ||
        get_render_method() == RENDER_FILL_TRIANGLE_PHONG_WIRE)
    {
        return base_color;
    }
    vec3_t flipped = vec3_new(-face_normal.x, -face_normal.y, -face_normal.z);
    float factor = light_alignment_factor(flipped, get_light_direction());
    return light_apply_intensity(base_color, factor);
}

static void push_triangle(
    vec4_t p0, vec4_t p1, vec4_t p2,
    vec4_t c0, vec4_t c1, vec4_t c2,
    vec3_t n0, vec3_t n1, vec3_t n2,
    tex2_t uv0, tex2_t uv1, tex2_t uv2,
    uint32_t color, upng_t* texture, material_t material
) {
    if (num_triangles_to_render >= MAX_TRIANGLES_PER_MESH) return;
    triangle_t* tr = &triangles_to_render[num_triangles_to_render++];
    tr->vertices[0].position = p0;
    tr->vertices[1].position = p1;
    tr->vertices[2].position = p2;
    tr->cam_vertices[0]      = c0;
    tr->cam_vertices[1]      = c1;
    tr->cam_vertices[2]      = c2;
    tr->vertices[0].normal   = n0;
    tr->vertices[1].normal   = n1;
    tr->vertices[2].normal   = n2;
    tr->texcoords[0]         = uv0;
    tr->texcoords[1]         = uv1;
    tr->texcoords[2]         = uv2;
    tr->color                = color;
    tr->texture              = texture;
    tr->material             = material;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void process_graphics_pipeline(mesh_t* mesh) {
    mesh->rotation.y += 0.3 * delta_time;
    // mesh->rotation.y = M_PI/3.0;

    // Update camera look at target to create view matrix
    vec3_t target = get_camera_lookat_target();
    vec3_t up_direction = { 0, 1, 0 };
    view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

    // Build world and combined world-view matrices
    mat4_t scale_matrix       = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x  = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y  = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z  = mat4_make_rotation_z(mesh->rotation.z);

    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(scale_matrix,       world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x,  world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y,  world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z,  world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    mat4_t world_view_matrix = mat4_mul_mat4(view_matrix, world_matrix);

    int width  = get_window_width();
    int height = get_window_height();

    // -------------------------------------------------------
    // Mesh-level frustum check — runs once per mesh
    // If the mesh origin is inside all frustum planes, skip
    // per-face frustum checks entirely
    // -------------------------------------------------------
    bool mesh_needs_clipping = false;
    plane_t* planes = get_frustum_planes();
    vec4_t mesh_origin = mat4_mul_vec4(world_view_matrix, vec4_new(0, 0, 0, 1));

    for (int p = 0; p < 6; p++) {
        float dot = vec4_dot(
            vec4_sub(mesh_origin, vec4_from_vec3(planes[p].point)),
            vec4_from_vec3(planes[p].normal)
        );
        if (dot < 0) { mesh_needs_clipping = true; break; }
    }

    // Loop all triangle faces
    int face_count = array_length(mesh->faces);
    for (int i = 0; i < face_count; i++) {
        face_t mesh_face = mesh->faces[i];

        // -------------------------------------------------------
        // T0: Transform phase
        // -------------------------------------------------------
        vec4_t transformed_vertices[3];
        transformed_vertices[0] = mat4_mul_vec4(world_view_matrix, mesh->vertices[mesh_face.a].position);
        transformed_vertices[1] = mat4_mul_vec4(world_view_matrix, mesh->vertices[mesh_face.b].position);
        transformed_vertices[2] = mat4_mul_vec4(world_view_matrix, mesh->vertices[mesh_face.c].position);

        // Transform normals (Phong only)
        vec3_t transformed_normals[3] = { {0,0,0}, {0,0,0}, {0,0,0} };
        if (get_render_method() == RENDER_FILL_TRIANGLE_PHONG ||
            get_render_method() == RENDER_FILL_TRIANGLE_PHONG_WIRE)
        {
            transformed_normals[0] = mesh_face.a_normal;
            transformed_normals[1] = mesh_face.b_normal;
            transformed_normals[2] = mesh_face.c_normal;

            for (int j = 0; j < 3; j++) {
                vec4_t n = vec4_new(
                    transformed_normals[j].x,
                    transformed_normals[j].y,
                    transformed_normals[j].z,
                    0.0
                );
                n = mat4_mul_vec4(world_matrix, n);
                transformed_normals[j] = vec3_from_vec4(n);
            }
        }

        // vec3_t face_normal = get_triangle_normal(transformed_vertices);
        vec4_t fn = vec4_new(mesh_face.a_normal.x, mesh_face.a_normal.y, mesh_face.a_normal.z, 0.0);
        fn = mat4_mul_vec4(world_view_matrix, fn);
        vec3_t face_normal = vec3_from_vec4(fn);
        vec3_normalize(&face_normal);


        // -------------------------------------------------------
        // Backface culling
        // -------------------------------------------------------
        if (is_cull_backface()) {
            vec3_t camera_ray = vec3_sub(vec3_new(0,0,0), vec3_from_vec4(transformed_vertices[0]));
            // vec3_normalize(&camera_ray);
            if (vec3_dot(face_normal, camera_ray) < 0) {
                continue;
            }
        }

        // -------------------------------------------------------
        // Per-face frustum check (skipped if mesh is fully inside)
        // -------------------------------------------------------
        bool needs_clipping = false;
        if (mesh_needs_clipping) {
            for (int p = 0; p < 6 && !needs_clipping; p++) {
                for (int j = 0; j < 3; j++) {
                    float dot = vec4_dot(
                        vec4_sub(transformed_vertices[j], vec4_from_vec3(planes[p].point)),
                        vec4_from_vec3(planes[p].normal)
                    );
                    if (dot < 0) { needs_clipping = true; break; }
                }
            }
        }

        uint32_t triangle_color = calculate_triangle_color(face_normal, mesh_face.color);

        // -------------------------------------------------------
        // Clipping path
        // -------------------------------------------------------
        if (needs_clipping) {
            polygon_t polygon = create_polygon_from_triangle(
                transformed_vertices[0], transformed_vertices[1], transformed_vertices[2],
                transformed_normals[0],  transformed_normals[1],  transformed_normals[2],
                mesh_face.a_uv, mesh_face.b_uv, mesh_face.c_uv
            );

            clip_polygon(&polygon);

            triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
            int num_triangles_after_clipping = 0;
            triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

            for (int t = 0; t < num_triangles_after_clipping; t++) {
                triangle_t* tc = &triangles_after_clipping[t];
                push_triangle(
                    project_vertex(tc->vertices[0].position, proj_matrix, width, height),
                    project_vertex(tc->vertices[1].position, proj_matrix, width, height),
                    project_vertex(tc->vertices[2].position, proj_matrix, width, height),
                    tc->vertices[0].position, tc->vertices[1].position, tc->vertices[2].position,
                    tc->vertices[0].normal, tc->vertices[1].normal, tc->vertices[2].normal,
                    tc->texcoords[0], tc->texcoords[1], tc->texcoords[2],
                    triangle_color, mesh->texture, mesh->material
                );
            }
        } else {
            // -------------------------------------------------------
            // Fast path — fully inside frustum, project directly
            // -------------------------------------------------------
            push_triangle(
                project_vertex(transformed_vertices[0], proj_matrix, width, height),
                project_vertex(transformed_vertices[1], proj_matrix, width, height),
                project_vertex(transformed_vertices[2], proj_matrix, width, height),
                transformed_vertices[0], transformed_vertices[1],transformed_vertices[2],
                transformed_normals[0], transformed_normals[1], transformed_normals[2],
                mesh_face.a_uv, mesh_face.b_uv, mesh_face.c_uv,
                triangle_color, mesh->texture, mesh->material
            );
        }

    }

}


void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // Get a delta time factor converted to seconds to be used to update our game obj
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    // Initialize the array of triangles to render
    num_triangles_to_render = 0;
    previous_frame_time = SDL_GetTicks();


    for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++) {
        mesh_t* mesh = get_mesh(mesh_index);
        process_graphics_pipeline(mesh);
    }
}

void render(void) {
    // Clear the arrays to get ready for the next frame
    clear_color_buffer(0xFF000000);
    clear_z_buffer();
    draw_grid();

    // Loop all projected triangles and render them
    for (int i = 0; i < num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (should_render_filled_triangles()) {
            draw_filled_triangle(
                triangle.vertices[0].position.x,
                triangle.vertices[0].position.y,
                triangle.vertices[0].position.z,
                triangle.vertices[0].position.w,

                triangle.vertices[1].position.x,
                triangle.vertices[1].position.y,
                triangle.vertices[1].position.z,
                triangle.vertices[1].position.w,

                triangle.vertices[2].position.x,
                triangle.vertices[2].position.y,
                triangle.vertices[2].position.z,
                triangle.vertices[2].position.w,

                triangle.color
            );
        }

        if (should_render_phong_filled_triangles()) {
            draw_filled_triangle_phong(&triangle);
        }

        if (should_render_textured_triangles()) {
            draw_textured_triangle(
                triangle.vertices[0].position.x,
                triangle.vertices[0].position.y,
                triangle.vertices[0].position.z,
                triangle.vertices[0].position.w,
                triangle.texcoords[0].u,
                triangle.texcoords[0].v,

                triangle.vertices[1].position.x,
                triangle.vertices[1].position.y,
                triangle.vertices[1].position.z,
                triangle.vertices[1].position.w,
                triangle.texcoords[1].u,
                triangle.texcoords[1].v,

                triangle.vertices[2].position.x,
                triangle.vertices[2].position.y,
                triangle.vertices[2].position.z,
                triangle.vertices[2].position.w,
                triangle.texcoords[2].u,
                triangle.texcoords[2].v,

                triangle.texture
            );
        }

        if (should_render_wireframe()) {
            draw_triangle(
                triangle.vertices[0].position.x,
                triangle.vertices[0].position.y,
                triangle.vertices[1].position.x,
                triangle.vertices[1].position.y,
                triangle.vertices[2].position.x,
                triangle.vertices[2].position.y,
                0xFFFFFFFF
            );
        }

        if (should_render_wire_vertex()) {
            draw_rect(triangle.vertices[0].position.x - 3, triangle.vertices[0].position.y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.vertices[1].position.x - 3, triangle.vertices[1].position.y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.vertices[2].position.x - 3, triangle.vertices[2].position.y - 3, 6, 6, 0xFFFF0000);
        }
    }

    render_color_buffer();
}

void free_resources(void) {
    // Clear the array of triangles to render every frame loop
    free_meshes();
    destroy_window();
}

int main(void) {

    is_running = initialize_window();
    setup();

    while (is_running){
        process_input();
        update();
        render();
    }

    free_resources();
    destroy_window();

    return 0;

}
