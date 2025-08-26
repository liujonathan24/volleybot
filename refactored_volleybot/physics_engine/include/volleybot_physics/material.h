#ifndef MATERIAL_H
#define MATERIAL_H

#include "../physics_core/vec3.h"

// A simple data struct to define the properties of a surface.
struct Material {
    // --- Physical Properties
    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.5f;  // Bounciness

    // --- Visual Properties
    Vec3 color = {0.8f, 0.8f, 0.8f};
    float shininess = 32.0f;
};

#endif // MATERIAL_H
