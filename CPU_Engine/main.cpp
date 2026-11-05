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
#include "clip.h"

#define WIDTH  800
#define HEIGHT 600
#define PI 3.14159265358979323846f
#ifndef WARP_ASSET_DIR
#define WARP_ASSET_DIR "assets"
#endif

constexpr bool Backface_culling = true;

static vec3 transform_normal(const mat4& model, vec3 normal)
{
    const vec4 n = {
        normal.x,
        normal.y,
        normal.z,
        0.0f
    };

    const vec4 transformed = mat4_mul_vec4(model, n);

    return vec3_normalize(
        vec3_make(
            transformed.x,
            transformed.y,
            transformed.z
        )
    );
}

static vec2 get_face_uv(const Mesh& mesh, int face_corner)
{
    const int index = mesh.face_uvs[face_corner];

    if (index >= 0 && index < mesh.uv_count)
        return mesh.uvs[index];

    return vec2_make(0.0f, 0.0f);
}

static vec3 get_face_normal(const Mesh& mesh, int face_corner)
{
    const int index = mesh.face_normals[face_corner];

    if (index >= 0 && index < mesh.normal_count)
        return mesh.normals[index];

    return vec3_make(0.0f, 0.0f, 0.0f);
}

int main() {

    Mesh mesh_vache;
    if (!mesh_load(WARP_ASSET_DIR "/spot.obj", &mesh_vache)) return 1;
    printf("loaded spot.obj: %d vertices, %d faces\n",
        mesh_vache.vert_count, mesh_vache.face_count);

    Texture tex_vache;
    if (!texture_load(WARP_ASSET_DIR "/spot_texture.png", &tex_vache)) return 1;
    printf("loaded spot_texture.png: %dx%d texels\n", tex_vache.width, tex_vache.height);

    Framebuffer framebuffer(WIDTH, HEIGHT);
    constexpr std::uint32_t BLACK = 0xFF000000;

    uint32_t axis_color[3];
    axis_color[0] = framebuffer.rgb(230, 80, 80);
    axis_color[1] = framebuffer.rgb(80, 210, 90);
    axis_color[2] = framebuffer.rgb(90, 140, 240);

    const float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

    const mat4 projection = mat4_perspective(radians(60.0f), aspect, 0.1f, 1000.0f);

    const vec3 eye = vec3_make(4.0f, 0.6f, 2.4f);
    const vec3 target = vec3_make(0.0f, 0.0f, 0.0f);
    const vec3 up = vec3_make(0.0f, 1.0f, 0.0f);

    const mat4 view = mat4_look_at(eye, target, up);

    //bool* visible dégage, il est gerer par le clipping maintenant
    vec4* clip_positions = (vec4*)malloc(mesh_vache.vert_count * sizeof(vec4));

    const Uint64 freq = SDL_GetPerformanceFrequency();
    const Uint64 start = SDL_GetPerformanceCounter();
    Uint64 last = start;
    double fps_accum = 0.0;
    int    fps_count = 0;
    char   title[160];
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

        double seconds = static_cast<double>(SDL_GetPerformanceCounter() - start) / static_cast<double>(freq);
        float t = static_cast<float>(seconds);

        mat4 animation = mat4_rotation_y(t * 0.7f);
        mat4 rotation = mat4_rotation_x(radians(0.0f));
        mat4 model = mat4_mul(animation, rotation);
        mat4 mvp = mat4_mul(projection, mat4_mul(view, model));

        for (int i = 0; i < mesh_vache.vert_count; i++)
        {
            clip_positions[i] = mat4_mul_vec4(mvp,vec4_from_point(mesh_vache.verts[i]));
        }

        framebuffer.clear(framebuffer.rgb(16, 16, 24));
        framebuffer.clearZbuffer();

        for (int fidx = 0; fidx < mesh_vache.face_count; fidx++)
        {
            int a = mesh_vache.faces[fidx * 3 + 0];
            int b = mesh_vache.faces[fidx * 3 + 1];
            int c = mesh_vache.faces[fidx * 3 + 2];

            int na = mesh_vache.face_normals[fidx * 3 + 0];
            int nb = mesh_vache.face_normals[fidx * 3 + 1];
            int nc = mesh_vache.face_normals[fidx * 3 + 2];

            vec3 normalA = (na >= 0) ? mesh_vache.normals[na] : vec3_make(0.0f, 0.0f, 1.0f);

            vec3 normalB = (nb >= 0) ? mesh_vache.normals[nb] : vec3_make(0.0f, 0.0f, 1.0f);

            vec3 normalC = (nc >= 0) ? mesh_vache.normals[nc] : vec3_make(0.0f, 0.0f, 1.0f);

            ClipVertex input[3];

            input[0].clip = clip_positions[a];
            input[1].clip = clip_positions[b];
            input[2].clip = clip_positions[c];

            input[0].uv = vec2_make(0, 0);
            input[1].uv = vec2_make(0, 0);
            input[2].uv = vec2_make(0, 0);

            ClipVertex output[4];

            int count = clipTriangleNear(input, output);

            if (count < 3)
                continue;

            for (int i = 1; i < count - 1; i++)
            {
                ClipVertex v0 = output[0];
                ClipVertex v1 = output[i];
                ClipVertex v2 = output[i + 1];

                if (v0.clip.w <= 0.0001f || v1.clip.w <= 0.0001f || v2.clip.w <= 0.0001f) continue;

                float iw0 = 1.0f / v0.clip.w;
                float iw1 = 1.0f / v1.clip.w;
                float iw2 = 1.0f / v2.clip.w;

                vec3 p0 = {(v0.clip.x * iw0 + 1.0f) * 0.5f * WIDTH, (1.0f - (v0.clip.y * iw0 + 1.0f) * 0.5f) * HEIGHT, v0.clip.z * iw0};

                vec3 p1 = {(v1.clip.x * iw1 + 1.0f) * 0.5f * WIDTH, (1.0f - (v1.clip.y * iw1 + 1.0f) * 0.5f) * HEIGHT, v1.clip.z * iw1};

                vec3 p2 = {(v2.clip.x * iw2 + 1.0f) * 0.5f * WIDTH, (1.0f - (v2.clip.y * iw2 + 1.0f) * 0.5f) * HEIGHT, v2.clip.z * iw2};

                int area = framebuffer.edge(p0, p1, p2);

                if (Backface_culling && area >= 0)
                {
                    culled++;
                    continue;
                }

                drawn++;

                uint32_t buildingColor = framebuffer.rgb(190, 195, 205);

                framebuffer.drawTriangleZ(p0, p1, p2, buildingColor);
            }
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
            drawn = 0;
            culled = 0;
    }

        //free(screen_x); free(screen_y); free(visible); free(screen_z); free(inv_w);
        mesh_free(&mesh_vache);
        SDL_DestroyTexture(display.texture);
        SDL_DestroyRenderer(display.renderer);
        SDL_DestroyWindow(display.window);
        SDL_Quit();

        return 0;
}
