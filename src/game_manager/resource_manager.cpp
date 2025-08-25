#include "resource_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ResourceManager &ResourceManager::Instance()
{
    static ResourceManager instance;
    return instance;
}

ResourceManager::~ResourceManager()
{
    Cleanup();
}

void ResourceManager::Cleanup()
{
    modelMap.clear();
    textureMap.clear();
    shaderMap.clear();
}

std::shared_ptr<Model> ResourceManager::GetModel(const std::string &id) const
{
    auto it = modelMap.find(id);
    return (it != modelMap.end()) ? it->second : nullptr;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string &id) const
{
    auto it = textureMap.find(id);
    return (it != textureMap.end()) ? it->second : nullptr;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string &id) const
{
    auto it = shaderMap.find(id);
    return (it != shaderMap.end()) ? it->second : nullptr;
}

bool ResourceManager::LoadFromFile(const std::string &filePath)
{
    std::cout << "[ResourceManager::LoadFromFile] Loading resources..." << std::endl;
    std::string fullPath = "resource/script/" + filePath;

    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "[ResourceManager::LoadFromFile] Cannot open file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    int count = 0;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);

        if (line.rfind("#Model", 0) == 0)
        {
            iss.ignore(6);
            iss >> count;

            for (int i = 0; i < count; i++)
            {
                std::string id, fileToken, path;
                std::getline(file, line);
                std::istringstream issId(line);
                issId >> fileToken >> id;

                std::getline(file, line);
                size_t start = line.find('"');
                size_t end = line.find_last_of('"');
                path = line.substr(start + 1, end - start - 1);

                std::cout << "[ResourceManager::LoadFromFile] Model[" << id << "]: " << path << std::endl;
                auto model = std::make_shared<Model>(path);
                if (!model || model->meshes.empty())
                {
                    std::cerr << "[ResourceManager::LoadFromFile] Failed to load model: " << path << std::endl;
                    continue;
                }
                else
                    modelMap[id] = model;
            }

            std::cout << "[ResourceManager::LoadFromFile] Loaded " << modelMap.size() << " models." << std::endl;
        }

        else if (line.rfind("#Texture", 0) == 0)
        {
            iss.ignore(8);
            iss >> count;

            for (int i = 0; i < count; i++)
            {
                std::string id, fileToken, path;
                std::getline(file, line);
                std::istringstream issId(line);
                issId >> fileToken >> id;

                std::getline(file, line);
                size_t start = line.find('"');
                size_t end = line.find_last_of('"');
                path = line.substr(start + 1, end - start - 1);

                std::cout << "[ResourceManager::LoadFromFile] Texture[" << id << "]: " << path << std::endl;
                auto texture = std::make_shared<Texture>(path);
                if (!texture || texture->ID == 0)
                {
                    std::cerr << "[ResourceManager::LoadFromFile] Failed to load model: " << path << std::endl;
                    continue;
                }
                else
                    textureMap[id] = texture;
            }

            std::cout << "[ResourceManager::LoadFromFile] Loaded " << textureMap.size() << " textures." << std::endl;
        }

        else if (line.rfind("#Shader", 0) == 0)
        {
            iss.ignore(7);
            iss >> count;

            for (int i = 0; i < count; i++)
            {
                std::string id, vsPath, fsPath;

                std::getline(file, line);
                {
                    std::istringstream issId(line);
                    std::string token;
                    issId >> token >> id;
                }

                std::getline(file, line);
                {
                    size_t start = line.find('"');
                    size_t end = line.find_last_of('"');
                    vsPath = line.substr(start + 1, end - start - 1);
                }

                std::getline(file, line);
                {
                    size_t start = line.find('"');
                    size_t end = line.find_last_of('"');
                    fsPath = line.substr(start + 1, end - start - 1);
                }

                std::cout << "[ResourceManager::LoadFromFile] Shader[" << id << "]: VS=" << vsPath << " FS=" << fsPath << std::endl;
                auto shader = std::make_shared<Shader>(vsPath, fsPath);
                if (!shader || shader->ID == 0)
                {
                    std::cerr << "[ResourceManager::LoadFromFile] Failed to load shader: " << id << std::endl;
                    continue;
                }
                else
                    shaderMap[id] = shader;
            }

            std::cout << "[ResourceManager::LoadFromFile] Loaded " << shaderMap.size() << " shaders." << std::endl;
        }
    }

    std::cout << "[ResourceManager::LoadFromFile] Finished loading resources" << std::endl;
    return true;
}