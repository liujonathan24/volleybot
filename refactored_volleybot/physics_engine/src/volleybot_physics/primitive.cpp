#include "../../include/volleybot_physics/primitive.h"
#include "../../include/physics_core/kinematics.h" // We will create this C header later

// --- Primitive Base Class --- //

Primitive::Primitive(std::shared_ptr<Material> mat) : material(mat) {
    vec3_set(&position, 0, 0, 0);
    vec3_set(&velocity, 0, 0, 0);
    vec3_set(&acceleration, 0, 0, 0);
    mat4_identity(&transform);
}

void Primitive::update_physics(float dt, Vec3 gravity) {
    // Apply gravity to the current acceleration
    vec3_add(&acceleration, &gravity, &acceleration);

    // Call the C core function to perform the Verlet integration
    update_kinematics(&position, &velocity, &acceleration, dt);

    // Reset acceleration for the next frame. Forces must be re-applied each frame.
    vec3_set(&acceleration, 0, 0, 0);

    // Update the object's transformation matrix from its new position
    // Note: This doesn't account for rotation yet.
    mat4_translate(position, &transform);
}

void Primitive::set_position(const Vec3& pos) {
    position = pos;
    // Update the transform matrix whenever position changes
    mat4_translate(position, &transform);
}

void Primitive::set_velocity(const Vec3& vel) {
    velocity = vel;
}

void Primitive::apply_force(const Vec3& force) {
    // F = ma  =>  a = F/m
    if (material && material->mass > 0.0f) {
        Vec3 scaled_force;
        vec3_scale(&force, 1.0f / material->mass, &scaled_force);
        vec3_add(&acceleration, &scaled_force, &acceleration);
    }
}

// --- Sphere Derived Class --- //

Sphere::Sphere(float radius, std::shared_ptr<Material> mat) 
    : Primitive(mat), radius(radius) {}

// --- Box Derived Class --- //

Box::Box(const Vec3& extents, std::shared_ptr<Material> mat) 
    : Primitive(mat), extents(extents) {}

// --- TriangleMesh Derived Class --- //

TriangleMesh::TriangleMesh(std::shared_ptr<Material> mat)
    : Primitive(mat) {}

// --- Cylinder Derived Class --- //

Cylinder::Cylinder(float height, float radius, int sides, std::shared_ptr<Material> mat)
    : Primitive(mat), height(height), radius(radius), sides(sides) {}
