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

int texture_load(const char* path, Texture* out);
void texture_free(Texture* tex);
uint32_t texture_sample(const Texture* tex, float u, float v);
uint32_t texture_sample_bilinear(const Texture* tex, float u, float v);
