#include "Camera.hpp"

Camera::Camera(int width, int height)
{
    position = vec3_make(0, 0, 5);
    target = vec3_make(0, 0, 0);
    up = vec3_make(0, 1, 0);

    fov = radians(60.0f);
    aspectRatio = static_cast<float>(width) / height;
    nearPlane = 0.1f;
    farPlane = 100.0f;
}

void Camera::setPosition(vec3 position)
{
    this->position = position;
}

void Camera::setTarget(vec3 target)
{
    this->target = target;
}

void Camera::setUp(vec3 up)
{
    this->up = up;
}

void Camera::setFov(float fov)
{
    this->fov = radians(fov);
}

void Camera::setAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
}

void Camera::setNearPlane(float nearPlane)
{
    this->nearPlane = nearPlane;
}

void Camera::setFarPlane(float farPlane)
{
    this->farPlane = farPlane;
}

mat4 Camera::getViewMatrix() const
{
    return mat4_look_at(position, target, up);
}

mat4 Camera::getProjectionMatrix() const
{
    return mat4_perspective(
        fov,
        aspectRatio,
        nearPlane,
        farPlane);
}