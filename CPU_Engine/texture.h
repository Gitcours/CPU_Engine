// ============================================================
//  texture.h — load a PNG and sample it (chapter 9)
// ------------------------------------------------------------
//  We decode the PNG with stb_image, a single-header public-domain
//  library (third-party/stb_image.h), then repack its RGBA bytes
//  into our ARGB8888 uint32_t layout so a sampled texel can be
//  written straight into the framebuffer.
//
//  Two convention notes for the sampler:
//    - OBJ 'vt' coordinates put v = 0 at the BOTTOM of the image,
//      but image rows (and stb_image) start at the TOP — so we
//      flip v when indexing.
//    - U and V are wrapped into [0, 1] (fractional part) so a
//      stray out-of-range coordinate can't read past the array.
//
//  Sampling here is nearest-neighbour: pick the single closest
//  texel. It can look blocky up close; chapter 12 adds the
//  smoother bilinear version.
// ============================================================
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// stb_image is general-purpose and noisier under /W4 than our own
// code, so we quiet the warnings just around its implementation.


#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4244 4456 4457 4459 4505 4267)
#endif
#include "stb_image.h"
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

typedef struct {
    uint32_t* pixels;     // width * height texels, ARGB8888
    int       width;
    int       height;
} Texture;

int texture_load(const char* path, Texture* out);
void texture_free(Texture* tex);
uint32_t texture_sample(const Texture* tex, float u, float v);