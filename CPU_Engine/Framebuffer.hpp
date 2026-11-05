#pragma once

#include <cstdint>
#include <vector>

#include "vec.h"
#include "texture.h"
#include "BoundingBox.h"

class Framebuffer
{
private:
    int width;
    int height;

    std::vector<std::uint32_t> pixels;
    std::vector<float> zbuffer;

public:

    struct Color
    {
        int r;
        int g;
        int b;
        int a;
    };

    Framebuffer(int width, int height);

    float edge(vec3 p0, vec3 p1, vec3 p2) const;
    vec3 getBarycentric(vec3 p0, vec3 p1, vec3 p2, vec3 p) const;
    int getPixelIndex(int x, int y) const;

    void clear(std::uint32_t color);
    void clearZBuffer();

    void putPixel(int x, int y, std::uint32_t color);
    void putPixelDepth(int x, int y, float z, std::uint32_t color);

    void drawLine(int x0, int y0, int x1, int y1, std::uint32_t color);
    void drawCircle(int cx, int cy, int r, std::uint32_t color);

    void drawTriangle(
        int x0, int y0,
        int x1, int y1,
        int x2, int y2,
        std::uint32_t color
    );

    void drawTriangleZ(vec3 p0, vec3 p1, vec3 p2, std::uint32_t color);
    void drawTriangleGradient(vec3 p0, vec3 p1, vec3 p2);

    void drawTriangleTextured(
        int x0, int y0, float z0, float iw0, vec2 uv0,
        int x1, int y1, float z1, float iw1, vec2 uv1,
        int x2, int y2, float z2, float iw2, vec2 uv2,
        const Texture* tex
    );

    uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

    int getWidth() const;
    int getHeight() const;

    const std::vector<std::uint32_t>& getPixels() const;

    vec3 vec;
};