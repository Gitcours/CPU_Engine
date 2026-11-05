#include "Renderer.hpp"

void transformVertices(
    const std::vector<vec3>& vertices,
    const mat4& mvp,
    std::vector<ScreenVertex>& screenVertices,
    int width,
    int height)
{
    for (int i = 0; i < vertices.size(); i++)
    {
        vec4 clip = mat4_mul_vec4(
            mvp,
            vec4_from_point(vertices[i]));

        if (clip.w <= 0.0001f)
        {
            screenVertices[i].visible = false;
            continue;
        }

        float inv = 1.0f / clip.w;

        screenVertices[i].x = static_cast<int>(
            (clip.x * inv + 1.0f) * 0.5f * width);

        screenVertices[i].y = static_cast<int>(
            (1.0f - (clip.y * inv + 1.0f) * 0.5f) * height);

        screenVertices[i].z = clip.z * inv;
        screenVertices[i].invW = inv;
        screenVertices[i].visible = true;
    }
}

RenderStats renderMesh(
    Framebuffer& framebuffer,
    const Mesh& mesh,
    const Texture* texture,
    const std::vector<ScreenVertex>& screenVertices,
    bool backfaceCulling)
{
    RenderStats stats = { 0, 0 };

    for (int fidx = 0; fidx < mesh.faces.size() / 3; fidx++)
    {
        int a = mesh.faces[fidx * 3 + 0];
        int b = mesh.faces[fidx * 3 + 1];
        int c = mesh.faces[fidx * 3 + 2];

        if (!screenVertices[a].visible ||
            !screenVertices[b].visible ||
            !screenVertices[c].visible)
            continue;

        int area = framebuffer.edge(
            { (float)screenVertices[a].x, (float)screenVertices[a].y },
            { (float)screenVertices[b].x, (float)screenVertices[b].y },
            { (float)screenVertices[c].x, (float)screenVertices[c].y });

        if (backfaceCulling && area >= 0)
        {
            stats.culled++;
            continue;
        }

        stats.drawn++;

        int ua = mesh.face_uvs[fidx * 3 + 0];
        int ub = mesh.face_uvs[fidx * 3 + 1];
        int uc = mesh.face_uvs[fidx * 3 + 2];

        vec2 uva = (ua >= 0) ? mesh.uvs[ua] : vec2_make(0, 0);
        vec2 uvb = (ub >= 0) ? mesh.uvs[ub] : vec2_make(0, 0);
        vec2 uvc = (uc >= 0) ? mesh.uvs[uc] : vec2_make(0, 0);

        framebuffer.drawTriangleTextured(
            screenVertices[a].x,
            screenVertices[a].y,
            screenVertices[a].z,
            screenVertices[a].invW,
            uva,

            screenVertices[b].x,
            screenVertices[b].y,
            screenVertices[b].z,
            screenVertices[b].invW,
            uvb,

            screenVertices[c].x,
            screenVertices[c].y,
            screenVertices[c].z,
            screenVertices[c].invW,
            uvc,

            texture);
    }

    return stats;
}