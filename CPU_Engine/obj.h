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

inline void mesh_free(Mesh* mesh)
{
    mesh->verts.clear();
    mesh->uvs.clear();
    mesh->faces.clear();
    mesh->face_uvs.clear();

}