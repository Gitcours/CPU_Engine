#pragma once

#include <algorithm>

struct BoundingBox
{
    int minX;
    int minY;
    int maxX;
    int maxY;

    BoundingBox(int x0, int y0, int x1, int y1, int x2, int y2,
        int width, int height)
    {
        minX = std::max(0, std::min({ x0, x1, x2 }));
        minY = std::max(0, std::min({ y0, y1, y2 }));

        maxX = std::min(width - 1, std::max({ x0, x1, x2 }));
        maxY = std::min(height - 1, std::max({ y0, y1, y2 }));
    }
};