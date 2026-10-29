#include "Framebuffer.hpp"

#include <cstdlib>
#include <algorithm>
#include <cstdint>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH  800
#define HEIGHT 600

constexpr float maxF{ std::numeric_limits<float>::max() };

Framebuffer::Framebuffer(int width, int height) : 
    width(width),
    height(height),
    pixels(width* height, 0xFF000000), 
    zbuffer(width* height, maxF)
{}


static inline int clampi(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

void Framebuffer::clear(std::uint32_t color)
{
    for (auto& pixel : pixels)
    {
        pixel = color;
    }
}

void Framebuffer::clearZbuffer()
{
    std::fill(zbuffer.begin(), zbuffer.end(), maxF);
}

void Framebuffer::putPixel(int x, int y, std::uint32_t color)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    const int index = y * width + x;
    pixels[index] = color;
}

void Framebuffer::putPixelDepth(int x, int y, float z, std::uint32_t color)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int index = y * width + x;

    if (z < zbuffer[index])
    {
        zbuffer[index] = z;
        pixels[index] = color;
    }
}

void Framebuffer::drawLine(int x0, int y0, int x1, int y1, std::uint32_t color)
{
    const int dx = std::abs(x1 - x0);
    const int dy = std::abs(y1 - y0);

    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;

    int error = dx - dy;

    while (true)
    {
        putPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        const int error2 = 2 * error;

        if (error2 > -dy)
        {
            error -= dy;
            x0 += sx;
        }

        if (error2 < dx)
        {
            error += dx;
            y0 += sy;
        }
    }
}


void Framebuffer::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, std::uint32_t color)
{
    const auto edge = [](
        int ax, int ay,
        int bx, int by,
        int px, int py) -> long long
        {
            return static_cast<long long>(bx - ax) * (py - ay)
                - static_cast<long long>(by - ay) * (px - ax);
        };

    const long long area =
        edge(x0, y0, x1, y1, x2, y2);

    if (area == 0)
        return;

    const int minX = std::max(
        0, std::min({ x0, x1, x2 }));

    const int maxX = std::min(
        width - 1, std::max({ x0, x1, x2 }));

    const int minY = std::max(
        0, std::min({ y0, y1, y2 }));

    const int maxY = std::min(
        height - 1, std::max({ y0, y1, y2 }));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            const long long e0 =
                edge(x0, y0, x1, y1, x, y);

            const long long e1 =
                edge(x1, y1, x2, y2, x, y);

            const long long e2 =
                edge(x2, y2, x0, y0, x, y);

            const bool inside =
                (area > 0)
                ? (e0 >= 0 && e1 >= 0 && e2 >= 0)
                : (e0 <= 0 && e1 <= 0 && e2 <= 0);

            if (inside)
                putPixel(x, y, color);
        }
    }


}

void Framebuffer::drawTriangleZ(vec3 p0, vec3 p1, vec3 p2, std::uint32_t color)
{
    float area = static_cast<float>(edge(p0, p1, p2)); //une autre facon de faire un cast, static_cast<float>(edge(p0, p1, p2)) aurait donner le meme résutlat mais finalement c'est plus jolie et moins long de juste faire (float)edge

    if (area == 0.0f) return;

    if (area < 0.0f)
    {
        std::swap(p1, p2);
        area = -area;
    }

    int minX = std::max(0, (int)std::min({ p0.x, p1.x, p2.x }));

    int maxX = std::min(width - 1, (int)std::max({ p0.x, p1.x, p2.x }));

    int minY = std::max(0, (int)std::min({ p0.y, p1.y, p2.y }));

    int maxY = std::min(height - 1, (int)std::max({ p0.y, p1.y, p2.y }));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            vec3 p = {
                (float)x,
                (float)y,
                0.0f
            };

            float l0 = (float)edge(p1, p2, p) / area;

            float l1 = (float)edge(p2, p0, p) / area;

            float l2 = (float)edge(p0, p1, p) / area;

            if (l0 >= 0.0f && l1 >= 0.0f && l2 >= 0.0f)
            {
                float z = l0 * p0.z + l1 * p1.z + l2 * p2.z;

                putPixelDepth(x, y, z, color);
            }
        }
    }
}

uint32_t Framebuffer::rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)0xFFu << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}
float Framebuffer::edge(vec3 p0, vec3 p1, vec3 p2){
    return ((p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x));
};

