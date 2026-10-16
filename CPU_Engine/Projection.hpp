#pragma once

#include "Vec3.h"

class Projection
{
public:
    static bool project(
        const Vec3& point,
        int screenWidth,
        int screenHeight,
        float fovDegrees,
        float nearPlane,
        float farPlane,
        int& screenX,
        int& screenY
    );
};
