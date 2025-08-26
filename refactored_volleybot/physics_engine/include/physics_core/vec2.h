#ifndef VEC2_H
#define VEC2_H

// Represents a 2D vector with float components.
typedef struct {
    float x, y;
} Vec2;

/**
 * Creates a new 2D vector with the given components.
 * @param x The x-component.
 * @param y The y-component.
 * @return A new Vec2.
 */
Vec2 vec2_create(float x, float y);

#endif // VEC2_H
