#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include <math.h> 

// Represents a 4x4 matrix, stored in column-major order for graphics API compatibility.
typedef struct {
    float m[4][4];
} Mat4;

/* --- Core Matrix Operations --- */

/**
 * Sets a matrix to the identity matrix.
 * @param result A pointer to the matrix to be set.
 */
void mat4_identity(Mat4* result);

/**
 * Multiplies two matrices (a * b) and stores the result.
 * @param a A pointer to the first matrix.
 * @param b A pointer to the second matrix.
 * @param result A pointer to the matrix where the result will be stored.
 */
void mat4_multiply(const Mat4* a, const Mat4* b, Mat4* result);

/**
 * Transforms a 3D vector (point) by a matrix. Assumes w=1.
 * @param m A pointer to the transformation matrix.
 * @param v The vector (point) to transform.
 * @return The transformed vector.
 */
Vec3 mat4_transform_point(const Mat4* m, Vec3 v);

/**
 * Transforms a 3D vector (direction) by a matrix. Assumes w=0.
 * This ignores the translation part of the matrix.
 * @param m A pointer to the transformation matrix.
 * @param v The vector (direction) to transform.
 * @return The transformed vector.
 */
Vec3 mat4_transform_direction(const Mat4* m, Vec3 v);


/* --- Transformation Generators --- */

/**
 * Creates a translation matrix and stores it in the result.
 * @param t The translation vector.
 * @param result A pointer to the matrix where the result will be stored.
 */
void mat4_translate(Vec3 t, Mat4* result);

/**
 * Creates a scaling matrix and stores it in the result.
 * @param s The scaling vector.
 * @param result A pointer to the matrix where the result will be stored.
 */
void mat4_scale(Vec3 s, Mat4* result);

/**
 * Creates a rotation matrix around an arbitrary axis and stores it in the result.
 * @param axis The axis of rotation (must be a unit vector).
 * @param angle_rad The angle of rotation in radians.
 * @param result A pointer to the matrix where the result will be stored.
 */
void mat4_rotate(Vec3 axis, float angle_rad, Mat4* result);


/* --- Camera and Projection Matrices --- */

/**
 * Creates a view matrix and stores it in the result.
 * This matrix transforms world coordinates into camera space, so that the camera
 * is effectively at the origin (0,0,0) looking down the -Z axis.
 *
 * @param eye The position of the camera in world space.
 * @param target The point the camera is looking at in world space.
 * @param up The 'up' direction for the camera (e.g., (0, 1, 0)).
 * @param result A pointer to the matrix where the result will be stored.
 */
void mat4_look_at(Vec3 eye, Vec3 target, Vec3 up, Mat4* result);

/**
 * Creates a perspective projection matrix and stores it in the result.
 * This matrix defines a viewing volume (a 'frustum' or truncated pyramid) and
 * maps all 3D coordinates within that volume into a canonical, normalized
 * cube.
 *
 * @param fov_y_rad The vertical field of view in radians.
 * @param aspect_ratio The width/height aspect ratio of the viewport.
 * @param near_plane The distance to the near clipping plane (must be > 0).
 * @param far_plane The distance to the far clipping plane.
 * @param result A pointer to the matrix where the result will be stored.
 */
void mat4_perspective(float fov_y_rad, float aspect_ratio, float near_plane, float far_plane, Mat4* result);

/**
 * Calculates the inverse of an affine matrix (composed of only rotation and translation).
 * @param m A pointer to the matrix to invert.
 * @param result A pointer to store the inverted matrix.
 */
void mat4_affine_inverse(const Mat4* m, Mat4* result);

#endif // MAT4_H