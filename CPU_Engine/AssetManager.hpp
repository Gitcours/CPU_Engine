#pragma once

#include <map>
#include <string>

#include "obj.h"
#include "texture.h"

class AssetManager
{
private:
    std::map<std::string, Mesh> meshes;
    std::map<std::string, Texture> textures;

public:
    AssetManager();

    ~AssetManager();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    bool loadMesh(const std::string& name, const char* path);
    bool loadTexture(const std::string& name, const char* path);

    Mesh* getMesh(const std::string& name);
    Texture* getTexture(const std::string& name);

    void clear();
};