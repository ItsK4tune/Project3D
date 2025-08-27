#include "game_state/GSMapRoam.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource_manager.h"
#include "scene_manager.h"
#include "mouse_manager.h"
#include "global.h"

void GSMapRoam::Init()
{
    std::cout << "[GSMapRoam::Init] Initializing game play state." << std::endl;
    ResourceManager::Instance().LoadFromFile("load/test.txt");
    SceneManager::Instance().LoadFromFile("scene/test.txt");
}

void GSMapRoam::Enter()
{
    std::cout << "[GSMapRoam::Enter] Entering game play state." << std::endl;
}

void GSMapRoam::Exit()
{
    std::cout << "[GSMapRoam::Exit] Exiting game play state." << std::endl;
}

StateAction GSMapRoam::Update(float deltaTime, GLFWwindow *window)
{
    auto camera = SceneManager::Instance().GetCamera();
    auto &mouse = MouseManager::Instance();

    const float speed = 0.25f * deltaTime;
    const float edgeThreshold = 50.0f;
    const float moveLerpSpeed = 0.1f * deltaTime;
    const float rotateLerpSpeed = 0.1f * deltaTime;
    const float stopThreshold = 0.05f;

    if (!isPressed)
    {
        if (mouse.x <= edgeThreshold)
            camera->Move(glm::vec3(-speed, 0.0f, 0.0f));
        if (mouse.x >= Global::ScreenWidth - edgeThreshold)
            camera->Move(glm::vec3(speed, 0.0f, 0.0f));
        if (mouse.y <= edgeThreshold)
            camera->Move(glm::vec3(0.0f, 0.0f, -speed));
        if (mouse.y >= Global::ScreenHeight - edgeThreshold)
            camera->Move(glm::vec3(0.0f, 0.0f, speed));
    }

    if (mouse.isPressed && !isPressed)
    {
        float x = (2.0f * mouse.x) / Global::ScreenWidth - 1.0f;
        float y = 1.0f - (2.0f * mouse.y) / Global::ScreenHeight;
        glm::vec4 ray_nds = glm::vec4(x, y, -1.0f, 1.0f);

        glm::vec4 ray_eye = glm::inverse(camera->GetPerspectiveMatrix()) * ray_nds;
        ray_eye.z = -1.0f;
        ray_eye.w = 0.0f;

        glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(camera->GetViewMatrix()) * ray_eye));
        glm::vec3 rayOrigin = camera->GetPosition();

        float minT = std::numeric_limits<float>::max();
        bool hit = false;
        auto map = SceneManager::Instance().GetObject("O_MAP");
        if (!map)
        {
            std::cerr << "[GSMapRoam::Update] No map object found." << std::endl;
            return StateAction{StateActionType::None, ""};
        }
        auto mapModel = map->GetModel();
        auto mapMatrix = map->GetWorldMatrix();
        for (const auto &mesh : mapModel->hitboxMeshes)
        {
            for (size_t i = 0; i < mesh.indices.size(); i += 3)
            {
                auto transformVec3 = [&](const glm::vec3& v) {
                    return glm::vec3(mapMatrix * glm::vec4(v, 1.0f));
                    };

                glm::vec3 v0 = transformVec3(mesh.vertices[mesh.indices[i]].Position);
                glm::vec3 v1 = transformVec3(mesh.vertices[mesh.indices[i + 1]].Position);
                glm::vec3 v2 = transformVec3(mesh.vertices[mesh.indices[i + 2]].Position);

                float t;
                const float EPSILON = 1e-8f;
                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;
                glm::vec3 h = glm::cross(rayDir, edge2);
                float a = glm::dot(edge1, h);
                if (fabs(a) < EPSILON)
                    continue;
                float f = 1.0f / a;
                glm::vec3 s = rayOrigin - v0;
                float u = f * glm::dot(s, h);
                if (u < 0.0f || u > 1.0f)
                    continue;
                glm::vec3 q = glm::cross(s, edge1);
                float v = f * glm::dot(rayDir, q);
                if (v < 0.0f || u + v > 1.0f)
                    continue;
                t = f * glm::dot(edge2, q);
                if (t > EPSILON && t < minT)
                {
                    minT = t;
                    newPos = rayOrigin + rayDir * t + glm::vec3(0.0f, 0.1f, 0.0f);
                    hit = true;
                }
            }
        }

        if (hit)
        {
            isPressed = true;
        }
    }

    if (isPressed)
    {
        glm::vec3 camPos = camera->GetPosition();
        glm::vec3 dir = newPos - camPos;
        float dist = glm::length(dir);

        if (dist < stopThreshold)
        {
            camera->SetPosition(newPos);
            isPressed = false;

            StateAction action;
            action.type = StateActionType::Push;
            action.nextState = "GSPlay";
            return action;
        }
        else
        {
            camera->SetPosition(glm::mix(camPos, newPos, moveLerpSpeed));
            glm::vec3 currentFront = glm::normalize(camera->GetTarget() - camPos);
            glm::vec3 newFront = glm::normalize(glm::mix(currentFront, glm::vec3(1, 0, 0), rotateLerpSpeed));
            camera->SetTarget(camera->GetPosition() + newFront);
        }
    }

    StateAction action;
    action.type = StateActionType::None;
    return action;
}

void GSMapRoam::Render()
{
    SceneManager::Instance().Draw();
}