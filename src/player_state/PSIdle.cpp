#include "player_state/PSIdle.h"

#include <iostream>
#include "global.h"
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include "scene_manager.h"
// #include "physic_manager.h"

void PSIdle::Init()
{
    std::cout << "[PSIdle::Init] Initializing game play state." << std::endl;
}

void PSIdle::Enter()
{
    std::cout << "[PSIdle::Enter] Entering game play state." << std::endl;
}

void PSIdle::Exit()
{
    std::cout << "[PSIdle::Exit] Exiting game play state." << std::endl;
}

StateAction PSIdle::Update(float deltaTime, Player *player)
{
    std::shared_ptr<Camera> camera = player->GetCamera();

    auto &mouse = MouseManager::Instance();

    double dx = mouse.x - mouse.last_x;
    double dy = mouse.last_y - mouse.y;

    mouse.last_x = mouse.x;
    mouse.last_y = mouse.y;

    float sensitivity = 0.1f;
    dx *= sensitivity;
    dy *= sensitivity;

    static float yaw = -90.0f;
    static float pitch = 0.0f;

    yaw   += static_cast<float>(dx);
    pitch += static_cast<float>(dy);

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front   = glm::normalize(front);

    camera->SetTarget(camera->GetPosition() + front);

    StateAction action;
    return action;
}
