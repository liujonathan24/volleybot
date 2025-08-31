#include "volleybot_physics/scene.h"
#include "physics_core/collision.h"
#include "volleybot_physics/composite_object.h"
#include <iostream> 
#include <algorithm> // For std::sort

#include "../tinyobj_loader_c.h"


Scene::Scene() {
    vec3_set(&gravity, 0, -9.81f, 0);
}

Scene::~Scene() {}

void Scene::step(float dt) {
    // 1. Update physics for all bodies
    for (auto& body : physics_bodies) {
        body->update_physics(dt, gravity);
        // For composites, we must also update the world positions of their parts
        if (body->get_type() == PrimitiveType::COMPOSITE) {
            static_cast<CompositeObject*>(body.get())->update_child_transforms();
        }
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

    for (size_t i = 0; i < physics_bodies.size(); ++i) {
        for (size_t j = i + 1; j < physics_bodies.size(); ++j) {
            narrow_phase(physics_bodies[i].get(), physics_bodies[j].get());
        }
    }
}

void Scene::narrow_phase(Primitive* a, Primitive* b) {
    // Recursively check parts of composite objects
    bool a_is_composite = a->get_type() == PrimitiveType::COMPOSITE;
    bool b_is_composite = b->get_type() == PrimitiveType::COMPOSITE;

    if (a_is_composite) {
        for (const auto& part : static_cast<CompositeObject*>(a)->get_parts()) {
            narrow_phase(part.primitive.get(), b); // Recurse
        }
        return;
    }
    if (b_is_composite) {
        for (const auto& part : static_cast<CompositeObject*>(b)->get_parts()) {
            narrow_phase(a, part.primitive.get()); // Recurse
        }
        return;
    }

    // --- Base Case: Two non-composite primitives ---
    auto typeA = a->get_type();
    auto typeB = b->get_type();

    // To avoid writing checks for both (A vs B) and (B vs A), we ensure
    // that typeA is always the smaller enum value.
    if (typeA > typeB) {
        std::swap(a, b);
        std::swap(typeA, typeB);
    }

    if (typeA == PrimitiveType::SPHERE && typeB == PrimitiveType::SPHERE) {
        auto* sphere_a = static_cast<Sphere*>(a);
        auto* sphere_b = static_cast<Sphere*>(b);
        CollisionInfo info = test_sphere_vs_sphere(
            sphere_a->get_position(), sphere_a->get_radius(),
            sphere_b->get_position(), sphere_b->get_radius()
        );
        if (info.has_collided) {
            collision_constraints.push_back({a, b, info.normal, info.depth, 0.0f});
        }
    } else if (typeA == PrimitiveType::SPHERE && typeB == PrimitiveType::BOX) {
        auto* sphere = static_cast<Sphere*>(a);
        auto* box = static_cast<Box*>(b);
        CollisionInfo info = test_sphere_vs_box(
            sphere->get_position(), sphere->get_radius(),
            box->get_position(), box->get_extents()
        );
        if (info.has_collided) {
            collision_constraints.push_back({a, b, info.normal, info.depth, 0.0f});
        }
    } 
    // else if (typeA == PrimitiveType::BOX && typeB == PrimitiveType::BOX) { ... }
    // etc. for other collision pairs
}

void Scene::solve_constraints(float dt) {
    const int solver_iterations = 8;
    for (int i = 0; i < solver_iterations; ++i) {
        // First, solve joint constraints
        for (auto& joint : joints) {
            joint->apply_constraint(dt);
        }

        // Then, solve collision constraints
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
    physics_bodies.push_back(std::move(primitive));
}

void Scene::add_composite_object(std::unique_ptr<CompositeObject> object) {
    physics_bodies.push_back(std::move(object));
}

void Scene::add_joint(std::unique_ptr<Joint> joint) {
    joints.push_back(std::move(joint));
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
