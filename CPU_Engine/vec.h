// ============================================================
//  vec.h — 3D vector type and operations
// ------------------------------------------------------------
//  See chapter README sections 4.3 to 4.7. Header-only: tiny
//  helpers marked `static inline` so they cost nothing extra
//  versus writing the math out by hand.
// ============================================================
#pragma once
#include <math.h>

typedef struct { float x, y, z; } vec3;

static inline vec3 vec3_make(float x, float y, float z) {
    vec3 v = { x, y, z };
    return v;
}

// a + b, component by component.
static inline vec3 vec3_add(vec3 a, vec3 b) {
    return vec3_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

// a - b. Geometrically: the arrow pointing from b to a.
static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return vec3_make(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Scale every component by s (stretch / shrink / flip).
static inline vec3 vec3_scale(vec3 a, float s) {
    return vec3_make(a.x * s, a.y * s, a.z * s);
}

// Dot product: a single number = |a||b|cos(angle).
// Positive when aligned, zero when perpendicular, negative when opposed.
static inline float vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cross product: a vector perpendicular to both a and b.
static inline vec3 vec3_cross(vec3 a, vec3 b) {
    return vec3_make(a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x);
}

// Length (magnitude) = sqrt(a . a).
static inline float vec3_length(vec3 a) {
    return sqrtf(vec3_dot(a, a));
}

// Unit vector pointing the same way. The zero vector is returned
// unchanged (rather than producing NaN) so callers don't have to
// guard against it.
static inline vec3 vec3_normalize(vec3 a) {
    float len = vec3_length(a);
    if (len > 0.0f) return vec3_scale(a, 1.0f / len);
    return a;
}