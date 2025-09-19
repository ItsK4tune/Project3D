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
    models.clear();
    textures.clear();
    shaders.clear();
}

std::shared_ptr<Model> ResourceManager::GetModel(const std::string &id) const
{
    for (const auto &model : models)
    {
        if (model && model->GetID() == id)
            return model;
    }
    return nullptr;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string &id) const
{
    for (const auto &texture : textures)
    {
        if (texture && texture->GetID() == id)
            return texture;
    }
    return nullptr;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string &id) const
{
    for (const auto &shader : shaders)
    {
        if (shader && shader->GetID() == id)
            return shader;
    }
    return nullptr;
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
                auto model = std::make_shared<Model>(id, path);
                if (!model || model->renderMeshes.empty())
                {
                    std::cerr << "[ResourceManager::LoadFromFile] Failed to load model: " << path << std::endl;
                    continue;
                }
                else
                    models.push_back(model);
            }

            std::cout << "[ResourceManager::LoadFromFile] Loaded " << models.size() << " models." << std::endl;
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
                auto texture = std::make_shared<Texture>(id, path);
                if (!texture || texture->ID == 0)
                {
                    std::cerr << "[ResourceManager::LoadFromFile] Failed to load model: " << path << std::endl;
                    continue;
                }
                else
                    textures.push_back(texture);
            }

            std::cout << "[ResourceManager::LoadFromFile] Loaded " << textures.size() << " textures." << std::endl;
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
                auto shader = std::make_shared<Shader>(id, vsPath, fsPath);
                if (!shader || shader->ID == 0)
                {
                    std::cerr << "[ResourceManager::LoadFromFile] Failed to load shader: " << id << std::endl;
                    continue;
                }
                else
                    shaders.push_back(shader);
            }

            std::cout << "[ResourceManager::LoadFromFile] Loaded " << shaders.size() << " shaders." << std::endl;
        }
    }

    std::cout << "[ResourceManager::LoadFromFile] Finished loading resources" << std::endl;
    return true;
}