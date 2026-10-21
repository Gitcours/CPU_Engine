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
static const vec3 CUBE_VERTS[8] = {
    {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
    {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1},
};

static const int CUBE_EDGES[12][3] = {
    {0,1,0}, {3,2,0}, {7,6,0}, {4,5,0},   
    {0,3,1}, {1,2,1}, {5,6,1}, {4,7,1},   
    {0,4,2}, {1,5,2}, {2,6,2}, {3,7,2},   
};

int main()
{
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    constexpr std::uint32_t BLACK = 0xFF000000;
    const float R = 200.0f;

    vec3 base[3];
    for (int i = 0; i < 3; i++) {
        float a = (float)i / 3.0f * 2.0f * PI - PI / 2.0f;
        base[i] = { R * cosf(a), R * sinf(a), 0.0f };

    }
    

    Framebuffer framebuffer(WIDTH, HEIGHT);
    
    
    
    const uint32_t corner_color[3] = {
        framebuffer.rgb(255,  60,  60),
        framebuffer.rgb(60, 255,  60),   
        framebuffer.rgb(60,  60, 255),   
    };

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


        


        /*display.processEvents();
        double seconds = (double)(SDL_GetPerformanceCounter() - start) / (double)freq;
        float angle = (float)seconds;


        mat4 transform = mat4_mul(
            mat4_translation(WIDTH / 2.0f, HEIGHT / 2.0f, 0.0f),
            mat4_rotation_z(angle));

        vec3 p[3];
        for (int i = 0; i < 3; i++) p[i] = mat4_apply_point(transform, base[i]);

        framebuffer.clear(BLACK);

        framebuffer.drawTriangleGradient({ p[0].x,p[0].y }, { p[1].x, p[1].y }, { p[2].x, p[2].y });

        framebuffer.drawCircle(WIDTH / 2, HEIGHT / 2, (int)R, framebuffer.rgb(60, 60, 80));

        framebuffer.drawLine((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y, framebuffer.rgb(255, 255, 255));
        framebuffer.drawLine((int)p[1].x, (int)p[1].y, (int)p[2].x, (int)p[2].y, framebuffer.rgb(255, 255, 255));
        framebuffer.drawLine((int)p[2].x, (int)p[2].y, (int)p[0].x, (int)p[0].y, framebuffer.rgb(255, 255, 255));*/

        display.present(framebuffer);

        Uint64 now = SDL_GetPerformanceCounter();
        double dt = (double)(now - last) / (double)freq;
        last = now;
        fps_accum += dt;
        fps_count += 1;

        if (fps_accum >= 1.0) {
            double fps = (double)fps_count / fps_accum;
            double ms = 1000.0 * fps_accum / (double)fps_count;
            snprintf(title, sizeof title,
                "Chapter 04 — Vectors & matrices  |  %.1f FPS  (%.2f ms/frame)",
                fps, ms);
            SDL_SetWindowTitle(display.window, title);
            fps_accum = 0.0;
            fps_count = 0;
        }
    }

    return 0;
}
