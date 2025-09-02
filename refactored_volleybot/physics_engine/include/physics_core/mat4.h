#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

// Represents a 4x4 matrix, stored in column-major order for graphics API compatibility.
typedef struct {
    float m[4][4];
} Mat4;

/* --- Core Matrix Operations --- */
void mat4_identity(Mat4* result);
void mat4_zero(Mat4* result);
void mat4_add(const Mat4* a, const Mat4* b, Mat4* result);
void mat4_multiply(const Mat4* a, const Mat4* b, Mat4* result);
Vec3 mat4_transform_point(const Mat4* m, Vec3 v);
Vec3 mat4_transform_direction(const Mat4* m, Vec3 v);

/* --- Transformation Generators --- */
void mat4_translate(Vec3 t, Mat4* result);
void mat4_scale(Vec3 s, Mat4* result);
void mat4_rotate(Vec3 axis, float angle_rad, Mat4* result);

/* --- Camera and Projection Matrices --- */
void mat4_look_at(Vec3 eye, Vec3 target, Vec3 up, Mat4* result);
void mat4_perspective(float fov_y_rad, float aspect_ratio, float near_plane, float far_plane, Mat4* result);
void mat4_affine_inverse(const Mat4* m, Mat4* result);

#ifdef __cplusplus
}
#endif

#endif // MAT4_H
