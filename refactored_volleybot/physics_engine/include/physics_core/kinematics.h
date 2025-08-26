#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "vec3.h"

/**
 * Updates position and velocity using the Velocity Verlet integration method.
 * This is a core function of the physics engine.
 * @param position A pointer to the object's current position (will be updated).
 * @param velocity A pointer to the object's current velocity (will be updated).
 * @param acceleration A pointer to the object's constant acceleration for the time step.
 * @param dt The time step duration in seconds.
 */
void update_kinematics(Vec3* position, Vec3* velocity, const Vec3* acceleration, float dt);

#endif // KINEMATICS_H