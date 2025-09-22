#include "player_state/PSIdle.h"
#include <iostream>
#include "mouse_manager.h"
#include "input_manager.h"

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
    camera->SetTarget(pos + front);

    auto &input = InputManager::Instance();
    bool moving =
        input.IsKeyHeld(GLFW_KEY_W) ||
        input.IsKeyHeld(GLFW_KEY_S) ||
        input.IsKeyHeld(GLFW_KEY_A) ||
        input.IsKeyHeld(GLFW_KEY_D);

    StateAction action;

    if (moving)
    {
        action.type = StateActionType::Push;
        action.nextState = "PSMove";
    }

    return action;
}
