#include "collision.h"
#include "vec3.h"
#include <math.h>

CollisionInfo test_sphere_vs_sphere(Vec3 pos_a, float radius_a, Vec3 pos_b, float radius_b) {
    CollisionInfo info = {false};

    Vec3 delta;
    vec3_sub(&pos_b, &pos_a, &delta);
    
    float dist_sq = vec3_length_sq(&delta);
    float radii_sum = radius_a + radius_b;

    if (dist_sq < radii_sum * radii_sum) {
        info.has_collided = true;
        float dist = sqrtf(dist_sq);
        info.depth = radii_sum - dist;
        if (dist > 0) {
            vec3_scale(&delta, 1.0f / dist, &info.normal);
        } else {
            // Spheres are in the exact same position, push apart on Y axis
            vec3_set(&info.normal, 0, 1, 0);
        }
    }
    return info;
}

CollisionInfo test_sphere_vs_box(Vec3 sphere_pos, float sphere_radius, Vec3 box_pos, Vec3 box_extents) {
    CollisionInfo info = {false};
    // 1. Find the closest point on the AABB to the sphere's center.
    Vec3 closest_point_on_square;
    // 2. Calculate the distance between the closest point and the sphere's center.
    float dist = vec3_dist(sphere_pos, closest_point_on_square);
    // 3. If the distance is less than the sphere's radius, they are colliding.
    if (dist < sphere_radius) {
        info.has_collided = true;
        info.depth = dist - sphere_radius;
    }
    // 4. The collision normal is the vector from the closest point to the sphere center.
    // 5. The depth is the sphere's radius minus the distance.
    return info;
}
