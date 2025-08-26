#ifndef VEC2_H
#define VEC2_H

#ifdef __cplusplus
extern "C" {
#endif

// Represents a 2D vector with float components.
typedef struct { float x, y; } Vec2;

Vec2 vec2_create(float x, float y);

#ifdef __cplusplus
}
#endif

#endif // VEC2_H