#include "texture.h"

#include <iostream>
#include <cmath>

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
        4);

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

    if (x >= texture->width)
        x = texture->width - 1;

    if (y >= texture->height)
        y = texture->height - 1;

    return texture->pixels[y * texture->width + x];
}
