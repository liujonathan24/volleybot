#include "physics_core/collision.h"
#include "physics_core/vec3.h"
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

    // Get box min/max points
    Vec3 box_min, box_max;
    vec3_sub(&box_pos, &box_extents, &box_min);
    vec3_add(&box_pos, &box_extents, &box_max);

    // Find the closest point on the box to the sphere's center
    float closest_x = fmaxf(box_min.x, fminf(sphere_pos.x, box_max.x));
    float closest_y = fmaxf(box_min.y, fminf(sphere_pos.y, box_max.y));
    float closest_z = fmaxf(box_min.z, fminf(sphere_pos.z, box_max.z));

    Vec3 closest_point = {closest_x, closest_y, closest_z};

    // Check if the distance from the closest point to the sphere center is less than the radius
    Vec3 delta;
    vec3_sub(&sphere_pos, &closest_point, &delta);
    float dist_sq = vec3_length_sq(&delta);

    if (dist_sq < sphere_radius * sphere_radius) {
        info.has_collided = true;
        float dist = sqrtf(dist_sq);
        info.depth = sphere_radius - dist;
        if (dist > 0) {
            vec3_scale(&delta, 1.0f / dist, &info.normal);
        } else {
            // Sphere center is inside the box, find a good push-out direction
            // For simplicity, we'll push out from the center of the box
            vec3_sub(&sphere_pos, &box_pos, &info.normal);
            if (vec3_length_sq(&info.normal) < 1e-6) {
                vec3_set(&info.normal, 0, 1, 0); // Default push-out
            } else {
                vec3_normalize(&info.normal, &info.normal);
            }
        }
    }

    return info;
}
