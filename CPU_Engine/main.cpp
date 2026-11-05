#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdio>

#include "Display.hpp"
#include "Camera.hpp"
#include "ECS.h"
#include "AssetManager.hpp"
#include "Renderer.hpp"

#define WIDTH 800
#define HEIGHT 600

#ifndef WARP_ASSET_DIR
#define WARP_ASSET_DIR "assets"
#endif

constexpr bool Backface_culling = true;

struct Position
{
    float x;
    float y;
    float z;
};

struct Velocity
{
    float x;
    float y;
    float z;
};

struct Health
{
    int value;
};

int main()
{
    Position position;
    position.x = 1;
    position.y = 2;

    ComponentStorage<Position> componentstorage;
    componentstorage.Add(0, &position);

    if (componentstorage.Has(0))
    {
        Position* position2 = (Position*)componentstorage.Get(0);
        std::cout << "Cool" << "," << position2->x << std::endl;
    }

    AssetManager assets;

    if (!assets.loadMesh("spot", WARP_ASSET_DIR "/spot.obj"))
        return 1;

    if (!assets.loadTexture("spot_texture", WARP_ASSET_DIR "/spot_texture.png"))
        return 1;

    Mesh* mesh = assets.getMesh("spot");
    Texture* tex = assets.getTexture("spot_texture");

    if (mesh == nullptr || tex == nullptr)
        return 1;

    Framebuffer framebuffer(WIDTH, HEIGHT);

    Camera camera(WIDTH, HEIGHT);

    camera.setPosition(vec3_make(0, 0, 5));
    camera.setTarget(vec3_make(0, 0, 0));

    mat4 projection = camera.getProjectionMatrix();
    mat4 view = camera.getViewMatrix();

    std::vector<ScreenVertex> screenVertices(mesh->verts.size());

    const Uint64 freq = SDL_GetPerformanceFrequency();
    const Uint64 start = SDL_GetPerformanceCounter();

    Uint64 last = start;

    double fps_accum = 0.0;
    int fps_count = 0;

    char title[160];

    int drawn = 0;
    int culled = 0;

    Display display(WIDTH, HEIGHT);

    if (!display.isOpen())
    {
        std::cerr << "Impossible d'initialiser l'affichage.\n";
        return 1;
    }

    while (display.isOpen())
    {
        display.processEvents();

        double seconds =
            static_cast<double>(SDL_GetPerformanceCounter() - start)
            / static_cast<double>(freq);

        float t = static_cast<float>(seconds);

        mat4 model = mat4_rotation_y(t * 0.7f);

        mat4 mvp = mat4_mul(
            projection,
            mat4_mul(view, model));

        transformVertices(
            mesh->verts,
            mvp,
            screenVertices,
            WIDTH,
            HEIGHT);

        framebuffer.clear(framebuffer.rgb(16, 16, 24));
        framebuffer.clearZBuffer();

        RenderStats stats = renderMesh(
            framebuffer,
            *mesh,
            tex,
            screenVertices,
            Backface_culling);

        drawn = stats.drawn;
        culled = stats.culled;

        display.present(framebuffer);

        Uint64 now = SDL_GetPerformanceCounter();

        double dt =
            static_cast<double>(now - last) / static_cast<double>(freq);

        last = now;

        fps_accum += dt;
        fps_count++;

        if (fps_accum >= 1.0)
        {
            double fps = static_cast<double>(fps_count) / fps_accum;

            double ms =
                1000.0 * fps_accum / static_cast<double>(fps_count);

            snprintf(
                title,
                sizeof title,
                "%.1f FPS | %.2f ms/frame | drew %d | culled %d",
                fps, ms, drawn, culled);

            SDL_SetWindowTitle(display.window, title);

            fps_accum = 0.0;
            fps_count = 0;
        }
    }

    SDL_DestroyTexture(display.texture);
    SDL_DestroyRenderer(display.renderer);
    SDL_DestroyWindow(display.window);
    SDL_Quit();

    return 0;
}