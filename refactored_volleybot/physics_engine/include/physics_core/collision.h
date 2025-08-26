#ifndef COLLISION_H
#define COLLISION_H

#include "vec3.h"
#include <stdbool.h>

// A struct to hold the results of a collision test.
typedef struct {
    bool has_collided;
    Vec3 normal;    // Collision normal, pointing from object A away from object B
    float depth;    // The amount of overlap/penetration
} CollisionInfo;

/**
 * Tests for collision between two spheres.
 */
CollisionInfo test_sphere_vs_sphere(Vec3 pos_a, float radius_a, Vec3 pos_b, float radius_b);

/**
 * Tests for collision between a sphere and an axis-aligned box.
 * TODO: This is for you to implement!
 * A great resource is the Separating Axis Theorem (SAT).
 * See: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection#separating_axis_theorem
 */
CollisionInfo test_sphere_vs_box(Vec3 sphere_pos, float sphere_radius, Vec3 box_pos, Vec3 box_extents);

#endif // COLLISION_H
