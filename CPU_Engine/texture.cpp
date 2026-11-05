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
        out->pixels[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        
        if (!out->pixels) {
            stbi_image_free(data);
            out->width = out->height = 0;
            return 0;
        }
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


uint32_t texture_sample_bilinear(const Texture* tex, float u, float v)
{
    u = u - floorf(u);
    v = v - floorf(v);

    float x = u * tex->width - 0.5f;
    float y = (1.0f - v) * tex->height - 0.5f;

    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);

    float tx = x - floorf(x);
    float ty = y - floorf(y);

    int x1 = x0 + 1;
    int y1 = y0 + 1;

    auto wrap = [](int value, int size)
        {
            return (value % size + size) % size;
        };

    x0 = wrap(x0, tex->width);
    x1 = wrap(x1, tex->width);

    y0 = wrap(y0, tex->height);
    y1 = wrap(y1, tex->height);

    uint32_t c00 = tex->pixels[y0 * tex->width + x0];
    uint32_t c10 = tex->pixels[y0 * tex->width + x1];
    uint32_t c01 = tex->pixels[y1 * tex->width + x0];
    uint32_t c11 = tex->pixels[y1 * tex->width + x1];

    uint32_t result = 0;

    for (int shift = 0; shift <= 24; shift += 8)
    {
        float a = (float)((c00 >> shift) & 255);
        float b = (float)((c10 >> shift) & 255);
        float c = (float)((c01 >> shift) & 255);
        float d = (float)((c11 >> shift) & 255);

        float top = a + (b - a) * tx;
        float bottom = c + (d - c) * tx;

        float value = top + (bottom - top) * ty;

        uint32_t channel = (uint32_t)fminf(255.0f, fmaxf(0.0f, value));

        result |= channel << shift;
    }

    return result;
}
