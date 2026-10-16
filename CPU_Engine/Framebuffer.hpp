#pragma once
#include <cstdint>
#include <vector>

class Framebuffer
{
public:
    struct Vec2
    {
        int x;
        int y;
    };
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
    void drawTriangleGradient(Vec2 p0, Vec2 p1, Vec2 p2);
    int edge(Vec2 p0, Vec2 p1, Vec2 p2);
    int getWidth() const;
    int getHeight() const;

    const std::vector<std::uint32_t>& getPixels() const;

    
private:
    int width;
    int height;
    std::vector<std::uint32_t> pixels;
};