void Framebuffer::drawTriangleGradient(vec3 p0, vec3 p1, vec3 p2)
{
    std::uint32_t RED = 0xFFFF0000;
    std::uint32_t GREEN = 0xFF00FF00;
    int area = edge(p0, p1, p2);

    if (area == 0) return;

    int minX = p0.x < p1.x ? (p0.x < p2.x ? p0.x : p2.x) : (p1.x < p2.x ? p1.x : p2.x);
    int minY = p0.y < p1.y ? (p0.y < p2.y ? p0.y : p2.y) : (p1.y < p2.y ? p1.y : p2.y);
    int maxX = p0.x > p1.x ? (p0.x > p2.x ? p0.x : p2.x) : (p1.x > p2.x ? p1.x : p2.x);
    int maxY = p0.y > p1.y ? (p0.y > p2.y ? p0.y : p2.y) : (p1.y > p2.y ? p1.y : p2.y);


    for (float y = minY; y < maxY; y++) {
        for (float x = minX; x < maxX; x++) {
            float e0 = (float)edge(p1, p2, { x,y }) / (float)area;
            float e1 = (float)edge(p2, p0, { x,y }) / (float)area;
            float e2 = (float)edge(p0, p1, { x,y }) / (float)area;

            if (e0 >= 0 && e1 >= 0 && e2 >= 0 ) {
                uint8_t r = e0*255;
                uint8_t g = e1*255;
                uint8_t b = e2*255;
                uint32_t color = rgb(r, g, b);
                putPixel(x, y, color);
            }
        }
    }
}

void Framebuffer::drawCircle(int cx, int cy, int r, uint32_t color) {

        int x = 0;
        int y = r;
        int d = 3 - 2 * r;
        while (x <= y) {
            putPixel(cx + x, cy + y, color);
            putPixel(cx - x, cy + y, color);
            putPixel(cx + x, cy - y, color);
            putPixel(cx - x, cy - y, color);
            putPixel(cx + y, cy + x, color);
            putPixel(cx - y, cy + x, color);
            putPixel(cx + y, cy - x, color);
            putPixel(cx - y, cy - x, color);
            x++;
            if (d > 0) {
                y--;
                d += 4 * (x - y) + 10;
            }
            else {
                d += 4 * x + 6;
            }
        }
}

void Framebuffer::draw_triangle_textured(int x0, int y0, float z0, float iw0, vec2 uv0,
    int x1, int y1, float z1, float iw1, vec2 uv1,
    int x2, int y2, float z2, float iw2, vec2 uv2,
    const Texture* tex) {
    int area = edge({ (float)x0, (float)y0 }, { (float)x1, (float)y1 }, { (float)x2, (float)y2 });
    if (area == 0) return;

    // Normalise winding to positive, carrying ALL per-vertex
    // attributes (depth, 1/w, uv) along with the swapped vertex.
    if (area < 0) {
        int tx = x1, ty = y1; float tz = z1, tiw = iw1; vec2 tuv = uv1;
        x1 = x2; y1 = y2; z1 = z2; iw1 = iw2; uv1 = uv2;
        x2 = tx; y2 = ty; z2 = tz; iw2 = tiw; uv2 = tuv;
        area = -area;
    }

    int minx = clampi(x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2), 0, WIDTH - 1);
    int miny = clampi(y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2), 0, HEIGHT - 1);
    int maxx = clampi(x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2), 0, WIDTH - 1);
    int maxy = clampi(y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2), 0, HEIGHT - 1);

    float inv_area = 1.0f / (float)area;

    // Pre-divide the UVs by w. These (u/w, v/w) are the quantities
    // that DO interpolate linearly in screen space — unlike u, v
    // themselves. 1/w interpolates linearly too.
    float u0w = uv0.x * iw0, v0w = uv0.y * iw0;
    float u1w = uv1.x * iw1, v1w = uv1.y * iw1;
    float u2w = uv2.x * iw2, v2w = uv2.y * iw2;

    for (int y = miny; y <= maxy; y++) {
        for (int x = minx; x <= maxx; x++) {
            int e0 = edge({ (float)x1, (float)y1 }, { (float)x2, (float)y2 }, { (float)x, (float)y });
            int e1 = edge({ (float)x2, (float)y2 }, { (float)x0, (float)y0 }, { (float)x, (float)y });
            int e2 = edge({ (float)x0, (float)y0 }, { (float)x1, (float)y1 }, { (float)x, (float)y });
            if ((e0 | e1 | e2) < 0) continue;

            float l0 = e0 * inv_area;
            float l1 = e1 * inv_area;
            float l2 = e2 * inv_area;

            // Depth: linear in screen space already (chapter 7).
            float z = l0 * z0 + l1 * z1 + l2 * z2;

            int idx = y * WIDTH + x;
            if (z >= zbuffer[idx]) continue;

#if PERSPECTIVE_CORRECT
            // Interpolate u/w, v/w, 1/w linearly, then divide back.
            float uw = l0 * u0w + l1 * u1w + l2 * u2w;
            float vw = l0 * v0w + l1 * v1w + l2 * v2w;
            float iw = l0 * iw0 + l1 * iw1 + l2 * iw2;
            float u = uw / iw;
            float v = vw / iw;
#else
            // Naive: interpolate u, v directly in screen space. Wrong
            // under perspective — the texture visibly warps.
            float u = l0 * uv0.x + l1 * uv1.x + l2 * uv2.x;
            float v = l0 * uv0.y + l1 * uv1.y + l2 * uv2.y;
#endif
            zbuffer[idx] = z;
            pixels[idx] = texture_sample(tex, u, v);
        }
    }
}



int Framebuffer::getWidth() const
{
    return width;
}

int Framebuffer::getHeight() const
{
    return height;
}

const std::vector<std::uint32_t>& Framebuffer::getPixels() const
{
    return pixels;
}