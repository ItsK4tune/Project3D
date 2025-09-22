#include "game_state/GSPlay.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource_manager.h"
#include "scene_manager.h"
#include "physic_manager.h"
// #include "mouse_manager.h"

void GSPlay::Init()
{
    std::cout << "[GSPlay::Init] Initializing game play state." << std::endl;
    ResourceManager::Instance().LoadFromFile("load/test.txt");
    SceneManager::Instance().LoadFromFile("scene/test.txt");
}

void GSPlay::Enter()
{
    std::cout << "[GSPlay::Enter] Entering game play state." << std::endl;
}

void GSPlay::Exit()
{
    std::cout << "[GSPlay::Exit] Exiting game play state." << std::endl;
}

StateAction GSPlay::Update(float deltaTime, GLFWwindow *window)
{
    if (!m_isCursorDisabled)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_isCursorDisabled = true;
        m_firstMouse = true;
    }

    auto camera = SceneManager::Instance().GetCamera();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && m_isCursorDisabled)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_isCursorDisabled = false;

        camera->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
        camera->SetTarget(glm::vec3(0.0f, 0.0f, -1.0f));

        StateAction action;
        action.type = StateActionType::Pop;
        return action;
    }

    const float speed = 2.5f * deltaTime;
    glm::vec3 pos = camera->GetPosition();
    glm::vec3 front = glm::normalize(camera->GetTarget() - pos);
    glm::vec3 right = glm::normalize(glm::cross(front, camera->GetUp()));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pos += front * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pos -= front * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pos -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pos += right * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        pos += camera->GetUp() * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        pos -= camera->GetUp() * speed;

    camera->SetPosition(pos);

    PhysicManager::Instance().Update(deltaTime);
    SceneManager::Instance().Update(deltaTime);

    StateAction action;
    action.type = StateActionType::None;
    return action;
}

void GSPlay::Render()
{
    SceneManager::Instance().Draw();
}