#pragma once
#include <cstdint>
#include <vector>

class Framebuffer
{
public:
    Framebuffer(int width, int height);

    void clear(std::uint32_t color);
    void putPixel(int x, int y, std::uint32_t color);
    void drawLine(int x0, int y0,int x1, int y1,std::uint32_t color);
    void drawTriangle(int x0, int y0,int x1, int y1,int x2, int y2,std::uint32_t color);
    void drawTriangleGradient(int x0, int y0, std::uint32_t color0,int x1, int y1, std::uint32_t color1,int x2, int y2, std::uint32_t color2);
    int getWidth() const;
    int getHeight() const;

    const std::vector<std::uint32_t>& getPixels() const;

private:
    int width;
    int height;
    std::vector<std::uint32_t> pixels;
};