#include "Camera.hpp"

Camera::Camera(
    const Vec3& position,
    const Vec3& target,
    const Vec3& up
)
    : position_(position),
    target_(target),
    up_(up)
{
}

void Camera::setPosition(const Vec3& position)
{
    position_ = position;
}

void Camera::setTarget(const Vec3& target)
{
    target_ = target;
}

const Vec3& Camera::getPosition() const
{
    return position_;
}

const Vec3& Camera::getTarget() const
{
    return target_;
}

Mat4x4 Camera::getViewMatrix() const
{
    return Mat4x4::LookAt(position_, target_, up_);
}
