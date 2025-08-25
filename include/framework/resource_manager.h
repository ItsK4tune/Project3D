#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "model.h"
#include "texture.h"
#include "shader.h"

class ResourceManager
{
public:
    static ResourceManager &Instance();

    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;

    bool LoadFromFile(const std::string& filePath);

    std::shared_ptr<Model> GetModel(const std::string &id) const;
    std::shared_ptr<Texture> GetTexture(const std::string &id) const;
    std::shared_ptr<Shader> GetShader(const std::string &id) const;

    void Cleanup();

private:
    ResourceManager() = default;
    ~ResourceManager();

    std::unordered_map<std::string, std::shared_ptr<Model>> modelMap;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaderMap;
};
