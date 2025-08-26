#include "../../include/volleybot_physics/camera.h"
#include "../../include/volleybot_physics/primitive.h"

Camera::Camera(int width, int height) 
    : viewport_width(width), 
      viewport_height(height),
      parent_object(nullptr) {

    aspect_ratio = (float)width / (float)height;

    // Set default perspective and look_at
    set_perspective(1.0472f, 0.1f, 1000.0f); // 60 degrees FOV
    set_look_at({0, 0, 10}, {0, 0, 0});
}

void Camera::set_look_at(Vec3 eye, Vec3 target, Vec3 up) {
    this->parent_object = nullptr; // Detach from any parent
    this->position = eye;
    this->target = target;
    this->up_direction = up;
    mat4_look_at(position, target, up_direction, &view_matrix);
}

void Camera::attach_to(const Primitive* parent, Vec3 local_offset) {
    this->parent_object = parent;
    mat4_translate(local_offset, &this->local_transform);
}

void Camera::set_perspective(float fov_y_rad, float near_plane, float far_plane) {
    this->fov_y_rad = fov_y_rad;
    this->near_plane = near_plane;
    this->far_plane = far_plane;
    update_projection_matrix();
}

void Camera::update_projection_matrix() {
    mat4_perspective(fov_y_rad, aspect_ratio, near_plane, far_plane, &projection_matrix);
}

const Mat4& Camera::get_view_matrix() const {
    if (parent_object) {
        // Get the parent's latest world transform
        const Mat4& parent_transform = parent_object->get_transform();

        // Calculate our final world transform
        Mat4 world_transform;
        mat4_multiply(&parent_transform, &local_transform, &world_transform);

        // The view matrix is the inverse of our final world transform
        // We update the mutable view_matrix member here
        mat4_affine_inverse(&world_transform, &view_matrix);
    }
    // If not attached, view_matrix is already correct from set_look_at()
    return view_matrix;
}

Vec2 Camera::world_to_screen(Vec3 world_point) {
    // 1. Get the latest view matrix (handles attached/unattached cases)
    const Mat4& current_view_matrix = get_view_matrix();

    // 2. Transform from World Space to Clip Space
    Mat4 view_projection;
    mat4_multiply(&projection_matrix, &current_view_matrix, &view_projection);
    Vec3 clip_space_pos = mat4_transform_point(&view_projection, world_point);

    // 3. Perform Viewport Transform to get pixel coordinates
    float screen_x = (clip_space_pos.x + 1.0f) * 0.5f * viewport_width;
    float screen_y = (1.0f - clip_space_pos.y) * 0.5f * viewport_height;

    return vec2_create(screen_x, screen_y);
}
