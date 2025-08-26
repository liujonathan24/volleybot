#include "volleybot_physics/light.h"

Light::Light(Vec3 position, Vec3 color, float intensity)
    : position(position), color(color), intensity(intensity) {
    // Constructor body is empty as we use a member initializer list
}

Light::~Light() {
    // Destructor body is empty
}
