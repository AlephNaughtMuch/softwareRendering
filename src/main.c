#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "array.h"
#include "clipping.h"
#include "display.h"
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
    init_light(vec3_new(0,0,1));

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
        vec3_new(-3,0,8),
        vec3_new(0,0,0)
    );

    load_mesh(
        mesh_location,
        texture_location,
        vec3_new(1,1,1),
        vec3_new(3,0,8),
        vec3_new(0,0,0)
    );

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

void process_graphics_pipeline(mesh_t* mesh) {
    // Update camera look at target to create view matrix
    vec3_t target = get_camera_lookat_target();
    vec3_t up_direction = { 0, 1, 0 };

    // Create the view matrix
    view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

    // Create a scale, rotation and translation matrices that will be used to multiply mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_make_translation(
                                                    mesh->translation.x,
                                                    mesh->translation.y,
                                                    mesh->translation.z
                                                    );
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    mat4_t world_view_matrix = mat4_mul_mat4(view_matrix, world_matrix);

    // Loop all triangle faces of our cube mesh
    int face_count = array_length(mesh->faces);
    for (int i = 0; i < face_count; i++) {
        face_t mesh_face = mesh->faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a];
        face_vertices[1] = mesh->vertices[mesh_face.b];
        face_vertices[2] = mesh->vertices[mesh_face.c];


        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Multiply the view matrix by the vector to transform the scene
            // to camera space.
            transformed_vertex = mat4_mul_vec4(world_view_matrix, transformed_vertex);

            // Store the transformed vertex
            transformed_vertices[j] = transformed_vertex;
        }

        // Calculate the triangle face normal
        vec3_t face_normal = get_triangle_normal(transformed_vertices);


        ////////////////////////////////////////////////////////
        // Backface Culling ///////////////////////////////////
        ///////////////////////////////////////////////////////
        if (is_cull_backface()) {
            vec3_t camera_ray = vec3_sub(vec3_new(0,0,0), vec3_from_vec4(transformed_vertices[0]));
            vec3_normalize(&camera_ray);

            float dot_normal_camera = vec3_dot(face_normal, camera_ray);

            // If the face is a back-face, continue to the next face
            if (dot_normal_camera < 0) { continue; }
        }


        // Create a polygon from the original transform vertices
        polygon_t polygon = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),

            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );

        // Clip the polygon and returns a new polygon with potential new vertices
        clip_polygon(&polygon);

        //  After clipping, we need to break the polygon into triangles again
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;

        triangles_from_polygon(
            &polygon,
            triangles_after_clipping,
            &num_triangles_after_clipping
        );

        // Loop all the assembled triangles after clipping
        for (int t = 0; t < num_triangles_after_clipping; t++) {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];

            // Projection
            vec4_t projected_points[3];

            for (int j = 0; j < 3; j ++) {
                projected_points[j] = mat4_mul_vec4_project(
                    proj_matrix,
                    triangle_after_clipping.points[j]
                );

                // Scale the points to the viewport
                projected_points[j].x *= (get_window_width() / 2.0);
                projected_points[j].y *= (get_window_height() / 2.0);

                // Invert y values to account for flipped screen y coordinate
                projected_points[j].y *= -1;

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (get_window_width() / 2.0);
                projected_points[j].y += (get_window_height() / 2.0);


            }

            // Calculate color and lighting
            float light_intensity_factor = light_alignment_factor(
                face_normal,
                get_light_direction()
            );
            uint32_t triangle_color = light_apply_intensity(
                mesh_face.color,
                light_intensity_factor
            );

            triangle_t triangle_to_render= {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
                },
                .texcoords = {
                    { triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v },
                    { triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v },
                    { triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v },
                },
                .color = triangle_color,
                .texture = mesh->texture,
            };

            // Save the projected triangles in the array of triangles
            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[num_triangles_to_render] = triangle_to_render;
                num_triangles_to_render++;
            }
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

    // Loop all the meshes of our scene
    // mesh.rotation.x += 0.6 * delta_time;
    // mesh->rotation.y += 0.6 * delta_time;
    // mesh.rotation.z += 0.6 * delta_time;

    // mesh.scale.x += 0.002;

    // mesh.translation.x += 0.01;
    // mesh.translation.z = 5.0;

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
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,

                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,

                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,

                triangle.color
            );
        }

        if (should_render_textured_triangles()) {
            draw_textured_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                triangle.texcoords[0].u,
                triangle.texcoords[0].v,

                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                triangle.texcoords[1].u,
                triangle.texcoords[1].v,

                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.texcoords[2].u,
                triangle.texcoords[2].v,

                triangle.texture
            );
        }

        if (should_render_wireframe()) {
            draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFFFFFF
            );
        }

        if (should_render_wire_vertex()) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
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
