#ifndef JOINT_H
#define JOINT_H

#include "primitive.h"

enum class JointType {
    FIXED,    // Welds two objects together
    REVOLUTE  // A hinge or motor
};

class Joint {
public:
    Joint(Primitive* a, Primitive* b, JointType type);
    virtual ~Joint() = default;

    // The solver will call this to apply the constraint impulse
    virtual void apply_constraint(float dt) = 0;

protected:
    Primitive* bodyA;
    Primitive* bodyB;
    JointType type;

    // Anchor points in the local space of each body
    Vec3 local_anchor_a;
    Vec3 local_anchor_b;
};

class RevoluteJoint : public Joint {
public:
    RevoluteJoint(Primitive* a, Primitive* b, const Vec3& world_anchor, const Vec3& axis);

    void apply_constraint(float dt) override;

    void set_motor(float speed, float max_force);

private:
    Vec3 motor_axis;
    float motor_speed;
    float max_motor_force;
};

#endif // JOINT_H
