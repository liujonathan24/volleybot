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
        // Get joints from composite objects and apply their constraints
        for (auto& body : physics_bodies) {
            if (body->get_type() == PrimitiveType::COMPOSITE) {
                auto* composite = static_cast<CompositeObject*>(body.get());
                for (auto& joint : composite->get_joints()) {
                    joint->apply_constraint(dt);
                }
            }
        }

        // Then, solve collision constraints
        for (auto& constraint : collision_constraints) {
            Primitive* a = constraint.a;
            Primitive* b = constraint.b;

            // --- REALISTIC IMPULSE SOLVER ---

            Vec3 pos_a = a->get_position();
            Vec3 pos_b = b->get_position();
            Vec3 contact_point = {0,0,0}; // Simplified: average of positions
            vec3_add(&pos_a, &pos_b, &contact_point);
            vec3_scale(&contact_point, 0.5f, &contact_point);

            // Get vectors from CoM to contact point
            Vec3 r_a, r_b;
            Vec3 world_com_a = mat4_transform_point(&a->get_transform(), a->get_center_of_mass());
            Vec3 world_com_b = mat4_transform_point(&b->get_transform(), b->get_center_of_mass());
            vec3_sub(&contact_point, &world_com_a, &r_a);
            vec3_sub(&contact_point, &world_com_b, &r_b);

            // Get velocity at contact point
            Vec3 v_a_angular, v_b_angular;
            Vec3 ang_vel_a = a->get_angular_velocity();
            Vec3 ang_vel_b = b->get_angular_velocity();
            vec3_cross(&ang_vel_a, &r_a, &v_a_angular);
            vec3_cross(&ang_vel_b, &r_b, &v_b_angular);
            Vec3 v_a, v_b;
            Vec3 lin_vel_a = a->get_velocity();
            Vec3 lin_vel_b = b->get_velocity();
            vec3_add(&lin_vel_a, &v_a_angular, &v_a);
            vec3_add(&lin_vel_b, &v_b_angular, &v_b);

            // Calculate relative velocity
            Vec3 relative_velocity;
            vec3_sub(&v_b, &v_a, &relative_velocity);
            float velocity_along_normal = vec3_dot(&relative_velocity, &constraint.normal);

            // Do nothing if objects are already separating
            if (velocity_along_normal > 0) continue;

            // Calculate restitution (bounciness)
            float e = fminf(a->get_material()->restitution, b->get_material()->restitution);

            // Calculate effective mass (K)
            float inv_mass_a = a->get_material()->mass > 0 ? 1.0f / a->get_material()->mass : 0.0f;
            float inv_mass_b = b->get_material()->mass > 0 ? 1.0f / b->get_material()->mass : 0.0f;
            
            Vec3 r_a_cross_n, r_b_cross_n;
            vec3_cross(&r_a, &constraint.normal, &r_a_cross_n);
            vec3_cross(&r_b, &constraint.normal, &r_b_cross_n);

            Vec3 I_inv_r_a_cross_n = mat4_transform_direction(&a->get_inverse_inertia_tensor(), r_a_cross_n);
            Vec3 I_inv_r_b_cross_n = mat4_transform_direction(&b->get_inverse_inertia_tensor(), r_b_cross_n);

            float angular_component = vec3_dot(&I_inv_r_a_cross_n, &r_a_cross_n) + vec3_dot(&I_inv_r_b_cross_n, &r_b_cross_n);
            float effective_mass = inv_mass_a + inv_mass_b + angular_component;

            // Prevent division by zero or very small numbers
            if (effective_mass < 1e-6f) continue;

            // Calculate impulse magnitude (j)
            float j = -(1.0f + e) * velocity_along_normal / effective_mass;

            // Calculate tangential velocity
            Vec3 current_v_a_linear = a->get_velocity();
            Vec3 current_v_b_linear = b->get_velocity();
            Vec3 current_v_a_angular, current_v_b_angular;
            Vec3 ang_vel_a = a->get_angular_velocity();
            Vec3 ang_vel_b = b->get_angular_velocity();
            vec3_cross(&ang_vel_a, &r_a, &current_v_a_angular);
            vec3_cross(&ang_vel_b, &r_b, &current_v_b_angular);
            Vec3 current_v_a, current_v_b;
            vec3_add(&current_v_a_linear, &current_v_a_angular, &current_v_a);
            vec3_add(&current_v_b_linear, &current_v_b_angular, &current_v_b);

            Vec3 current_relative_velocity;
            vec3_sub(&current_v_b, &current_v_a, &current_relative_velocity);

            // Calculate tangential direction
            Vec3 tangent_direction_unnormalized;
            vec3_scale(&constraint.normal, vec3_dot(&current_relative_velocity, &constraint.normal), &tangent_direction_unnormalized);
            vec3_sub(&current_relative_velocity, &tangent_direction_unnormalized, &tangent_direction_unnormalized);
            
            float tangent_speed = vec3_length(&tangent_direction_unnormalized);
            if (tangent_speed < 1e-6) continue; // No tangential velocity

            Vec3 tangent_direction;
            vec3_scale(&tangent_direction_unnormalized, 1.0f / tangent_speed, &tangent_direction); // Normalize

            // Calculate tangential effective mass
            Vec3 r_a_cross_t, r_b_cross_t;
            vec3_cross(&r_a, &tangent_direction, &r_a_cross_t);
            vec3_cross(&r_b, &tangent_direction, &r_b_cross_t);

            Vec3 I_inv_r_a_cross_t = mat4_transform_direction(&a->get_inverse_inertia_tensor(), r_a_cross_t);
            Vec3 I_inv_r_b_cross_t = mat4_transform_direction(&b->get_inverse_inertia_tensor(), r_b_cross_t);

            float angular_component_t = vec3_dot(&I_inv_r_a_cross_t, &r_a_cross_t) + vec3_dot(&I_inv_r_b_cross_t, &r_b_cross_t);
            float effective_mass_t = inv_mass_a + inv_mass_b + angular_component_t;

            if (effective_mass_t < 1e-6f) continue;

            // Calculate tangential impulse magnitude
            float jt = -vec3_dot(&current_relative_velocity, &tangent_direction) / effective_mass_t;

            // Apply Coulomb friction model (clamp tangential impulse by normal impulse)
            float combined_friction = fminf(a->get_material()->friction, b->get_material()->friction);
            float max_friction_impulse = combined_friction * j; // j is the normal impulse magnitude

            jt = fmaxf(-max_friction_impulse, fminf(jt, max_friction_impulse));

            // Apply tangential impulse
            Vec3 friction_impulse;
            vec3_scale(&tangent_direction, jt, &friction_impulse);

            a->apply_impulse(friction_impulse, contact_point);
            Vec3 negative_friction_impulse;
            vec3_negate(&friction_impulse, &negative_friction_impulse);
            b->apply_impulse(negative_friction_impulse, contact_point);

            // --- FRICTION IMPULSE ---
            // Recalculate relative velocity after normal impulse is applied
            Vec3 v_a_friction, v_b_friction;
            Vec3 temp_v_a_f = a->get_velocity();
            Vec3 temp_v_b_f = b->get_velocity();
            Vec3 temp_av_a_f = a->get_angular_velocity();
            Vec3 temp_av_b_f = b->get_angular_velocity();

            vec3_cross(&temp_av_a_f, &r_a, &v_a_angular);
            vec3_cross(&temp_av_b_f, &r_b, &v_b_angular);
            vec3_add(&temp_v_a_f, &v_a_angular, &v_a_friction);
            vec3_add(&temp_v_b_f, &v_b_angular, &v_b_friction);

            Vec3 relative_velocity_friction;
            vec3_sub(&v_b_friction, &v_a_friction, &relative_velocity_friction);

            Vec3 tangent_direction;
            vec3_scale(&constraint.normal, vec3_dot(&relative_velocity_friction, &constraint.normal), &tangent_direction);
            vec3_sub(&relative_velocity_friction, &tangent_direction, &tangent_direction);
            
            float tangent_speed = vec3_length(&tangent_direction);
            if (tangent_speed > 1e-6f) {
                vec3_scale(&tangent_direction, 1.0f / tangent_speed, &tangent_direction); // Normalize

                // Calculate tangential effective mass
                Vec3 r_a_cross_t, r_b_cross_t;
                vec3_cross(&r_a, &tangent_direction, &r_a_cross_t);
                vec3_cross(&r_b, &tangent_direction, &r_b_cross_t);

                Vec3 I_inv_r_a_cross_t = mat4_transform_direction(&a->get_inverse_inertia_tensor(), r_a_cross_t);
                Vec3 I_inv_r_b_cross_t = mat4_transform_direction(&b->get_inverse_inertia_tensor(), r_b_cross_t);

                float angular_component_t = vec3_dot(&I_inv_r_a_cross_t, &r_a_cross_t) + vec3_dot(&I_inv_r_b_cross_t, &r_b_cross_t);
                float effective_mass_t = inv_mass_a + inv_mass_b + angular_component_t;

                if (effective_mass_t > 1e-6f) {
                    // Calculate tangential impulse magnitude
                    float jt = -vec3_dot(&relative_velocity_friction, &tangent_direction) / effective_mass_t;

                    // Apply Coulomb friction model (clamp tangential impulse by normal impulse)
                    float combined_friction = fminf(a->get_material()->friction, b->get_material()->friction);
                    float max_friction_impulse = combined_friction * j; // j is the normal impulse magnitude

                    jt = fmaxf(-max_friction_impulse, fminf(jt, max_friction_impulse));

                    // Apply tangential impulse
                    Vec3 friction_impulse;
                    vec3_scale(&tangent_direction, jt, &friction_impulse);

                    a->apply_impulse(friction_impulse, contact_point);
                    Vec3 negative_friction_impulse;
                    vec3_negate(&friction_impulse, &negative_friction_impulse);
                    b->apply_impulse(negative_friction_impulse, contact_point);
                }
            }
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

void Scene::add_primitive(std::shared_ptr<Primitive> primitive) {
    physics_bodies.push_back(primitive);
}

void Scene::add_composite_object(std::shared_ptr<CompositeObject> object) {
    physics_bodies.push_back(object);
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
