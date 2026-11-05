#include "AssetManager.hpp"

#include <utility>

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
    clear();
}

bool AssetManager::loadMesh(const std::string& name, const char* path)
{
    if (meshes.find(name) != meshes.end())
        return false;

    Mesh mesh;

    if (!mesh_load(path, &mesh))
        return false;

    meshes.emplace(name, std::move(mesh));

    return true;
}

bool AssetManager::loadTexture(const std::string& name, const char* path)
{
    if (textures.find(name) != textures.end())
        return false;

    Texture texture;

    if (!texture_load(path, &texture))
        return false;

    textures.emplace(name, std::move(texture));

    return true;
}

Mesh* AssetManager::getMesh(const std::string& name)
{
    auto it = meshes.find(name);

    if (it == meshes.end())
        return nullptr;

    return &it->second;
}

Texture* AssetManager::getTexture(const std::string& name)
{
    auto it = textures.find(name);

    if (it == textures.end())
        return nullptr;

    return &it->second;
}

void AssetManager::clear()
{
    for (auto& mesh : meshes)
    {
        mesh_free(&mesh.second);
    }

    for (auto& texture : textures)
    {
        texture_free(&texture.second);
    }

    meshes.clear();
    textures.clear();
}