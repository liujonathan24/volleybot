#ifndef LIGHT_H
#define LIGHT_H

#include "../physics_core/vec3.h"

class Light {
public:
    /**
     * Constructs a new Light.
     * @param position The light's position in world space.
     * @param color The RGB color of the light (values 0-1).
     * @param intensity The brightness multiplier.
     */
    Light(Vec3 position = {0, 10.0f, 0}, Vec3 color = {1.0f, 1.0f, 1.0f}, float intensity = 1.0f);
    ~Light();

    Vec3 position;
    Vec3 color;
    float intensity;
};

#endif // LIGHT_H
