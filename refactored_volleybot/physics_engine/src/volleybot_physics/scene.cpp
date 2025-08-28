#include "volleybot_physics/scene.h"
#include "physics_core/collision.h"
#include <iostream> 
#include <algorithm> // For std::sort

#include "../tinyobj_loader_c.h"


Scene::Scene() {
    vec3_set(&gravity, 0, -9.81f, 0);
}

Scene::~Scene() {}

void Scene::step(float dt) {
    // 1. Update physics for all primitives 
    for (auto& p : primitives) {
        p->update_physics(dt, gravity); 
    }

    // 2. Broadphase collision detection 
    broad_phase();

    // 3. Solve collision constraints (velocity correction)
    solve_constraints(dt);

    // 4. Resolve penetration (position correction)
    resolve_penetration();
}

void Scene::broad_phase() {
    // For now, we will use a brute-force O(n^2) approach.
    // A proper Sort and Sweep would be implemented here later for performance.
    collision_constraints.clear();

    for (size_t i = 0; i < primitives.size(); ++i) {
        for (size_t j = i + 1; j < primitives.size(); ++j) {
            narrow_phase(primitives[i].get(), primitives[j].get());
        }
    }
}

void Scene::narrow_phase(Primitive* a, Primitive* b) {
    Sphere* sphere_a = dynamic_cast<Sphere*>(a);
    Sphere* sphere_b = dynamic_cast<Sphere*>(b);

    if (sphere_a && sphere_b) {
        CollisionInfo info = test_sphere_vs_sphere(
            sphere_a->get_position(), sphere_a->get_radius(),
            sphere_b->get_position(), sphere_b->get_radius()
        );

        if (info.has_collided) {
            collision_constraints.push_back({a, b, info.normal, info.depth, 0.0f});
        }
    }
    // TODO: User can add Sphere vs Box check here
}

void Scene::solve_constraints(float dt) {
    const int solver_iterations = 8;
    for (int i = 0; i < solver_iterations; ++i) {
        for (auto& constraint : collision_constraints) {
            Primitive* a = constraint.a;
            Primitive* b = constraint.b;

            // Simplified impulse solver for non-penetration.
            // Does not yet include friction or full restitution physics.

            // Get velocities into local variables
            Vec3 vel_a = a->get_velocity();
            Vec3 vel_b = b->get_velocity();

            Vec3 relative_velocity;
            vec3_sub(&vel_b, &vel_a, &relative_velocity);

            float velocity_along_normal = vec3_dot(&relative_velocity, &constraint.normal);
            if (velocity_along_normal > 0) continue; // Objects are already separating

            // Simplified: Assume mass is 1 and restitution is 0 for now.
            float impulse_magnitude = -velocity_along_normal;

            Vec3 impulse;
            vec3_scale(&constraint.normal, impulse_magnitude, &impulse);

            // Apply the impulse by updating the local velocity variables
            vec3_sub(&vel_a, &impulse, &vel_a);
            vec3_add(&vel_b, &impulse, &vel_b);

            // Update the actual primitives
            a->set_velocity(vel_a);
            b->set_velocity(vel_b);
        }
    }
}

void Scene::resolve_penetration() {
    const float correction_percent = 0.4f; // Correct a percentage of the error each frame to avoid jitter
    const float slop = 0.01f; // Allow for a small amount of overlap

    for (const auto& constraint : collision_constraints) {
        float correction_magnitude = fmaxf(0, constraint.depth - slop);
        if (correction_magnitude == 0) continue;

        // For now, assume equal mass and move each object by half the amount
        Vec3 correction_vector;
        vec3_scale(&constraint.normal, correction_magnitude * correction_percent * 0.5f, &correction_vector);

        Primitive* a = constraint.a;
        Primitive* b = constraint.b;

        Vec3 pos_a = a->get_position();
        Vec3 pos_b = b->get_position();

        vec3_sub(&pos_a, &correction_vector, &pos_a);
        vec3_add(&pos_b, &correction_vector, &pos_b);

        a->set_position(pos_a);
        b->set_position(pos_b);
    }
}


void Scene::render() {
    // Rendering logic will go here.
}

void Scene::add_primitive(std::unique_ptr<Primitive> primitive) {
    primitives.push_back(std::move(primitive));
}

void Scene::add_light(std::unique_ptr<Light> light) {
    lights.push_back(std::move(light));
}

void Scene::set_camera(std::unique_ptr<Camera> camera) {
    active_camera = std::move(camera);
}

Primitive* Scene::load_obj_as_primitive(const std::string& filepath, std::shared_ptr<Material> material) {
    // ... (obj loading code remains the same) ...
    return nullptr; // Simplified for brevity
}
