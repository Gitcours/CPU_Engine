#include <cstdint>
#include <iostream>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Framebuffer.hpp"
#include "Display.hpp"
#include "vec.h"
#include "mat.h"

#define PI 3.14159265358979323846f

constexpr bool Backface_culling = true;

static const vec3 CUBE_VERTS[8] = {
    {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
    {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1},
};

static const int CUBE_EDGES[12][3] = {
    {0,1,0}, {3,2,0}, {7,6,0}, {4,5,0},
    {0,3,1}, {1,2,1}, {5,6,1}, {4,7,1},
    {0,4,2}, {1,5,2}, {2,6,2}, {3,7,2},
};

static const int CUBE_TRIANGLES[12][4] = {
    {0, 1, 2, 2}, {0, 2, 3, 2}, {4, 6, 5, 2},
    {4, 7, 6, 2}, {0, 3, 7, 0}, {0, 7, 4, 0},
    {1, 5, 6, 0}, {1, 6, 2, 0}, {0, 4, 5, 1},
    {0, 5, 1, 1}, {3, 2, 6, 1}, {3, 6, 7, 1}
};

int main()
{
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    Framebuffer framebuffer(WIDTH, HEIGHT);
    constexpr std::uint32_t BLACK = 0xFF000000;
    
    uint32_t axis_color[3];
    axis_color[0] = framebuffer.rgb(230, 80, 80); 
    axis_color[1] = framebuffer.rgb(80, 210, 90);   
    axis_color[2] = framebuffer.rgb(90, 140, 240);  

    const float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

    const mat4 projection = mat4_perspective(radians(60.0f), aspect,
        0.1f, 100.0f);

    const vec3 eye = vec3_make(0, 0, 5);
    const vec3 target = vec3_make(0, 0, 0);
    const vec3 up = vec3_make(0, 1, 0);
    const mat4 view = mat4_look_at(eye, target, up);


    const Uint64 freq = SDL_GetPerformanceFrequency();
    const Uint64 start = SDL_GetPerformanceCounter();
    Uint64 last = start;
    double fps_accum = 0.0;
    int    fps_count = 0;
    char   title[160];

    Display display(WIDTH, HEIGHT);
   
    if (!display.isOpen())
    {
        std::cerr << "Impossible d'initialiser l'affichage.\n";
        return 1;
    }

    while (display.isOpen())
    {
        display.processEvents();

        double seconds = static_cast<double>(SDL_GetPerformanceCounter() - start) / static_cast<double>(freq);
        float t = static_cast<float>(seconds);

        mat4 model = mat4_mul(mat4_rotation_y(t * 0.8f),
            mat4_rotation_x(t * 0.5f));
        mat4 mvp = mat4_mul(projection, mat4_mul(view, model));

        int  screen_x[8], screen_y[8];

        float screen_z[8];

        bool visible[8];

        for (int i = 0; i < 8; i++) {
            vec4 clip = mat4_mul_vec4(mvp, vec4_from_point(CUBE_VERTS[i]));

           
            if (clip.w <= 0.0001f) { visible[i] = false; continue; }

            
            float ndc_x = clip.x / clip.w;
            float ndc_y = clip.y / clip.w;
            float ndc_z = clip.z / clip.w;


            screen_x[i] = static_cast<int>((ndc_x + 1.0f) * 0.5f * WIDTH);
            screen_y[i] = static_cast<int>((1.0f - (ndc_y + 1.0f) * 0.5f) * HEIGHT);
            screen_z[i] = ndc_z;
            visible[i] = true;
        }


        framebuffer.clear(framebuffer.rgb(16, 16, 24));
        framebuffer.clearZbuffer();

        uint32_t cubeColor =framebuffer.rgb(100, 180, 240);

        int drawn = 0;
        int culled = 0;

        for (int i = 0; i < 12; i++) {
            int v0 = CUBE_TRIANGLES[i][0];
            int v1 = CUBE_TRIANGLES[i][1];
            int v2 = CUBE_TRIANGLES[i][2];

            int axe = CUBE_TRIANGLES[i][3];

            if (!visible[v0] || !visible[v1] || !visible[v2]) continue;
            vec3 p0 = {static_cast<float>(screen_x[v0]), static_cast<float>(screen_y[v0]), screen_z[v0]};

            vec3 p1 = {static_cast<float>(screen_x[v1]), static_cast<float>(screen_y[v1]), screen_z[v1]};

            vec3 p2 = {static_cast<float>(screen_x[v2]), static_cast<float>(screen_y[v2]), screen_z[v2]};

            float area = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);

            if (Backface_culling && area <= 0.0f) {culled++; continue;}

            drawn++;

            framebuffer.drawTriangleZ(p0, p1, p2, axis_color[axe]);
        }

        display.present(framebuffer);

        Uint64 now = SDL_GetPerformanceCounter();
        double dt = static_cast<double>(now - last) / static_cast<double>(freq);
        last = now;
        fps_accum += dt;
        fps_count += 1;

        if (fps_accum >= 1.0) {
            double fps = static_cast<double>(fps_count) / fps_accum;
            double ms = 1000.0 * fps_accum / static_cast<double>(fps_count);
            snprintf(title, sizeof title, "%.1f FPS | %.2f ms/frame | drew %d | culled %d", fps, ms, drawn, culled);
            SDL_SetWindowTitle(display.window, title);
            fps_accum = 0.0;
            fps_count = 0;
        }
    }
    return 0;
}
