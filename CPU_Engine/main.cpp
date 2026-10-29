#include <cstdint>
#include <iostream>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_ONLY_PNG

#include "obj.h"

#include "Display.hpp"
#include "mat.h"

#define WIDTH  800
#define HEIGHT 600
#define PI 3.14159265358979323846f
#ifndef WARP_ASSET_DIR
#define WARP_ASSET_DIR "assets"
#endif

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

int main() {

    Mesh mesh;
    if (!mesh_load(WARP_ASSET_DIR "/spot.obj", &mesh)) return 1;
    printf("loaded spot.obj: %d vertices, %d faces\n",
        mesh.vert_count, mesh.face_count);

    Texture tex;
    if (!texture_load(WARP_ASSET_DIR "/spot_texture.png", &tex)) return 1;
    printf("loaded spot_texture.png: %dx%d texels\n", tex.width, tex.height);
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

    int* screen_x = (int*)malloc(mesh.vert_count * sizeof(int));
    int* screen_y = (int*)malloc(mesh.vert_count * sizeof(int));
    float* screen_z = (float*)malloc(mesh.vert_count * sizeof(float));
    float* inv_w = (float*)malloc(mesh.vert_count * sizeof(float));
    bool* visible = (bool*)malloc(mesh.vert_count * sizeof(bool));



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

        int drawn = 0;
        int culled = 0;

        mat4 model = mat4_rotation_y(t * 0.7f);
        mat4 mvp = mat4_mul(projection, mat4_mul(view, model));

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
        
        for (int i = 0; i < mesh.vert_count; i++) {
            vec4 clip = mat4_mul_vec4(mvp, vec4_from_point(mesh.verts[i]));

            if (clip.w <= 0.0001f) { visible[i] = false; continue; }
            float inv = 1.0f / clip.w;
            screen_x[i] = (int)((clip.x * inv + 1.0f) * 0.5f * WIDTH);
            screen_y[i] = (int)((1.0f - (clip.y * inv + 1.0f) * 0.5f) * HEIGHT);
            screen_z[i] = clip.z * inv;
            inv_w[i] = inv;
            visible[i] = true;
        }

        framebuffer.clear(framebuffer.rgb(16, 16, 24));
        framebuffer.clearZbuffer();

        for (int fidx = 0; fidx < mesh.face_count; fidx++) {
            int a = mesh.faces[fidx * 3 + 0];
            int b = mesh.faces[fidx * 3 + 1];
            int c = mesh.faces[fidx * 3 + 2];
            if (!visible[a] || !visible[b] || !visible[c]) continue;

            // Backface culling (chapter 8): front-facing = negative area.
            int area = framebuffer.edge({ (float)screen_x[a], (float)screen_y[a] }, { (float)screen_x[b], (float)screen_y[b] }, { (float)screen_x[c], (float)screen_y[c] });
            if (Backface_culling && area >= 0) { culled++;  continue; }

            drawn++;

            // Per-corner texture coordinates (fall back to (0,0) if a
            // face somehow lacked a UV index).
            int ua = mesh.face_uvs[fidx * 3 + 0];
            int ub = mesh.face_uvs[fidx * 3 + 1];
            int uc = mesh.face_uvs[fidx * 3 + 2];
            vec2 uva = (ua >= 0) ? mesh.uvs[ua] : vec2_make(0, 0);
            vec2 uvb = (ub >= 0) ? mesh.uvs[ub] : vec2_make(0, 0);
            vec2 uvc = (uc >= 0) ? mesh.uvs[uc] : vec2_make(0, 0);

            framebuffer.draw_triangle_textured(screen_x[a], screen_y[a], screen_z[a], inv_w[a], uva,
                screen_x[b], screen_y[b], screen_z[b], inv_w[b], uvb,
                screen_x[c], screen_y[c], screen_z[c], inv_w[c], uvc,
                &tex);
        }
            uint32_t cubeColor =framebuffer.rgb(100, 180, 240);

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

        free(screen_x); free(screen_y);; free(visible);
        mesh_free(&mesh);
        SDL_DestroyTexture(display.texture);
        SDL_DestroyRenderer(display.renderer);
        SDL_DestroyWindow(display.window);
        SDL_Quit();

        return 0;
}
