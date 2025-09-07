#include "volleybot_physics/joint.h"

// --- Base Joint --- //
Joint::Joint(Primitive* a, Primitive* b, JointType type)
    : bodyA(a), bodyB(b), type(type) {}


// --- Revolute Joint --- //
RevoluteJoint::RevoluteJoint(Primitive* a, Primitive* b, const Vec3& world_anchor, const Vec3& axis)
    : Joint(a, b, JointType::REVOLUTE), motor_axis(axis), motor_speed(0.0f), max_motor_force(0.0f) {

    // Convert the world-space anchor point into the local space of each body.
    // This is crucial because the bodies will move, but their local anchor points remain constant.
    Mat4 inv_transform_a, inv_transform_b;
    mat4_affine_inverse(&a->get_transform(), &inv_transform_a);
    mat4_affine_inverse(&b->get_transform(), &inv_transform_b);

    this->local_anchor_a = mat4_transform_point(&inv_transform_a, world_anchor);
    this->local_anchor_b = mat4_transform_point(&inv_transform_b, world_anchor);
}

void RevoluteJoint::set_motor(float speed, float max_force) {
    motor_speed = speed;
    max_motor_force = max_force;
}

float RevoluteJoint::get_relative_speed() const {
    Vec3 omega_a = bodyA->get_angular_velocity();
    Vec3 omega_b = bodyB->get_angular_velocity();
    Vec3 relative_omega;
    vec3_sub(&omega_b, &omega_a, &relative_omega);
    return vec3_dot(&relative_omega, &this->motor_axis);
}

void RevoluteJoint::apply_constraint(float dt) {
    // --- Part 1: Motor --- 
    if (this->max_motor_force > 0.0f) {
        // Calculate effective mass for the angular motor
        Vec3 inv_I_a_axis = mat4_transform_direction(&bodyA->get_inverse_inertia_tensor(), this->motor_axis);
        Vec3 inv_I_b_axis = mat4_transform_direction(&bodyB->get_inverse_inertia_tensor(), this->motor_axis);
        float effective_mass_angular = vec3_dot(&inv_I_a_axis, &this->motor_axis) + vec3_dot(&inv_I_b_axis, &this->motor_axis);

        // Calculate the impulse to reach the target speed
        float current_speed = get_relative_speed();
        float speed_error = this->motor_speed - current_speed;
        float impulse_magnitude = 0.0f;
        if (effective_mass_angular < 1e-6f) {
            impulse_magnitude = 0.0f;
        } else {
            impulse_magnitude = speed_error / effective_mass_angular;
        }

        // Clamp by max force
        float max_impulse = this->max_motor_force * dt;
        impulse_magnitude = fmaxf(-max_impulse, fminf(impulse_magnitude, max_impulse));

        // Apply the angular impulse
        Vec3 angular_impulse;
        vec3_scale(&this->motor_axis, impulse_magnitude, &angular_impulse);
        bodyA->apply_angular_impulse(angular_impulse);
        Vec3 negative_impulse;
        vec3_negate(&angular_impulse, &negative_impulse);
        bodyB->apply_angular_impulse(negative_impulse);
    }

    // --- Part 2: Hinge Constraint (Iterative Solver) ---
    // This part ensures the anchor points on both bodies stick together.
    // We solve this iteratively for each axis (x, y, z).
    const int iterations = 8; // Number of solver iterations for stability
    for (int iter = 0; iter < iterations; ++iter) {
        Vec3 world_anchor_a = mat4_transform_point(&bodyA->get_transform(), this->local_anchor_a);
        Vec3 world_anchor_b = mat4_transform_point(&bodyB->get_transform(), this->local_anchor_b);

        Vec3 r_a, r_b;
        Vec3 bodyA_world_com = mat4_transform_point(&bodyA->get_transform(), bodyA->get_center_of_mass());
        Vec3 bodyB_world_com = mat4_transform_point(&bodyB->get_transform(), bodyB->get_center_of_mass());
        vec3_sub(&world_anchor_a, &bodyA_world_com, &r_a);
        vec3_sub(&world_anchor_b, &bodyB_world_com, &r_b);

        Vec3 v_a_angular, v_b_angular;
        Vec3 ang_vel_a = bodyA->get_angular_velocity();
        Vec3 ang_vel_b = bodyB->get_angular_velocity();
        vec3_cross(&ang_vel_a, &r_a, &v_a_angular);
        vec3_cross(&ang_vel_b, &r_b, &v_b_angular);
        Vec3 v_a, v_b;
        Vec3 lin_vel_a = bodyA->get_velocity();
        Vec3 lin_vel_b = bodyB->get_velocity();
        vec3_add(&lin_vel_a, &v_a_angular, &v_a);
        vec3_add(&lin_vel_b, &v_b_angular, &v_b);

        Vec3 relative_velocity;
        vec3_sub(&v_b, &v_a, &relative_velocity);

        // Positional correction (Baumgarte Stabilization)
        Vec3 position_error;
        vec3_sub(&world_anchor_b, &world_anchor_a, &position_error);
        float beta = 0.2f; // Correction factor
        float slop = 0.01f;
        float correction_magnitude = fmaxf(0, vec3_length(&position_error) - slop);
        Vec3 correction_velocity;
        vec3_scale(&position_error, beta / dt * correction_magnitude, &correction_velocity);
        
        Vec3 total_velocity_error;
        vec3_add(&relative_velocity, &correction_velocity, &total_velocity_error);

        // Solve for impulse along each axis (x, y, z)
        Vec3 normal_x = {1,0,0}; Vec3 normal_y = {0,1,0}; Vec3 normal_z = {0,0,1};
        Vec3 normals[3] = {normal_x, normal_y, normal_z};

        for (int axis_idx = 0; axis_idx < 3; ++axis_idx) {
            Vec3 normal = normals[axis_idx];
            float current_relative_speed = vec3_dot(&total_velocity_error, &normal);

            float inv_mass_a = bodyA->get_material()->mass > 0 ? 1.0f / bodyA->get_material()->mass : 0.0f;
            float inv_mass_b = bodyB->get_material()->mass > 0 ? 1.0f / bodyB->get_material()->mass : 0.0f;

            Vec3 r_a_cross_n, r_b_cross_n;
            vec3_cross(&r_a, &normal, &r_a_cross_n);
            vec3_cross(&r_b, &normal, &r_b_cross_n);

            Vec3 I_inv_r_a_cross_n = mat4_transform_direction(&bodyA->get_inverse_inertia_tensor(), r_a_cross_n);
            Vec3 I_inv_r_b_cross_n = mat4_transform_direction(&bodyB->get_inverse_inertia_tensor(), r_b_cross_n);

            float angular_component = vec3_dot(&I_inv_r_a_cross_n, &r_a_cross_n) + vec3_dot(&I_inv_r_b_cross_n, &r_b_cross_n);
            float effective_mass = inv_mass_a + inv_mass_b + angular_component;

            if (effective_mass > 1e-6f) {
                float impulse_magnitude = -current_relative_speed / effective_mass;
                Vec3 impulse;
                vec3_scale(&normal, impulse_magnitude, &impulse);

                bodyA->apply_impulse(impulse, world_anchor_a);
                Vec3 negative_impulse;
                vec3_negate(&impulse, &negative_impulse);
                bodyB->apply_impulse(negative_impulse, world_anchor_b);
            }
        }
    }
}
