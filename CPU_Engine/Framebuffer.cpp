#include "Framebuffer.hpp"
#include <cstdlib>
#include <algorithm>
#include <cstdint>

Framebuffer::Framebuffer(int width, int height)
    : width(width),
    height(height),
    pixels(width* height, 0xFF000000)
{
}

void Framebuffer::clear(std::uint32_t color)
{
    for (auto& pixel : pixels)
    {
        pixel = color;
    }
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

void Framebuffer::drawLine(
    int x0, int y0,
    int x1, int y1,
    std::uint32_t color)
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


void Framebuffer::drawTriangle(
    int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    std::uint32_t color)
{
    // Aire orientee : permet de determiner l'orientation
    // du triangle et de rejeter les triangles degeneres.
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