#pragma once

#include <cmath>

struct vec2
{
    float x;
    float y;
};

struct vec3
{
    float x;
    float y;
    float z;
};

struct vec4
{
    float x;
    float y;
    float z;
    float w;
};

inline vec2 vec2_make(float x, float y)
{
    return { x, y };
}

inline float vec2_cross(vec2 a, vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

inline vec3 vec3_make(float x, float y, float z)
{
    return { x, y, z };
}

inline vec3 vec3_add(vec3 a, vec3 b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

inline vec3 vec3_sub(vec3 a, vec3 b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

inline vec3 vec3_scale(vec3 a, float scale)
{
    return {
        a.x * scale,
        a.y * scale,
        a.z * scale
    };
}

inline float vec3_dot(vec3 a, vec3 b)
{
    return
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;
}

inline vec3 vec3_cross(vec3 a, vec3 b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline vec3 barycentric(float l0, float l1, float l2)
{
    return { l0, l1, l2 };
}

inline float vec3_length(vec3 a)
{
    return std::sqrt(vec3_dot(a, a));
}

inline vec3 vec3_normalize(vec3 a)
{
    float length = vec3_length(a);

    if (length > 0.0f)
        return vec3_scale(a, 1.0f / length);

    return a;
}

inline vec4 vec4_from_point(vec3 point)
{
    return {
        point.x,
        point.y,
        point.z,
        1.0f
    };
}