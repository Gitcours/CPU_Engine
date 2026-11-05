#include "texture.h"

#include <iostream>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool texture_load(const char* path, Texture* texture)
{
    int width;
    int height;
    int channels;

    unsigned char* data = stbi_load(
        path,
        &width,
        &height,
        &channels,
        4
    );

    if (!data)
    {
        std::cerr << "Impossible de charger la texture : "
                  << path << std::endl;

        return false;
    }

    texture->width = width;
    texture->height = height;

    texture->pixels.resize(width * height);

    for (int i = 0; i < width * height; i++)
    {
        uint8_t r = data[i * 4 + 0];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];

        texture->pixels[i] =
            ((uint32_t)a << 24) |
            ((uint32_t)r << 16) |
            ((uint32_t)g << 8) |
            (uint32_t)b;
    }

    stbi_image_free(data);

    return true;
}

void texture_free(Texture* texture)
{
    texture->pixels.clear();

    texture->width = 0;
    texture->height = 0;
}

uint32_t texture_sample(const Texture* texture, float u, float v)
{
    u = u - std::floor(u);
    v = v - std::floor(v);

    int x = static_cast<int>(u * texture->width);
    int y = static_cast<int>((1.0f - v) * texture->height);

    x = std::min(x, texture->width - 1);
    y = std::min(y, texture->height - 1);

    return texture->pixels[y * texture->width + x];
}

uint32_t texture_sample_bilinear(const Texture* texture, float u, float v)
{
    u = u - std::floor(u);
    v = v - std::floor(v);

    float x = u * (texture->width - 1);
    float y = (1.0f - v) * (texture->height - 1);

    int x0 = static_cast<int>(x);
    int y0 = static_cast<int>(y);

    int x1 = std::min(x0 + 1, texture->width - 1);
    int y1 = std::min(y0 + 1, texture->height - 1);

    float tx = x - x0;
    float ty = y - y0;

    uint32_t c00 = texture->pixels[y0 * texture->width + x0];
    uint32_t c10 = texture->pixels[y0 * texture->width + x1];
    uint32_t c01 = texture->pixels[y1 * texture->width + x0];
    uint32_t c11 = texture->pixels[y1 * texture->width + x1];

    uint32_t result = 0;

    for (int shift = 0; shift <= 24; shift += 8)
    {
        float c0 =
            ((c00 >> shift) & 255) * (1.0f - tx) +
            ((c10 >> shift) & 255) * tx;

        float c1 =
            ((c01 >> shift) & 255) * (1.0f - tx) +
            ((c11 >> shift) & 255) * tx;

        float value = c0 * (1.0f - ty) + c1 * ty;

        result |= static_cast<uint32_t>(value) << shift;
    }

    return result;
}