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

bool SceneManager::LoadFromFile(const std::string &filePath)
{
    Cleanup();
    return AddFromFile(filePath);
}

bool SceneManager::AddFromFile(const std::string &filePath)
{
    std::cout << "[SceneManager::AddFromFile] Loading scene..." << std::endl;
    std::string fullPath = "resource/script/" + filePath;

    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "[SceneManager::AddFromFile] Cannot open file: " << filePath << std::endl;
        return false;
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
            std::cout << "[SceneManager::AddFromFile] Loaded camera: POS("
                      << pos.x << "," << pos.y << "," << pos.z
                      << ") TARGET(" << target.x << "," << target.y << "," << target.z
                      << ") UP(" << up.x << "," << up.y << "," << up.z << ")\n";

            if (line.rfind("#", 0) != 0)
                continue;
        }

        if (line.rfind("#Object", 0) == 0)
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
                    std::cerr << "[SceneManager::AddFromFile] Object[" << id << "] missing model: " << modelID << std::endl;
                    continue;
                }
                if (!shader)
                {
                    std::cerr << "[SceneManager::AddFromFile] Object[" << id << "] missing shader: " << shaderID << std::endl;
                    continue;
                }
                if (!texture && textureID != "-1")
                {
                    std::cerr << "[SceneManager::AddFromFile] Object[" << id << "] missing texture: " << textureID << std::endl;
                    continue;
                }

                objects.push_back(std::make_shared<Object>(model, shader, texture, pos, rot, scale));
            }

            std::cout << "[SceneManager::AddFromFile] Loaded " << count << " objects." << std::endl;
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

                huds[id] = std::make_shared<Object>(model, shader, texture, pos, rot, scale);
            }

            std::cout << "[SceneManager::AddFromFile] Loaded " << count << " HUDs." << std::endl;
            continue;
        }
    }

    if (!camera)
    {
        std::cout << "[SceneManager::AddFromFile] Loaded default camera: POS(0,0,3) TARGET(0,0,0) UP(0,1,0)\n";
        camera = std::make_shared<Camera>(45.0f, 0.1f, 100.0f);
    }

    std::cout << "[SceneManager::AddFromFile] Finished loading scene" << std::endl;
    return true;
}

bool SceneManager::AddObject(const std::shared_ptr<Object> &object)
{
    if (!object)
    {
        std::cerr << "[SceneManager::AddObject] Invalid object." << std::endl;
        return false;
    }
    objects.push_back(object);
    return true;
}

bool SceneManager::AddHUD(const std::shared_ptr<Object> &hud)
{
    if (!hud)
    {
        std::cerr << "[SceneManager::AddHUD] Invalid HUD." << std::endl;
        return false;
    }
    huds["HUD_" + std::to_string(huds.size())] = hud;
    return true;
}

bool SceneManager::SetCamera(const std::shared_ptr<Camera> &cam)
{
    if (!cam)
    {
        std::cerr << "[SceneManager::SetCamera] Invalid camera." << std::endl;
        return false;
    }
    camera = cam;
    return true;
}

void SceneManager::Cleanup()
{
    objects.clear();
    huds.clear();
    camera.reset();
}

void SceneManager::Update(float deltaTime)
{
    // Currently no dynamic updates needed
}

void SceneManager::DrawHUD()
{
    if (!camera)
    {
        std::cerr << "[SceneManager::Draw] No camera set." << std::endl;
        return;
    }

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 orthosMatrix = camera->GetOrthosMatrix();

    for (const auto &pair : huds)
    {
        const auto &hud = pair.second;
        if (hud)
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

    for (const auto &obj : objects)
    {
        if (obj)
            obj->Draw(viewMatrix, projectionMatrix);
    }
}