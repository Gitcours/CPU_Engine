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
    Framebuffer framebuffer(WIDTH, HEIGHT);
    constexpr std::uint32_t BLACK = 0xFF000000;
    const float R = 200.0f;

    vec3 base[3];
    for (int i = 0; i < 3; i++) {
        float a = (float)i / 3.0f * 2.0f * PI - PI / 2.0f;
        base[i] = { R * cosf(a), R * sinf(a), 0.0f };

    }
    

    
    uint32_t axis_color[3];
    axis_color[0] = framebuffer.rgb(230, 80, 80); 
    axis_color[1] = framebuffer.rgb(80, 210, 90);   
    axis_color[2] = framebuffer.rgb(90, 140, 240);  

    const float aspect = (float)WIDTH / (float)HEIGHT;

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

        double seconds = (double)(SDL_GetPerformanceCounter() - start) / (double)freq;
        float t = (float)seconds;

        mat4 model = mat4_mul(mat4_rotation_y(t * 0.8f),
            mat4_rotation_x(t * 0.5f));
        mat4 mvp = mat4_mul(projection, mat4_mul(view, model));

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
