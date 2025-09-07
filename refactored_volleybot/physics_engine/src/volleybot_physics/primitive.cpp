#include "volleybot_physics/primitive.h"
#include "physics_core/kinematics.h" 

// --- Primitive Base Class --- //

Primitive::Primitive(std::shared_ptr<Material> mat) : material(mat) {
    vec3_set(&position, 0, 0, 0);
    vec3_set(&velocity, 0, 0, 0);
    vec3_set(&acceleration, 0, 0, 0);

    vec3_set(&angular_velocity, 0, 0, 0);
    vec3_set(&center_of_mass, 0, 0, 0); // Default for single primitives
    mat4_identity(&transform);
    mat4_zero(&inertia_tensor); // Initialize to zero
    mat4_zero(&inverse_inertia_tensor); // Initialize to zero
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

void Primitive::apply_impulse(const Vec3& impulse, const Vec3& world_contact_point) {
    if (this->material->mass <= 0.0f) return; // Static objects don't move

    // 1. Update linear velocity
    Vec3 linear_velocity_change;
    vec3_scale(&impulse, 1.0f / this->material->mass, &linear_velocity_change);
    vec3_add(&this->velocity, &linear_velocity_change, &this->velocity);

    // 2. Update angular velocity
    // First, find the world-space center of mass
    Vec3 world_center_of_mass = mat4_transform_point(&this->transform, this->center_of_mass);

    // Calculate r, the vector from the CoM to the contact point
    Vec3 r;
    vec3_sub(&world_contact_point, &world_center_of_mass, &r);

    // Calculate torque: T = r x impulse
    Vec3 torque;
    vec3_cross(&r, &impulse, &torque);

    // Calculate change in angular velocity: delta_omega = I_inv * T
    Vec3 angular_velocity_change = mat4_transform_direction(&this->inverse_inertia_tensor, torque);
    vec3_add(&this->angular_velocity, &angular_velocity_change, &this->angular_velocity);
}

void Primitive::apply_angular_impulse(const Vec3& impulse) {
    if (this->material->mass <= 0.0f) return; // Static objects don't rotate from impulses

    Vec3 angular_velocity_change = mat4_transform_direction(&this->inverse_inertia_tensor, impulse);
    vec3_add(&this->angular_velocity, &angular_velocity_change, &this->angular_velocity);
}

Box::Box(const Vec3& extents, std::shared_ptr<Material> mat) 
    : Primitive(mat), extents(extents) {
    type = PrimitiveType::BOX;
    // Calculate inertia tensor for a solid box aligned with axes
    float mass = material->mass;
    float W = extents.x * 2.0f; // Assuming extents are half-widths
    float H = extents.y * 2.0f;
    float D = extents.z * 2.0f;

    inertia_tensor.m[0][0] = (1.0f / 12.0f) * mass * (H*H + D*D);
    inertia_tensor.m[1][1] = (1.0f / 12.0f) * mass * (W*W + D*D);
    inertia_tensor.m[2][2] = (1.0f / 12.0f) * mass * (W*W + H*H);
    mat3_inverse(&inertia_tensor, &inverse_inertia_tensor);
}

void Box::compute_aabb() {
    vec3_sub(&position, &extents, &aabb.min);
    vec3_add(&position, &extents, &aabb.max);
}

// --- Sphere Derived Class --- //

Sphere::Sphere(float radius, std::shared_ptr<Material> mat) 
    : Primitive(mat), radius(radius) {
    type = PrimitiveType::SPHERE;
    // Calculate inertia tensor for a solid sphere
    float I = (2.0f / 5.0f) * material->mass * radius * radius;
    inertia_tensor.m[0][0] = I;
    inertia_tensor.m[1][1] = I;
    inertia_tensor.m[2][2] = I;
    mat3_inverse(&inertia_tensor, &inverse_inertia_tensor);
}

void Sphere::compute_aabb() {
    Vec3 r_vec = {radius, radius, radius};
    vec3_sub(&position, &r_vec, &aabb.min);
    vec3_add(&position, &r_vec, &aabb.max);
}

// --- TriangleMesh Derived Class --- //

TriangleMesh::TriangleMesh(std::shared_ptr<Material> mat)
    : Primitive(mat) {
    type = PrimitiveType::MESH;
}

// --- Cylinder Derived Class --- //

Cylinder::Cylinder(float height, float radius, int sides, std::shared_ptr<Material> mat)
    : Primitive(mat), height(height), radius(radius), sides(sides) {
    type = PrimitiveType::CYLINDER;
}
