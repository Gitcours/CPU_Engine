
#pragma once

#include <vector>
#include "Vec3.h"

struct Triangle
{
    int a;
    int b;
    int c;
};

class Mesh
{
public:
    std::vector<Vec3> vertices;
    std::vector<Triangle> triangles;

    static Mesh CreateCube(float size = 2.0f);
};
