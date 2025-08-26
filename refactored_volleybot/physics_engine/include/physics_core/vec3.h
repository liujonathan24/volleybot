#ifndef VEC3_H
#define VEC3_H

#include <math.h> // For sqrtf

// Represents a 3D vector with float components.
typedef struct {
    float x, y, z;
} Vec3;

/* --- Vector Creation --- */

/**
 * Sets the components of a vector.
 * @param output A pointer to the vector to be modified.
 * @param x The x-component.
 * @param y The y-component.
 * @param z The z-component.
 */
void vec3_set(Vec3* output, float x, float y, float z);

/* --- Core Arithmetic Operations --- */

/**
 * Adds two vectors component-wise (a + b) and stores the result.
 * @param a A pointer to the first vector.
 * @param b A pointer to the second vector.
 * @param result A pointer to the vector where the result will be stored.
 */
void vec3_add(const Vec3* a, const Vec3* b, Vec3* result);

/**
 * Subtracts the second vector from the first (a - b) and stores the result.
 * @param a A pointer to the first vector.
 * @param b A pointer to the second vector.
 * @param result A pointer to the vector where the result will be stored.
 */
void vec3_sub(const Vec3* a, const Vec3* b, Vec3* result);

/**
 * Scales a vector by a scalar value (v * s) and stores the result.
 * @param v A pointer to the vector to scale.
 * @param s The scalar value.
 * @param result A pointer to the vector where the scaled result will be stored.
 */
void vec3_scale(const Vec3* v, float s, Vec3* result);

/**
 * Negates a vector and stores the result.
 * @param v A pointer to the vector to negate.
 * @param result A pointer to the vector where the negated result will be stored.
 */
void vec3_negate(const Vec3* v, Vec3* result);


/* --- Vector Properties & Products --- */

/**
 * Calculates the squared length (magnitude) of a vector.
 * @param v A pointer to the vector.
 * @return The squared length.
 */
float vec3_length_sq(const Vec3* v);

/**
 * Calculates the length (magnitude) of a vector.
 * @param v A pointer to the vector.
 * @return The length.
 */
float vec3_length(const Vec3* v);

/**
 * Normalizes a vector to have a length of 1 and stores the result.
 * @param v A pointer to the vector to normalize.
 * @param result A pointer to the vector where the normalized result will be stored.
 */
void vec3_normalize(const Vec3* v, Vec3* result);

/**
 * Calculates the dot product of two vectors.
 * @param a A pointer to the first vector.
 * @param b A pointer to the second vector.
 * @return The dot product.
 */
float vec3_dot(const Vec3* a, const Vec3* b);

/**
 * Calculates the cross product of two vectors and stores the result.
 * @param a A pointer to the first vector.
 * @param b A pointer to the second vector.
 * @param result A pointer to the vector where the cross product will be stored.
 */
void vec3_cross(const Vec3* a, const Vec3* b, Vec3* result);


/* --- Utility & Physics Functions --- */

/**
 * Calculates the squared distance between two vector points.
 * @param a A pointer to the first point.
 * @param b A pointer to the second point.
 * @return The squared distance.
 */
float vec3_dist_sq(const Vec3* a, const Vec3* b);

/**
 * Calculates the distance between two vector points.
 * @param a A pointer to the first point.
 * @param b A pointer to the second point.
 * @return The distance.
 */
float vec3_dist(const Vec3* a, const Vec3* b);

/**
 * Calculates the reflection of an incident vector off a surface and stores the result.
 * @param incident A pointer to the incoming vector.
 * @param normal A pointer to the surface normal (must be a unit vector).
 * @param result A pointer to the vector where the reflected result will be stored.
 */
void vec3_reflect(const Vec3* incident, const Vec3* normal, Vec3* result);

#endif // VEC3_H
