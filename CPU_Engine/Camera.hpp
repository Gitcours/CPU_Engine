#pragma once

#include "vec.h"
#include "mat.h"

class Camera
{
private:
    vec3 position;
    vec3 target;
    vec3 up;

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

public:
    Camera(int width, int height);

    void setPosition(vec3 position);
    void setTarget(vec3 target);
    void setUp(vec3 up);

    void setFov(float fov);
    void setAspectRatio(float aspectRatio);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix() const;
};