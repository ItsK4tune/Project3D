#include "scene_manager.h"
#include "resource_manager.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

SceneManager &SceneManager::Instance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::~SceneManager()
{
    Cleanup();
}

std::shared_ptr<Entity> SceneManager::GetEntity(const std::string &name) const
{
    for (const auto &e : entities)
    {
        if (e && e->GetID() == name)
            return e;
    }
    return nullptr;
}

std::shared_ptr<HUD> SceneManager::GetHUD(const std::string &name) const
{
    for (const auto &hud : huds)
    {
        if (hud && hud->GetID() == name)
            return hud;
    }
    return nullptr;
}

void SceneManager::LoadFromFile(const std::string &filePath)
{
    Cleanup();
    std::cout << "[SceneManager::LoadFromFile] Loading scene..." << std::endl;
    std::string fullPath = "resource/script/" + filePath;

    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "[SceneManager::LoadFromFile] Cannot open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t\r\n"));

        if (line.empty())
            continue;

        if (line.rfind("#Camera", 0) == 0)
        {
            glm::vec3 pos(0.0f), target(0.0f), up(0.0f);

            while (std::getline(file, line))
            {
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                if (line.empty())
                    continue;
                if (line.rfind("#", 0) == 0)
                    break;

                if (line.rfind("POS", 0) == 0)
                {
                    std::istringstream iss(line.substr(3));
                    iss >> pos.x >> pos.y >> pos.z;
                }
                else if (line.rfind("TARGET", 0) == 0)
                {
                    std::istringstream iss(line.substr(6));
                    iss >> target.x >> target.y >> target.z;
                }
                else if (line.rfind("UP", 0) == 0)
                {
                    std::istringstream iss(line.substr(2));
                    iss >> up.x >> up.y >> up.z;
                }
            }

            camera = std::make_shared<Camera>(45.0f, 0.1f, 100.0f, pos, target, up);
            std::cout << "[SceneManager::LoadFromFile] Loaded camera: POS("
                      << pos.x << "," << pos.y << "," << pos.z
                      << ") TARGET(" << target.x << "," << target.y << "," << target.z
                      << ") UP(" << up.x << "," << up.y << "," << up.z << ")\n";

            if (line.rfind("#", 0) != 0)
                continue;
        }

        if (line.rfind("#Player", 0) == 0)
        {
            std::string id, modelID, textureID, shaderID;
            glm::vec3 pos(0.0f), rot(0.0f), scale(1.0f);

            std::getline(file, line);
            id = line.substr(3);
            std::getline(file, line);
            modelID = line.substr(6);
            std::getline(file, line);
            textureID = line.substr(8);
            std::getline(file, line);
            shaderID = line.substr(7);
            std::getline(file, line);
            {
                std::istringstream iss(line.substr(3));
                iss >> pos.x >> pos.y >> pos.z;
            }
            std::getline(file, line);
            {
                std::istringstream iss(line.substr(4));
                iss >> rot.x >> rot.y >> rot.z;
            }
            std::getline(file, line);
            {
                std::istringstream iss(line.substr(6));
                iss >> scale.x >> scale.y >> scale.z;
            }

            auto model = ResourceManager::Instance().GetModel(modelID);
            std::shared_ptr<Texture> texture = nullptr;
            if (textureID != "-1")
                texture = ResourceManager::Instance().GetTexture(textureID);
            auto shader = ResourceManager::Instance().GetShader(shaderID);

            if (!model)
            {
                std::cerr << "[SceneManager::LoadFromFile] Player[" << id << "] missing model: " << modelID << std::endl;
                continue;
            }
            if (!shader)
            {
                std::cerr << "[SceneManager::LoadFromFile] Player[" << id << "] missing shader: " << shaderID << std::endl;
                continue;
            }
            if (!texture && textureID != "-1")
            {
                std::cerr << "[SceneManager::LoadFromFile] Player[" << id << "] missing texture: " << textureID << std::endl;
                continue;
            }

            auto player = std::make_shared<Player>(id, model, shader, texture, pos, rot, scale, camera);
            entities.push_back(player);
            player->AttachRigidDynamic(1.0f);
            player->LockRotation(true, true, true);

            std::cout << "[SceneManager::LoadFromFile] Loaded player" << std::endl;
            continue;
        }

        if (line.rfind("#Map", 0) == 0)
        {
            int count = 0;
            std::istringstream iss(line);
            std::string token;
            iss >> token >> count;

            for (int i = 0; i < count; i++)
            {
                std::string id, modelID, textureID, shaderID;
                glm::vec3 pos(0.0f), rot(0.0f), scale(1.0f);

                std::getline(file, line);
                id = line.substr(3);
                std::getline(file, line);
                modelID = line.substr(6);
                std::getline(file, line);
                textureID = line.substr(8);
                std::getline(file, line);
                shaderID = line.substr(7);
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(3));
                    iss >> pos.x >> pos.y >> pos.z;
                }
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(4));
                    iss >> rot.x >> rot.y >> rot.z;
                }
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(6));
                    iss >> scale.x >> scale.y >> scale.z;
                }

                auto model = ResourceManager::Instance().GetModel(modelID);
                std::shared_ptr<Texture> texture = nullptr;
                if (textureID != "-1")
                    texture = ResourceManager::Instance().GetTexture(textureID);
                auto shader = ResourceManager::Instance().GetShader(shaderID);

                if (!model)
                {
                    std::cerr << "[SceneManager::LoadFromFile] Object[" << id << "] missing model: " << modelID << std::endl;
                    continue;
                }
                if (!shader)
                {
                    std::cerr << "[SceneManager::LoadFromFile] Object[" << id << "] missing shader: " << shaderID << std::endl;
                    continue;
                }
                if (!texture && textureID != "-1")
                {
                    std::cerr << "[SceneManager::LoadFromFile] Object[" << id << "] missing texture: " << textureID << std::endl;
                    continue;
                }

                auto entity = std::make_shared<Entity>(id, model, shader, texture, pos, rot, scale);
                entities.push_back(entity);
                entity->AttachRigidStatic();
                entity->SetStatic();
            }

            std::cout << "[SceneManager::LoadFromFile] Loaded " << count << " map." << std::endl;
            continue;
        }

        if (line.rfind("#Entity", 0) == 0)
        {
            int count = 0;
            std::istringstream iss(line);
            std::string token;
            iss >> token >> count;

            for (int i = 0; i < count; i++)
            {
                std::string id, modelID, textureID, shaderID;
                glm::vec3 pos(0.0f), rot(0.0f), scale(1.0f);

                std::getline(file, line);
                id = line.substr(3);
                std::getline(file, line);
                modelID = line.substr(6);
                std::getline(file, line);
                textureID = line.substr(8);
                std::getline(file, line);
                shaderID = line.substr(7);
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(3));
                    iss >> pos.x >> pos.y >> pos.z;
                }
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(4));
                    iss >> rot.x >> rot.y >> rot.z;
                }
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(6));
                    iss >> scale.x >> scale.y >> scale.z;
                }

                auto model = ResourceManager::Instance().GetModel(modelID);
                std::shared_ptr<Texture> texture = nullptr;
                if (textureID != "-1")
                    texture = ResourceManager::Instance().GetTexture(textureID);
                auto shader = ResourceManager::Instance().GetShader(shaderID);

                if (!model)
                {
                    std::cerr << "[SceneManager::LoadFromFile] Object[" << id << "] missing model: " << modelID << std::endl;
                    continue;
                }
                if (!shader)
                {
                    std::cerr << "[SceneManager::LoadFromFile] Object[" << id << "] missing shader: " << shaderID << std::endl;
                    continue;
                }
                if (!texture && textureID != "-1")
                {
                    std::cerr << "[SceneManager::LoadFromFile] Object[" << id << "] missing texture: " << textureID << std::endl;
                    continue;
                }

                auto entity = std::make_shared<Entity>(id, model, shader, texture, pos, rot, scale);
                entities.push_back(entity);
                entity->AttachRigidDynamic(1.0f);
                // entity->LockRotation(true, false, true);
            }

            std::cout << "[SceneManager::LoadFromFile] Loaded " << count << " objects." << std::endl;
            continue;
        }

        if (line.rfind("#HUD", 0) == 0)
        {
            int count = 0;
            std::istringstream iss(line);
            std::string token;
            iss >> token >> count;

            for (int i = 0; i < count; i++)
            {
                std::string id, modelID, textureID, shaderID;
                glm::vec3 pos(0.0f), rot(0.0f), scale(1.0f);

                std::getline(file, line);
                id = line.substr(3);
                std::getline(file, line);
                modelID = line.substr(6);
                std::getline(file, line);
                textureID = line.substr(8);
                std::getline(file, line);
                shaderID = line.substr(6);
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(3));
                    iss >> pos.x >> pos.y >> pos.z;
                }
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(4));
                    iss >> rot.x >> rot.y >> rot.z;
                }
                std::getline(file, line);
                {
                    std::istringstream iss(line.substr(6));
                    iss >> scale.x >> scale.y >> scale.z;
                }

                auto model = ResourceManager::Instance().GetModel(modelID);
                std::shared_ptr<Texture> texture = nullptr;
                if (textureID != "-1")
                    texture = ResourceManager::Instance().GetTexture(textureID);
                auto shader = ResourceManager::Instance().GetShader(shaderID);

                auto hud = std::make_shared<HUD>(id, model, shader, texture, pos, rot, scale);
                huds.push_back(hud);
            }

            std::cout << "[SceneManager::LoadFromFile] Loaded " << count << " HUDs." << std::endl;
            continue;
        }
    }

    if (!camera)
    {
        std::cout << "[SceneManager::LoadFromFile] Loaded default camera: POS(0,0,3) TARGET(0,0,0) UP(0,1,0)\n";
        camera = std::make_shared<Camera>(45.0f, 0.1f, 100.0f);
    }

    std::cout << "[SceneManager::LoadFromFile] Finished loading scene" << std::endl;
    return;
}

