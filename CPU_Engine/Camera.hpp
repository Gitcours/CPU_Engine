#pragma once

#include "Vec3.h"
#include "Mat4x4.h"

class Camera
{
public:
    Camera(
        const Vec3& position,
        const Vec3& target,
        const Vec3& up = Vec3(0.0f, 1.0f, 0.0f)
    );

    void setPosition(const Vec3& position);
    void setTarget(const Vec3& target);

    const Vec3& getPosition() const;
    const Vec3& getTarget() const;

    Mat4x4 getViewMatrix() const;

private:
    Vec3 position_;
    Vec3 target_;
    Vec3 up_;
};
