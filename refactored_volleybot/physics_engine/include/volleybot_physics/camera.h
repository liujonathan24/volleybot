#ifndef CAMERA_H
#define CAMERA_H

#include "../physics_core/mat4.h"
#include "primitive.h"

class Camera {
public:
    Camera(int width, int height);

    /**
     * Aims the camera in world space. This will detach it from any parent object.
     */
    void set_look_at(Vec3 eye, Vec3 target, Vec3 up = {0, 1.0f, 0});

    /**
     * Attaches the camera to a parent primitive. The camera will now move with the parent.
     * @param parent A pointer to the primitive to attach to.
     * @param local_offset The camera's position relative to the parent.
     */
    void attach_to(const Primitive* parent, Vec3 local_offset);

    void set_perspective(float fov_y_rad, float near_plane, float far_plane);

    Vec2 world_to_screen(Vec3 world_point);

    const Mat4& get_view_matrix() const;
    const Mat4& get_projection_matrix() const { return projection_matrix; }

private:
    void update_projection_matrix();

    mutable Mat4 view_matrix; // Mutable to allow for lazy updates in a const getter
    Mat4 projection_matrix;

    // For free-floating state
    Vec3 position;
    Vec3 target;
    Vec3 up_direction;

    // For attached state
    const Primitive* parent_object;
    Mat4 local_transform; // Position and orientation relative to the parent

    // Viewport State
    int viewport_width;
    int viewport_height;
    float aspect_ratio;
    float fov_y_rad;
    float near_plane;
    float far_plane;
};

#endif // CAMERA_H
