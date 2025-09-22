#include "player_state/PSMove.h"
#include <iostream>
#include "mouse_manager.h"
#include "input_manager.h"

void PSMove::Init()
{
    std::cout << "[PSMove::Init] Initializing game play state." << std::endl;
}

void PSMove::Enter()
{
    std::cout << "[PSMove::Enter] Entering game play state." << std::endl;
}

void PSMove::Exit()
{
    std::cout << "[PSMove::Exit] Exiting game play state." << std::endl;
}

StateAction PSMove::Update(float deltaTime, Player *player)
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

    float &yaw = camera->GetYaw();
    float &pitch = camera->GetPitch();

    yaw += static_cast<float>(dx);
    pitch += static_cast<float>(dy);

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    glm::vec3 pos = camera->GetPosition();
    glm::vec3 right = glm::normalize(glm::cross(front, camera->GetUp()));

    bool moving = false;
    float speed = 5.0f * deltaTime;
    auto &input = InputManager::Instance();

    if (input.IsKeyHeld(GLFW_KEY_W))
    {
        pos += front * speed;
        moving = true;
    }
    if (input.IsKeyHeld(GLFW_KEY_S))
    {
        pos -= front * speed;
        moving = true;
    }
    if (input.IsKeyHeld(GLFW_KEY_A))
    {
        pos -= right * speed;
        moving = true;
    }
    if (input.IsKeyHeld(GLFW_KEY_D))
    {
        pos += right * speed;
        moving = true;
    }
    if (input.IsKeyHeld(GLFW_KEY_SPACE))
    {
        pos += camera->GetUp() * speed;
        moving = true;
    }
    if (input.IsKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        pos -= camera->GetUp() * speed;
        moving = true;
    }

    camera->SetPosition(pos);
    camera->SetTarget(pos + front);

    StateAction action;

    if (!moving)
    {
        action.type = StateActionType::Pop;
    }

    return action;
}