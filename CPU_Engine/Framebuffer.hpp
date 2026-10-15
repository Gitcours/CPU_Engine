#pragma once

#include <cstdint>
#include <vector>

class Framebuffer
{
public:
    Framebuffer(int width, int height);

    void clear(std::uint32_t color);
    void putPixel(int x, int y, std::uint32_t color);

    int getWidth() const;
    int getHeight() const;

    const std::vector<std::uint32_t>& getPixels() const;

private:
    int width;
    int height;
    std::vector<std::uint32_t> pixels;
};