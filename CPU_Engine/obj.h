#pragma once

#include "vec.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Mesh
{
    std::vector<vec3> verts;
    std::vector<vec2> uvs;
    std::vector<int> faces;
    std::vector<int> face_uvs;
};

inline bool mesh_load(const char* path, Mesh* mesh)
{
    std::ifstream file(path);

    if (!file.is_open())
        return false;

    mesh->verts.clear();
    mesh->uvs.clear();
    mesh->faces.clear();
    mesh->face_uvs.clear();

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream stream(line);
        std::string type;

        stream >> type;

        if (type == "v")
        {
            vec3 vertex;

            stream >> vertex.x >> vertex.y >> vertex.z;

            mesh->verts.push_back(vertex);
        }
        else if (type == "vt")
        {
            vec2 uv;

            stream >> uv.x >> uv.y;

            mesh->uvs.push_back(uv);
        }
        else if (type == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                std::string token;
                stream >> token;

                size_t firstSlash = token.find('/');
                size_t secondSlash = token.find('/', firstSlash + 1);

                int vertexIndex;
                int uvIndex = -1;

                if (firstSlash == std::string::npos)
                {
                    vertexIndex = std::stoi(token);
                }
                else
                {
                    vertexIndex = std::stoi(token.substr(0, firstSlash));

                    if (secondSlash == std::string::npos)
                    {
                        if (firstSlash + 1 < token.size())
                            uvIndex = std::stoi(token.substr(firstSlash + 1));
                    }
                    else if (secondSlash > firstSlash + 1)
                    {
                        uvIndex = std::stoi(
                            token.substr(firstSlash + 1,
                                secondSlash - firstSlash - 1));
                    }
                }

                mesh->faces.push_back(vertexIndex - 1);

                if (uvIndex > 0)
                    mesh->face_uvs.push_back(uvIndex - 1);
                else
                    mesh->face_uvs.push_back(-1);
            }
        }
    }

    file.close();

    return true;
}

static void obj__add_triangle(
    Mesh* out,
    int* fcap,

    int va, int ta,
    int vb, int tb,
    int vc, int tc
)
{
    if (out->face_count >= *fcap)
    {
        *fcap *= 2;

        out->faces = (int*)realloc(
            out->faces,
            (*fcap) * 3 * sizeof(int)
        );

        out->face_uvs = (int*)realloc(
            out->face_uvs,
            (*fcap) * 3 * sizeof(int)
        );
    }

    int fi = out->face_count * 3;

    out->faces[fi + 0] = va - 1;
    out->faces[fi + 1] = vb - 1;
    out->faces[fi + 2] = vc - 1;

    out->face_uvs[fi + 0] = ta ? ta - 1 : -1;
    out->face_uvs[fi + 1] = tb ? tb - 1 : -1;
    out->face_uvs[fi + 2] = tc ? tc - 1 : -1;

    out->face_count++;
}

static int mesh_load(const char* path, Mesh* out)
{
    FILE* f = fopen(path, "r");

    if (!f)
    {
        fprintf(stderr, "could not open %s\n", path);
        return 0;
    }

    int vcap = 1024;
    int vtcap = 1024;
    int fcap = 1024;

    out->verts = (vec3*)malloc(vcap * sizeof(vec3));
    out->uvs = (vec2*)malloc(vtcap * sizeof(vec2));

    out->faces =
        (int*)malloc(fcap * 3 * sizeof(int));

    out->face_uvs =
        (int*)malloc(fcap * 3 * sizeof(int));

    out->vert_count = 0;
    out->uv_count = 0;
    out->face_count = 0;

    char line[4096];


    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            vec3 v;

            if (sscanf(
                line + 2,
                "%f %f %f",
                &v.x,
                &v.y,
                &v.z
            ) == 3)
            {
                if (out->vert_count >= vcap)
                {
                    vcap *= 2;

                    out->verts = (vec3*)realloc(
                        out->verts,
                        vcap * sizeof(vec3)
                    );
                }

                out->verts[out->vert_count++] = v;
            }
        }
        else if (
            line[0] == 'v' &&
            line[1] == 't' &&
            line[2] == ' '
            )
        {
            vec2 t;

            if (sscanf(
                line + 3,
                "%f %f",
                &t.x,
                &t.y
            ) >= 2)
            {
                if (out->uv_count >= vtcap)
                {
                    vtcap *= 2;

                    out->uvs = (vec2*)realloc(
                        out->uvs,
                        vtcap * sizeof(vec2)
                    );
                }

                out->uvs[out->uv_count++] = t;
            }
        }

        else if (line[0] == 'f' && line[1] == ' ')
        {
            const char* p = line + 2;
            const int MAX_FACE_VERTS = 64;

            int vis[MAX_FACE_VERTS];
            int tis[MAX_FACE_VERTS];

            int count = 0;

            while (*p &&
                *p != '\n' &&
                *p != '\r')
            {
                while (*p == ' ' || *p == '\t')
                    p++;

                if (!*p || *p == '\n' || *p == '\r')
                    break;

                if (count >= MAX_FACE_VERTS)
                {
                    fprintf(
                        stderr,
                        "OBJ: face has too many vertices\n"
                    );
                    break;
                }

                int vi;
                int ti;

                obj__parse_face_token(
                    &p,
                    &vi,
                    &ti
                );

                if (vi == 0)
                    break;

                vis[count] = vi;
                tis[count] = ti;

                count++;
            }

            if (count >= 3)
            {
                for (int i = 1; i < count - 1; i++)
                {
                    obj__add_triangle(
                        out,
                        &fcap,

                        vis[0],
                        tis[0],

                        vis[i],
                        tis[i],

                        vis[i + 1],
                        tis[i + 1]
                    );
                }
            }
        }
    }


    fclose(f);

    printf(
        "OBJ loaded: %d vertices, %d UVs, %d triangles\n",
        out->vert_count,
        out->uv_count,
        out->face_count
    );

    return 1;
}


inline void mesh_free(Mesh* mesh)
{
    mesh->verts.clear();
    mesh->uvs.clear();
    mesh->faces.clear();
    mesh->face_uvs.clear();

}