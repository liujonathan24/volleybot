#ifndef COLLISION_H
#define COLLISION_H

#include "vec3.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// A struct to hold the results of a collision test.
typedef struct {
    bool has_collided;
    Vec3 normal;    // Collision normal, pointing from object A away from object B
    float depth;    // The amount of overlap/penetration
} CollisionInfo;

CollisionInfo test_sphere_vs_sphere(Vec3 pos_a, float radius_a, Vec3 pos_b, float radius_b);
CollisionInfo test_sphere_vs_box(Vec3 sphere_pos, float sphere_radius, Vec3 box_pos, Vec3 box_extents);

#ifdef __cplusplus
}
#endif

#endif // COLLISION_H