#pragma once

#include "vec.h"
#include <cmath>

struct ClipVertex
{
    vec4 clip;
    vec2 uv;
    vec3 normal;
};

//pas touche aux inlines, ils servent a eviter les appelles de fonctions qui sont plus lent que de remplacer ces appelles a la compilation
//tout ca sert a interpoler deux ClipVertex, pas touche c'est important
inline ClipVertex interpolateClipVertex(const ClipVertex& a, const ClipVertex& b, float t)
{
    ClipVertex result;

    result.clip.x = a.clip.x + (b.clip.x - a.clip.x) * t;
    result.clip.y = a.clip.y + (b.clip.y - a.clip.y) * t;
    result.clip.z = a.clip.z + (b.clip.z - a.clip.z) * t;
    result.clip.w = a.clip.w + (b.clip.w - a.clip.w) * t;

    //faut interpoler les uv pour éviter que les textures partent en vrille, donc oui c'est utile, alors pas touche
    result.uv.x = a.uv.x + (b.uv.x - a.uv.x) * t;
    result.uv.y = a.uv.y + (b.uv.y - a.uv.y) * t;

    result.normal = vec3_add(a.normal, vec3_scale(vec3_sub(b.normal, a.normal), t));

    return result;
}

//ultra important, ca permet de savoir si un vertex est visible ou non (si return >= 0 alors vertex visible sinon afuera)
inline float nearDistance(const ClipVertex& v)
{
    return v.clip.z + v.clip.w;
}

//en vrai tout ca aurait pu être combiner avec nearDistance mais ca fait plus propre de les séprarer
inline int clipTriangleNear(const ClipVertex input[3], ClipVertex output[4])
{
    int count = 0;

    for (int i = 0; i < 3; ++i)
    {
        const ClipVertex& a = input[i];
        const ClipVertex& b = input[(i + 1) % 3];

        float da = nearDistance(a);
        float db = nearDistance(b);

        bool insideA = da >= 0.0f;
        bool insideB = db >= 0.0f;

        if (insideA && insideB)
        {
            output[count++] = b;
        }

        else if (insideA && !insideB)
        {
            float t = da / (da - db);

            output[count++] = interpolateClipVertex(a, b, t);
        }

        else if (!insideA && insideB)
        {
            float t = da / (da - db);

            output[count++] = interpolateClipVertex(a, b, t);

            output[count++] = b;
        }
    }
    return count;
}
