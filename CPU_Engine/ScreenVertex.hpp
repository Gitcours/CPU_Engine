#pragma once

struct ScreenVertex
{
    int x;
    int y;
    float z;
    float invW;
    bool visible;
};