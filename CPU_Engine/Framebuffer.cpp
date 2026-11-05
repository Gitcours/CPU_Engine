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
{
}

float Framebuffer::edge(vec3 p0, vec3 p1, vec3 p2) const
{
    vec2 a = {
        p1.x - p0.x,
        p1.y - p0.y
    };

    vec2 b = {
        p2.x - p0.x,
        p2.y - p0.y
    };

    return vec2_cross(a, b);
}

static inline int clampi(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

vec3 Framebuffer::getBarycentric(vec3 p0, vec3 p1, vec3 p2, vec3 p) const
{
    float area = edge(p0, p1, p2);

    return barycentric(
        edge(p1, p2, p) / area,
        edge(p2, p0, p) / area,
        edge(p0, p1, p) / area
    );
}

int Framebuffer::getPixelIndex(int x, int y) const
{
    return y * width + x;
}

void Framebuffer::clear(std::uint32_t color)
{
    for (auto& pixel : pixels)
    {
        pixel = color;
    }
}

void Framebuffer::clearZBuffer()
{
    std::fill(zbuffer.begin(), zbuffer.end(), maxF);
}

void Framebuffer::putPixel(int x, int y, std::uint32_t color)
{

    int index = getPixelIndex(x, y);
    pixels[index] = color;
}

void Framebuffer::putPixelDepth(int x, int y, float z, std::uint32_t color)
{
    int index = getPixelIndex(x, y);


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

        if (x0 == x1 && y0 == y1) break;

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

void Framebuffer::drawCircle(int cx, int cy, int r, uint32_t color)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y)
    {
        putPixel(cx + x, cy + y, color);
        putPixel(cx - x, cy + y, color);
        putPixel(cx + x, cy - y, color);
        putPixel(cx - x, cy - y, color);
        putPixel(cx + y, cy + x, color);
        putPixel(cx - y, cy + x, color);
        putPixel(cx + y, cy - x, color);
        putPixel(cx - y, cy - x, color);

        x++;

        if (d > 0)
        {
            y--;
            d += 4 * (x - y) + 10;
        }
        else
        {
            d += 4 * x + 6;
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

    BoundingBox box(x0, y0, x1, y1, x2, y2, width, height);

    for (int y = box.minY; y <= box.maxY; ++y)
    {
        for (int x = box.minX; x <= box.maxX; ++x)
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
    float area = static_cast<float>(edge(p0, p1, p2));

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

            vec3 weights = getBarycentric(p0, p1, p2, p);

            if (weights.x >= 0.0f &&
                weights.y >= 0.0f &&
                weights.z >= 0.0f)
            {
                float z =
                    weights.x * p0.z +
                    weights.y * p1.z +
                    weights.z * p2.z;

                putPixelDepth(x, y, z, color);
            }
        }
    }
}

void Framebuffer::drawTriangleGradient(vec3 p0, vec3 p1, vec3 p2)
{
    std::uint32_t RED = 0xFFFF0000;
    std::uint32_t GREEN = 0xFF00FF00;
    int area = edge(p0, p1, p2);

    if (area == 0) return;

    BoundingBox box(
        (int)p0.x, (int)p0.y,
        (int)p1.x, (int)p1.y,
        (int)p2.x, (int)p2.y,
        width, height
    );

    for (int y = box.minY; y <= box.maxY; y++)
    {
        for (int x = box.minX; x <= box.maxX; x++)
        {
            vec3 weights = getBarycentric(
                p0, p1, p2,
                { (float)x, (float)y, 0.0f }
            );

            if (weights.x >= 0.0f &&
                weights.y >= 0.0f &&
                weights.z >= 0.0f)
            {
                uint8_t r = weights.x * 255;
                uint8_t g = weights.y * 255;
                uint8_t b = weights.z * 255;

                uint32_t color = rgb(r, g, b);
                putPixel(x, y, color);
            }
        }
    }
}

void Framebuffer::drawTriangleTextured(
    int x0, int y0, float z0, float iw0, vec2 uv0,
    int x1, int y1, float z1, float iw1, vec2 uv1,
    int x2, int y2, float z2, float iw2, vec2 uv2,
    const Texture* tex)
{
    int area = edge(
        { (float)x0, (float)y0 },
        { (float)x1, (float)y1 },
        { (float)x2, (float)y2 }
    );

    if (area == 0) return;

    if (area < 0)
    {
        int tx = x1, ty = y1;
        float tz = z1, tiw = iw1;
        vec2 tuv = uv1;

        x1 = x2; y1 = y2; z1 = z2; iw1 = iw2; uv1 = uv2;
        x2 = tx; y2 = ty; z2 = tz; iw2 = tiw; uv2 = tuv;

        area = -area;
    }

    BoundingBox box(
        x0, y0,
        x1, y1,
        x2, y2,
        width, height
    );

    float inv_area = 1.0f / (float)area;

    float u0w = uv0.x * iw0, v0w = uv0.y * iw0;
    float u1w = uv1.x * iw1, v1w = uv1.y * iw1;
    float u2w = uv2.x * iw2, v2w = uv2.y * iw2;

    for (int y = box.minY; y <= box.maxY; y++)
    {
        for (int x = box.minX; x <= box.maxX; x++)
        {
            int e0 = edge(
                { (float)x1, (float)y1 },
                { (float)x2, (float)y2 },
                { (float)x, (float)y }
            );

            int e1 = edge(
                { (float)x2, (float)y2 },
                { (float)x0, (float)y0 },
                { (float)x, (float)y }
            );

            int e2 = edge(
                { (float)x0, (float)y0 },
                { (float)x1, (float)y1 },
                { (float)x, (float)y }
            );

            if ((e0 | e1 | e2) < 0) continue;

            vec3 weights = barycentric(
                e0 * inv_area,
                e1 * inv_area,
                e2 * inv_area
            );

            float z =
                weights.x * z0 +
                weights.y * z1 +
                weights.z * z2;

            int idx = getPixelIndex(x, y);

            if (z >= zbuffer[idx]) continue;

#if PERSPECTIVE_CORRECT
            float uw = weights.x * u0w + weights.y * u1w + weights.z * u2w;
            float vw = weights.x * v0w + weights.y * v1w + weights.z * v2w;
            float iw = weights.x * iw0 + weights.y * iw1 + weights.z * iw2;

            float u = uw / iw;
            float v = vw / iw;
#else
            float u = weights.x * uv0.x + weights.y * uv1.x + weights.z * uv2.x;
            float v = weights.x * uv0.y + weights.y * uv1.y + weights.z * uv2.y;
#endif

            zbuffer[idx] = z;
            pixels[idx] = texture_sample(tex, u, v);
        }
    }
}

uint32_t Framebuffer::rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)0xFFu << 24)
        | ((uint32_t)r << 16)
        | ((uint32_t)g << 8)
        | (uint32_t)b;
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