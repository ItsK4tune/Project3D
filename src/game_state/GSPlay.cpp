#include "game_state/GSPlay.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource_manager.h"
#include "scene_manager.h"

void GSPlay::Init()
{
    std::cout << "[GSPlay::Init] Initializing game play state." << std::endl;
}

void GSPlay::Enter()
{
    std::cout << "[GSPlay::Enter] Entering game play state." << std::endl;
}

void GSPlay::Exit()
{
    std::cout << "[GSPlay::Exit] Exiting game play state." << std::endl;
}

// GSPlay.cpp
StateAction GSPlay::Update(float deltaTime, GLFWwindow *window)
{
    auto camera = SceneManager::Instance().GetCamera();

    // ====== Keyboard di chuyển ======
    const float speed = 2.5f * deltaTime;
    glm::vec3 pos = camera->GetPosition();
    glm::vec3 front = glm::normalize(camera->GetTarget() - pos);
    glm::vec3 right = glm::normalize(glm::cross(front, camera->GetUp()));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) pos += front * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) pos -= front * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) pos -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) pos += right * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) pos += camera->GetUp() * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) pos -= camera->GetUp() * speed;

    camera->SetPosition(pos);

    // ====== Mouse xoay camera ======
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (m_firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - m_lastX);
    float yoffset = static_cast<float>(m_lastY - ypos); // đảo chiều y
    m_lastX = xpos;
    m_lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_yaw   += xoffset;
    m_pitch += yoffset;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    glm::vec3 newFront;
    newFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    newFront.y = sin(glm::radians(m_pitch));
    newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    camera->SetTarget(pos + glm::normalize(newFront));

    // ====== Trả về action ======
    StateAction action;
    action.type = StateActionType::None;
    return action;
}

void GSPlay::Render()
{
    SceneManager::Instance().Draw();
}