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


void Framebuffer::drawTriangleGradient(
    int x0, int y0, std::uint32_t color0,
    int x1, int y1, std::uint32_t color1,
    int x2, int y2, std::uint32_t color2)
{
    // Fonction de bord : indique de quel côté d'une arête se trouve P.
    const auto edge = [](
        int ax, int ay,
        int bx, int by,
        int px, int py) -> double
        {
            return static_cast<double>(bx - ax) * (py - ay)
                - static_cast<double>(by - ay) * (px - ax);
        };

    const double area = edge(x0, y0, x1, y1, x2, y2);

    // Triangle dégénéré : ses sommets sont alignés.
    if (area == 0.0)
        return;

    // Zone rectangulaire à parcourir, limitée au framebuffer.
    const int minX = std::max(0, std::min({ x0, x1, x2 }));
    const int maxX = std::min(width - 1, std::max({ x0, x1, x2 }));
    const int minY = std::max(0, std::min({ y0, y1, y2 }));
    const int maxY = std::min(height - 1, std::max({ y0, y1, y2 }));

    // Décompose les couleurs ARGB en canaux.
    const auto channel = [](std::uint32_t color, int shift) {
        return static_cast<double>((color >> shift) & 0xFF);
        };

    const double r0 = channel(color0, 16);
    const double g0 = channel(color0, 8);
    const double b0 = channel(color0, 0);

    const double r1 = channel(color1, 16);
    const double g1 = channel(color1, 8);
    const double b1 = channel(color1, 0);

    const double r2 = channel(color2, 16);
    const double g2 = channel(color2, 8);
    const double b2 = channel(color2, 0);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            const double e0 = edge(x0, y0, x1, y1, x, y);
            const double e1 = edge(x1, y1, x2, y2, x, y);
            const double e2 = edge(x2, y2, x0, y0, x, y);

            // Même principe que notre rasterizer précédent.
            const bool inside = (area > 0.0)
                ? (e0 >= 0.0 && e1 >= 0.0 && e2 >= 0.0)
                : (e0 <= 0.0 && e1 <= 0.0 && e2 <= 0.0);

            if (!inside)
                continue;

            // Coordonnées barycentriques : contribution de chaque sommet.
            const double w0 = edge(x1, y1, x2, y2, x, y) / area;
            const double w1 = edge(x2, y2, x0, y0, x, y) / area;
            const double w2 = edge(x0, y0, x1, y1, x, y) / area;

            // Interpolation indépendante des canaux RGB.
            const auto toByte = [](double value) -> std::uint32_t {
                if (value < 0.0)
                    value = 0.0;

                if (value > 255.0)
                    value = 255.0;

                return static_cast<std::uint32_t>(value);
                };

            const std::uint32_t r = toByte(w0 * r0 + w1 * r1 + w2 * r2);
            const std::uint32_t g = toByte(w0 * g0 + w1 * g1 + w2 * g2);
            const std::uint32_t b = toByte(w0 * b0 + w1 * b1 + w2 * b2);

            // Alpha opaque, puis RGB interpolé.
            const std::uint32_t color = 0xFF000000
                | (r << 16)
                | (g << 8)
                | b;

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