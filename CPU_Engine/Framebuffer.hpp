#pragma once
#include <cstdint>
#include <vector>
#include "vec.h"
class Framebuffer
{
public:
    
    struct Color
    {
        int r;
        int g;
        int b;
        int a;
    };

    
    Framebuffer(int width, int height);

    void clear(std::uint32_t color);
    uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);
    void putPixel(int x, int y, std::uint32_t color);
    void drawLine(int x0, int y0,int x1, int y1,std::uint32_t color);
    void drawTriangle(int x0, int y0,int x1, int y1,int x2, int y2,std::uint32_t color);
    void drawTriangleGradient(vec3 p0, vec3 p1, vec3 p2);
    void drawCircle(int cx, int cy, int r, uint32_t color);
    int edge(vec3 p0, vec3 p1, vec3 p2);
    int getWidth() const;
    int getHeight() const;

    const std::vector<std::uint32_t>& getPixels() const;

    vec3 vec;
private:
    int width;
    int height;
    std::vector<std::uint32_t> pixels;
};