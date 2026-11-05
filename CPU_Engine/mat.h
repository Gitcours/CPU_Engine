#pragma once

#include "vec.h"

#include <cmath>

struct mat4
{
    float m[4][4];
};

inline mat4 mat4_identity()
{
    mat4 result{};

    for (int i = 0; i < 4; i++)
        result.m[i][i] = 1.0f;

    return result;
}

inline mat4 mat4_mul(mat4 a, mat4 b)
{
    mat4 result{};

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            for (int k = 0; k < 4; k++)
            {
                result.m[row][col] +=
                    a.m[row][k] * b.m[k][col];
            }
        }
    }

    return result;
}

inline vec4 mat4_mul_vec4(mat4 matrix, vec4 vector)
{
    vec4 result;

    result.x =
        matrix.m[0][0] * vector.x +
        matrix.m[0][1] * vector.y +
        matrix.m[0][2] * vector.z +
        matrix.m[0][3] * vector.w;

    result.y =
        matrix.m[1][0] * vector.x +
        matrix.m[1][1] * vector.y +
        matrix.m[1][2] * vector.z +
        matrix.m[1][3] * vector.w;

    result.z =
        matrix.m[2][0] * vector.x +
        matrix.m[2][1] * vector.y +
        matrix.m[2][2] * vector.z +
        matrix.m[2][3] * vector.w;

    result.w =
        matrix.m[3][0] * vector.x +
        matrix.m[3][1] * vector.y +
        matrix.m[3][2] * vector.z +
        matrix.m[3][3] * vector.w;

    return result;
}

inline mat4 mat4_translation(float x, float y, float z)
{
    mat4 result = mat4_identity();

    result.m[0][3] = x;
    result.m[1][3] = y;
    result.m[2][3] = z;

    return result;
}

inline mat4 mat4_scale(float scale)
{
    mat4 result = mat4_identity();

    result.m[0][0] = scale;
    result.m[1][1] = scale;
    result.m[2][2] = scale;

    return result;
}

inline mat4 mat4_rotation_x(float angle)
{
    mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[1][1] = c;
    result.m[1][2] = -s;

    result.m[2][1] = s;
    result.m[2][2] = c;

    return result;
}

inline mat4 mat4_rotation_y(float angle)
{
    mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0][0] = c;
    result.m[0][2] = s;

    result.m[2][0] = -s;
    result.m[2][2] = c;

    return result;
}

inline mat4 mat4_rotation_z(float angle)
{
    mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0][0] = c;
    result.m[0][1] = -s;

    result.m[1][0] = s;
    result.m[1][1] = c;

    return result;
}

inline float radians(float degrees)
{
    return degrees * 3.14159265358979323846f / 180.0f;
}

inline mat4 mat4_perspective(
    float fov,
    float aspect,
    float nearPlane,
    float farPlane)
{
    mat4 result{};

    float yScale = 1.0f / tanf(fov / 2.0f);
    float xScale = yScale / aspect;

    result.m[0][0] = xScale;
    result.m[1][1] = yScale;

    result.m[2][2] =
        (farPlane + nearPlane) / (nearPlane - farPlane);

    result.m[2][3] =
        (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);

    result.m[3][2] = -1.0f;

    return result;
}

inline mat4 mat4_look_at(vec3 eye, vec3 target, vec3 up)
{
    vec3 forward = vec3_normalize(vec3_sub(eye, target));

    vec3 right = vec3_normalize(vec3_cross(up, forward));

    vec3 cameraUp = vec3_cross(forward, right);

    mat4 result = mat4_identity();

    result.m[0][0] = right.x;
    result.m[0][1] = right.y;
    result.m[0][2] = right.z;

    result.m[1][0] = cameraUp.x;
    result.m[1][1] = cameraUp.y;
    result.m[1][2] = cameraUp.z;

    result.m[2][0] = forward.x;
    result.m[2][1] = forward.y;
    result.m[2][2] = forward.z;

    result.m[0][3] = -vec3_dot(right, eye);
    result.m[1][3] = -vec3_dot(cameraUp, eye);
    result.m[2][3] = -vec3_dot(forward, eye);

    return result;
}