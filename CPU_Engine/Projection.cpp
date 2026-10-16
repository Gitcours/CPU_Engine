#include "Projection.hpp"

#include <cmath>

bool Projection::project(
    const Vec3& point,
    int screenWidth,
    int screenHeight,
    float fovDegrees,
    float nearPlane,
    float farPlane,
    int& screenX,
    int& screenY
)
{
    // Validation des parametres.
    if (screenWidth <= 0 || screenHeight <= 0)
        return false;

    if (fovDegrees <= 0.0f || fovDegrees >= 180.0f)
        return false;

    if (nearPlane <= 0.0f || farPlane <= nearPlane)
        return false;

    // Notre camera regarde vers +Z.
    const float z = point.z();

    if (z < nearPlane || z > farPlane)
        return false;

    constexpr float PI = 3.14159265358979323846f;

    const float fovRadians = fovDegrees * PI / 180.0f;

    // Distance focale exprimee en pixels.
    const float focalLength =
        (static_cast<float>(screenHeight) * 0.5f)
        / std::tan(fovRadians * 0.5f);

    // Projection perspective.
    const float projectedX = point.x() * focalLength / z;
    const float projectedY = point.y() * focalLength / z;

    // Centre du framebuffer.
    const float centerX = static_cast<float>(screenWidth) * 0.5f;
    const float centerY = static_cast<float>(screenHeight) * 0.5f;

    // Conversion en pixels. L'axe Y est inverse sur l'ecran.
    screenX = static_cast<int>(
        std::lround(centerX + projectedX)
        );

    screenY = static_cast<int>(
        std::lround(centerY - projectedY)
        );

    return true;
}
