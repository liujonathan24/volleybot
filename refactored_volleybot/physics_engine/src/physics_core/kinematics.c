#include "physics_core/kinematics.h"

void update_kinematics(Vec3* position, Vec3* velocity, const Vec3* acceleration, float dt) {
    // Using the Velocity Verlet integration scheme.
    // This provides better stability than simple Euler integration.

    // 1. Update position: p_new = p_old + v_old*dt + 0.5*a*dt*dt
    Vec3 scaled_velocity;
    vec3_scale(velocity, dt, &scaled_velocity);

    Vec3 scaled_acceleration;
    vec3_scale(acceleration, 0.5f * dt * dt, &scaled_acceleration);

    vec3_add(position, &scaled_velocity, position);
    vec3_add(position, &scaled_acceleration, position);

    // 2. Update velocity: v_new = v_old + a*dt
    // A full Velocity Verlet implementation would use the average of the old and new acceleration
    // for higher accuracy, but for many simulations (especially with constant gravity),
    // this simplified version is common, stable, and effective.
    Vec3 velocity_change;
    vec3_scale(acceleration, dt, &velocity_change);
    vec3_add(velocity, &velocity_change, velocity);
}
