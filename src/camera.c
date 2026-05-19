#include "camera.h"
#include "matrix.h"

static camera_t camera = {
    .position = { 0,0,0 },
    .direction = { 0, 0, 1 },
    .forward_velocity = { 0, 0, 0 },
    .yaw = 0.0f,
    .pitch = 0.0f,
};

void set_camera_position(vec3_t position) {
    camera.position = position;
}

void set_camera_direction(vec3_t direction) {
    camera.direction = direction;
}

void set_camera_forward_velocity(vec3_t forward_velocity) {
    camera.forward_velocity = forward_velocity;
}

vec3_t get_camera_direction(void) {
    return camera.direction;
}

vec3_t get_camera_position(void) {
    return camera.position;
}

vec3_t get_camera_forward_velocity(void) {
    return camera.forward_velocity;
}

vec3_t get_camera_lookat_target(void) {
    // Initialize the target as positive z
    vec3_t target = { 0, 0, 1 };

    // Calculate the rotation for the camera and camera direction
    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);
    mat4_t camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_pitch_rotation);

    mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
    camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_rotation, vec4_from_vec3(target)));

    // Offset the camera position in the direction where the camera is pointing at
    target = vec3_add(camera.position, camera.direction);

    return target;
}

float get_camera_yaw(void) {
    return camera.yaw;
}

float get_camera_pitch(void) {
    return camera.pitch;
}

void init_camera(vec3_t position, vec3_t direction) {
    camera.position = position;
    camera.direction = direction;
}

void update_camera_forward_velocity(vec3_t forward_velocity) {
    camera.forward_velocity = vec3_add(camera.forward_velocity, forward_velocity);
}

void update_camera_position(vec3_t position) {
    camera.position = vec3_add(camera.position, position);
}

void update_camera_yaw(float yaw) {
    camera.yaw += yaw;
}

void update_camera_pitch(float pitch) {
    camera.pitch += pitch;
}
