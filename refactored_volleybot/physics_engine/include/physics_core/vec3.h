#ifndef VEC3_H
#define VEC3_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// Represents a 3D vector with float components.
typedef struct {
    float x, y, z;
} Vec3;

#ifdef __cplusplus
// C++ operator overloads for pybind11
inline Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(const Vec3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }
#endif

/* --- Vector Creation --- */
void vec3_set(Vec3* output, float x, float y, float z);
/* --- Core Arithmetic Operations --- */
void vec3_add(const Vec3* a, const Vec3* b, Vec3* result);
void vec3_sub(const Vec3* a, const Vec3* b, Vec3* result);
void vec3_scale(const Vec3* v, float s, Vec3* result);
void vec3_negate(const Vec3* v, Vec3* result);
/* --- Vector Properties & Products --- */
float vec3_length_sq(const Vec3* v);
float vec3_length(const Vec3* v);
void vec3_normalize(const Vec3* v, Vec3* result);
float vec3_dot(const Vec3* a, const Vec3* b);
void vec3_cross(const Vec3* a, const Vec3* b, Vec3* result);
/* --- Utility & Physics Functions --- */
float vec3_dist_sq(const Vec3* a, const Vec3* b);
float vec3_dist(const Vec3* a, const Vec3* b);
void vec3_reflect(const Vec3* incident, const Vec3* normal, Vec3* result);

#ifdef __cplusplus
}
#endif

#endif // VEC3_H