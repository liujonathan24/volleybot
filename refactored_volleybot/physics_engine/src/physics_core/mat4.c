#include "physics_core/mat4.h"
#include <string.h> // For memset
#include <math.h>

void mat4_zero(Mat4* result) {
    memset(result->m, 0, sizeof(result->m));
}

void mat4_add(const Mat4* a, const Mat4* b, Mat4* result) {
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            result->m[c][r] = a->m[c][r] + b->m[c][r];
        }
    }
}

void mat4_identity(Mat4* result) {
    memset(result->m, 0, sizeof(result->m));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void mat4_multiply(const Mat4* a, const Mat4* b, Mat4* result) {
    Mat4 temp; // Use a temporary matrix to allow for aliasing (e.g., result = a * result)

    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            temp.m[c][r] = a->m[0][r] * b->m[c][0] + 
                           a->m[1][r] * b->m[c][1] + 
                           a->m[2][r] * b->m[c][2] + 
                           a->m[3][r] * b->m[c][3];
        }
    }

    memcpy(result->m, temp.m, sizeof(temp.m));
}

Vec3 mat4_transform_point(const Mat4* m, Vec3 v) {
    Vec3 result;
    float w = m->m[0][3] * v.x + m->m[1][3] * v.y + m->m[2][3] * v.z + m->m[3][3];
    if (w == 0.0f) w = 1.0f; // Avoid division by zero

    result.x = (m->m[0][0] * v.x + m->m[1][0] * v.y + m->m[2][0] * v.z + m->m[3][0]) / w;
    result.y = (m->m[0][1] * v.x + m->m[1][1] * v.y + m->m[2][1] * v.z + m->m[3][1]) / w;
    result.z = (m->m[0][2] * v.x + m->m[1][2] * v.y + m->m[2][2] * v.z + m->m[3][2]) / w;
    return result;
}

Vec3 mat4_transform_direction(const Mat4* m, Vec3 v) {
    Vec3 result;
    result.x = m->m[0][0] * v.x + m->m[1][0] * v.y + m->m[2][0] * v.z;
    result.y = m->m[0][1] * v.x + m->m[1][1] * v.y + m->m[2][1] * v.z;
    result.z = m->m[0][2] * v.x + m->m[1][2] * v.y + m->m[2][2] * v.z;
    return result;
}

void mat4_translate(Vec3 t, Mat4* result) {
    mat4_identity(result);
    result->m[3][0] = t.x;
    result->m[3][1] = t.y;
    result->m[3][2] = t.z;
}

void mat4_scale(Vec3 s, Mat4* result) {
    mat4_identity(result);
    result->m[0][0] = s.x;
    result->m[1][1] = s.y;
    result->m[2][2] = s.z;
}

void mat4_rotate(Vec3 axis, float angle_rad, Mat4* result) {
    mat4_identity(result);

    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    float t = 1.0f - c;

    Vec3 norm_axis = axis;
    vec3_normalize(&norm_axis, &norm_axis);
    float x = norm_axis.x;
    float y = norm_axis.y;
    float z = norm_axis.z;

    result->m[0][0] = c + x * x * t;
    result->m[0][1] = y * x * t + z * s;
    result->m[0][2] = z * x * t - y * s;

    result->m[1][0] = x * y * t - z * s;
    result->m[1][1] = c + y * y * t;
    result->m[1][2] = z * y * t + x * s;

    result->m[2][0] = x * z * t + y * s;
    result->m[2][1] = y * z * t - x * s;
    result->m[2][2] = c + z * z * t;
}

void mat4_look_at(Vec3 eye, Vec3 target, Vec3 up, Mat4* result) {
    Vec3 f, r, u;
    vec3_sub(&target, &eye, &f);
    vec3_normalize(&f, &f);

    vec3_cross(&f, &up, &r);
    vec3_normalize(&r, &r);

    vec3_cross(&r, &f, &u);

    mat4_identity(result);
    result->m[0][0] = r.x;
    result->m[1][0] = r.y;
    result->m[2][0] = r.z;

    result->m[0][1] = u.x;
    result->m[1][1] = u.y;
    result->m[2][1] = u.z;

    result->m[0][2] = -f.x;
    result->m[1][2] = -f.y;
    result->m[2][2] = -f.z;

    result->m[3][0] = -vec3_dot(&r, &eye);
    result->m[3][1] = -vec3_dot(&u, &eye);
    result->m[3][2] = vec3_dot(&f, &eye);
}

void mat4_perspective(float fov_y_rad, float aspect_ratio, float near_plane, float far_plane, Mat4* result) {
    mat4_identity(result);
    float tan_half_fov = tanf(fov_y_rad / 2.0f);
    
    result->m[0][0] = 1.0f / (aspect_ratio * tan_half_fov);
    result->m[1][1] = 1.0f / tan_half_fov;
    result->m[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    result->m[2][3] = -1.0f;
    result->m[3][2] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    result->m[3][3] = 0.0f;
}

void mat4_affine_inverse(const Mat4* m, Mat4* result) {
    // Transpose the rotation part (upper 3x3)
    result->m[0][0] = m->m[0][0]; result->m[0][1] = m->m[1][0]; result->m[0][2] = m->m[2][0];
    result->m[1][0] = m->m[0][1]; result->m[1][1] = m->m[1][1]; result->m[1][2] = m->m[2][1];
    result->m[2][0] = m->m[0][2]; result->m[2][1] = m->m[1][2]; result->m[2][2] = m->m[2][2];

    // Calculate the new translation part
    Vec3 t = {m->m[3][0], m->m[3][1], m->m[3][2]};
    result->m[3][0] = - (t.x * result->m[0][0] + t.y * result->m[1][0] + t.z * result->m[2][0]);
    result->m[3][1] = - (t.x * result->m[0][1] + t.y * result->m[1][1] + t.z * result->m[2][1]);
    result->m[3][2] = - (t.x * result->m[0][2] + t.y * result->m[1][2] + t.z * result->m[2][2]);

    // The rest of the matrix is identity
    result->m[0][3] = 0.0f; result->m[1][3] = 0.0f; result->m[2][3] = 0.0f;
    result->m[3][3] = 1.0f;
}