#pragma once

#include <cmath>

#include "Vec3.h"

class alignas(16) Mat4x4
{
public:
    float m[4][4];

    Mat4x4()
        : m{}
    {
    }

    // ================================================
    // Matrice identite
    // ================================================

    static Mat4x4 Identity()
    {
        Mat4x4 result;

        result.m[0][0] = 1.0f;
        result.m[1][1] = 1.0f;
        result.m[2][2] = 1.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    // ================================================
    // Translation
    // ================================================

    static Mat4x4 Translation(float x, float y, float z)
    {
        Mat4x4 result = Identity();

        result.m[0][3] = x;
        result.m[1][3] = y;
        result.m[2][3] = z;

        return result;
    }

    // ================================================
    // Rotation X
    // ================================================

    static Mat4x4 RotationX(float angle)
    {
        Mat4x4 result = Identity();

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        result.m[1][1] = c;
        result.m[1][2] = -s;
        result.m[2][1] = s;
        result.m[2][2] = c;

        return result;
    }

    // ================================================
    // Rotation Y
    // ================================================

    static Mat4x4 RotationY(float angle)
    {
        Mat4x4 result = Identity();

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        result.m[0][0] = c;
        result.m[0][2] = s;
        result.m[2][0] = -s;
        result.m[2][2] = c;

        return result;
    }

    // ================================================
    // Rotation Z
    // ================================================

    static Mat4x4 RotationZ(float angle)
    {
        Mat4x4 result = Identity();

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;

        return result;
    }

    // ================================================
    // Matrice de vue : LookAt
    // Camera orientee vers +Z
    // ================================================

    static Mat4x4 LookAt(
        const Vec3& eye,
        const Vec3& target,
        const Vec3& up
    )
    {
        const Vec3 forward = (target - eye).normalized();
        const Vec3 right = up.cross(forward).normalized();
        const Vec3 cameraUp = forward.cross(right).normalized();

        Mat4x4 result = Identity();

        result.m[0][0] = right.x();
        result.m[0][1] = right.y();
        result.m[0][2] = right.z();
        result.m[0][3] = -right.dot(eye);

        result.m[1][0] = cameraUp.x();
        result.m[1][1] = cameraUp.y();
        result.m[1][2] = cameraUp.z();
        result.m[1][3] = -cameraUp.dot(eye);

        result.m[2][0] = forward.x();
        result.m[2][1] = forward.y();
        result.m[2][2] = forward.z();
        result.m[2][3] = -forward.dot(eye);

        return result;
    }

    // ================================================
    // Multiplication de matrices
    // ================================================

    Mat4x4 operator*(const Mat4x4& other) const
    {
        Mat4x4 result;

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                for (int k = 0; k < 4; ++k)
                {
                    result.m[row][col] +=
                        m[row][k] * other.m[k][col];
                }
            }
        }

        return result;
    }

    // ================================================
    // Transformation d'un point
    // La translation est appliquee
    // ================================================

    Vec3 TransformPoint(const Vec3& point) const
    {
        const float x =
            m[0][0] * point.x()
            + m[0][1] * point.y()
            + m[0][2] * point.z()
            + m[0][3];

        const float y =
            m[1][0] * point.x()
            + m[1][1] * point.y()
            + m[1][2] * point.z()
            + m[1][3];

        const float z =
            m[2][0] * point.x()
            + m[2][1] * point.y()
            + m[2][2] * point.z()
            + m[2][3];

        return Vec3(x, y, z);
    }

    // ================================================
    // Transformation d'une direction
    // La translation est ignoree
    // ================================================

    Vec3 TransformDirection(const Vec3& direction) const
    {
        const float x =
            m[0][0] * direction.x()
            + m[0][1] * direction.y()
            + m[0][2] * direction.z();

        const float y =
            m[1][0] * direction.x()
            + m[1][1] * direction.y()
            + m[1][2] * direction.z();

        const float z =
            m[2][0] * direction.x()
            + m[2][1] * direction.y()
            + m[2][2] * direction.z();

        return Vec3(x, y, z);
    }
};