void SceneManager::Cleanup()
{
    entities.empty();
    huds.empty();
    camera.reset();
}

void SceneManager::DeactivateAll()
{
    for (auto &e : entities)
    {
        e->SetActive(false);
    }
}

void SceneManager::ActiveAll()
{
    for (auto &e : entities)
    {
        e->SetActive(true);
    }
}

void SceneManager::DeactivateEntity(const std::string &id)
{
    GetEntity(id)->SetActive(false);
}

void SceneManager::ActivateEntity(const std::string &id)
{
    GetEntity(id)->SetActive(true);
}

void SceneManager::Update(float deltaTime)
{
    for (auto &e : entities)
    {
        e->Update(deltaTime);
    }
}

void SceneManager::DrawHUD()
{
    if (!camera)
    {
        std::cerr << "[SceneManager::DrawHUD] No camera set." << std::endl;
        return;
    }

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 orthosMatrix = camera->GetOrthosMatrix();

    for (const auto &hud : huds)
    {
        hud->Draw(viewMatrix, orthosMatrix);
    }
}

void SceneManager::Draw()
{
    if (!camera)
    {
        std::cerr << "[SceneManager::Draw] No camera set." << std::endl;
        return;
    }

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetPerspectiveMatrix();

    for (const auto &e : entities)
    {
        e->Draw(viewMatrix, projectionMatrix);
    }
}