#pragma once

#include <cstdint>
#include <vector>

struct Texture
{
    std::vector<uint32_t> pixels;
    int width = 0;
    int height = 0;
};

bool texture_load(const char* path, Texture* texture);

void texture_free(Texture* texture);

uint32_t texture_sample(const Texture* texture, float u, float v);