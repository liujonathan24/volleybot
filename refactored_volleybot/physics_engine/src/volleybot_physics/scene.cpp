#include "../../include/volleybot_physics/scene.h"
#include "../../include/physics_core/collision.h"
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
            // This is a simplified impulse solver and does not yet include friction or restitution.
            // It solves for the non-penetration constraint.
            Primitive* a = constraint.a;
            Primitive* b = constraint.b;

            // Simplified: Assume mass is 1 and restitution is 0 for now.
            Vec3 relative_velocity;
            vec3_sub(&b->get_velocity(), &a->get_velocity(), &relative_velocity);

            float velocity_along_normal = vec3_dot(&relative_velocity, &constraint.normal);
            if (velocity_along_normal > 0) continue; // Objects are separating

            float impulse_magnitude = -velocity_along_normal;

            Vec3 impulse;
            vec3_scale(&constraint.normal, impulse_magnitude, &impulse);

            a->set_velocity(vec3_sub(&a->get_velocity(), &impulse, &a->get_velocity()));
            b->set_velocity(vec3_add(&b->get_velocity(), &impulse, &b->get_velocity()));
        }
    }
}

void Scene::resolve_penetration() {
    // TODO: User implementation.
    // Loop through 'collision_constraints'. For each constraint:
    // 1. Get the two primitives (a and b) and the penetration depth.
    // 2. Calculate the amount to move each object (e.g., depth / 2).
    // 3. Create a correction vector by scaling the collision normal by the move amount.
    // 4. Apply the correction to each object's position (one positive, one negative).
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
