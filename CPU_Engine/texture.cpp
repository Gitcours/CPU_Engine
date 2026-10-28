#include "texture.h"

// Load a PNG into ARGB8888. Returns 1 on success, 0 on failure.
int texture_load(const char* path, Texture* out) {
    int w, h, channels;
    // Ask stb_image for 4 channels; it fills alpha = 255 for images
    // that don't carry one.
    uint8_t* data = stbi_load(path, &w, &h, &channels, 4);
    if (!data) {
        fprintf(stderr, "texture_load: failed on '%s': %s\n",
            path, stbi_failure_reason());
        return 0;
    }

    out->width = w;
    out->height = h;
    out->pixels = (uint32_t*)malloc((size_t)w * (size_t)h * sizeof(uint32_t));

    // Repack RGBA8 -> ARGB8888 (our framebuffer's native layout).
    for (int i = 0; i < w * h; i++) {
        uint8_t r = data[i * 4 + 0];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];
        out->pixels[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16)
            | ((uint32_t)g << 8) | (uint32_t)b;
    }

    stbi_image_free(data);
    return 1;
}

void texture_free(Texture* tex) {
    free(tex->pixels);
    tex->pixels = NULL;
    tex->width = tex->height = 0;
}

// Nearest-neighbour sample at texture coordinate (u, v). U and V are
// wrapped to [0, 1]; v is flipped because OBJ's v = 0 is the image
// bottom but our pixel array's row 0 is the top.
uint32_t texture_sample(const Texture* tex, float u, float v) {
    u = u - floorf(u);
    v = v - floorf(v);
    int x = (int)(u * (float)tex->width);
    int y = (int)((1.0f - v) * (float)tex->height);
    if (x < 0) x = 0; if (x >= tex->width)  x = tex->width - 1;
    if (y < 0) y = 0; if (y >= tex->height) y = tex->height - 1;
    return tex->pixels[y * tex->width + x];
}