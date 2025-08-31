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

void RevoluteJoint::apply_constraint(float dt) {
    // This is where the magic happens. A real implementation would:
    // 1. Calculate the relative velocity between bodyA and bodyB at the anchor point.
    // 2. Identify the velocity components that violate the constraint (e.g., movement not along the hinge axis).
    // 3. Calculate the impulse (force) needed to correct this velocity.
    // 4. Apply the impulse to bodyA and bodyB.
    // 5. If a motor is active, calculate and apply the motor impulse.

    // For now, this is a placeholder.
}
