#include "physics_core/vec3.h"
#include <math.h> 

/* --- Vector Creation --- */
void vec3_set(Vec3* output, float x, float y, float z) {
    output->x = x;
    output->y = y;
    output->z = z; 
}

/* --- Core Arithmetic Operations --- */
void vec3_add(const Vec3* a, const Vec3* b, Vec3* result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void vec3_sub(const Vec3* a, const Vec3* b, Vec3* result) {
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

void vec3_scale(const Vec3* v, float s, Vec3* result) {
    result->x = v->x * s;
    result->y = v->y * s;
    result->z = v->z * s;
}

void vec3_negate(const Vec3* v, Vec3* result) {
    result->x = -v->x;
    result->y = -v->y;
    result->z = -v->z;
}

/* --- Vector Properties & Products --- */
float vec3_length_sq(const Vec3* v) {
    // Optimization: Direct multiplication is faster than pow()
    return (v->x * v->x) + (v->y * v->y) + (v->z * v->z);
}

float vec3_length(const Vec3* v) {
    // Optimization: sqrtf is faster than pow(n, 0.5)
    return sqrtf(vec3_length_sq(v));
}

void vec3_normalize(const Vec3* v, Vec3* result) {
    float len = vec3_length(v);
    // Safety check for division by zero
    if (len > 0.0f) {
        float inv_len = 1.0f / len;
        result->x = v->x * inv_len;
        result->y = v->y * inv_len;
        result->z = v->z * inv_len;
    } else {
        result->x = 0.0f;
        result->y = 0.0f;
        result->z = 0.0f;
    }
}

float vec3_dot(const Vec3* a, const Vec3* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

void vec3_cross(const Vec3* a, const Vec3* b, Vec3* result) {
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

/* --- Utility & Physics Functions --- */
float vec3_dist_sq(const Vec3* a, const Vec3* b) {
    Vec3 difference;
    vec3_sub(a, b, &difference);
    return vec3_length_sq(&difference);
}

float vec3_dist(const Vec3* a, const Vec3* b) {
    Vec3 difference;
    vec3_sub(a, b, &difference);
    return vec3_length(&difference);
}

void vec3_reflect(const Vec3* incident, const Vec3* normal, Vec3* result) {
    // Formula: R = I - 2 * dot(I, N) * N
    float dot_product = vec3_dot(incident, normal);
    Vec3 scaled_normal;
    vec3_scale(normal, 2.0f * dot_product, &scaled_normal);
    vec3_sub(incident, &scaled_normal, result);
}
