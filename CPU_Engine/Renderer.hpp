#pragma once

#include <vector>

#include "vec.h"
#include "mat.h"
#include "Framebuffer.hpp"
#include "ScreenVertex.hpp"
#include "obj.h"

struct RenderStats
{
    int drawn;
    int culled;
};

void transformVertices(
    const std::vector<vec3>& vertices,
    const mat4& mvp,
    std::vector<ScreenVertex>& screenVertices,
    int width,
    int height);

RenderStats renderMesh(
    Framebuffer& framebuffer,
    const Mesh& mesh,
    const Texture* texture,
    const std::vector<ScreenVertex>& screenVertices,
    bool backfaceCulling);