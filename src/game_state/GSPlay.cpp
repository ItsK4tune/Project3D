#include "game_state/GSPlay.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource_manager.h"
#include "scene_manager.h"

void GSplay::Init()
{
    std::cout << "[GSplay::Init] Initializing game play state." << std::endl;
    ResourceManager::Instance().LoadFromFile("load/test.txt");
    SceneManager::Instance().LoadFromFile("scene/test.txt");
}

void GSplay::Enter()
{
    std::cout << "[GSplay::Enter] Entering game play state." << std::endl;
}

void GSplay::Exit()
{
    std::cout << "[GSplay::Exit] Exiting game play state." << std::endl;
}

StateAction GSplay::Update(float deltaTime, GLFWwindow *window)
{
    auto camera = SceneManager::Instance().GetCamera();

    const float speed = 0.05f * deltaTime; // tốc độ di chuyển mỗi frame

    // --- Di chuyển theo XY ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->Move(glm::vec3(0.0f, +speed, 0.0f)); // lên (y+)
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->Move(glm::vec3(0.0f, -speed, 0.0f)); // xuống (y-)
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->Move(glm::vec3(-speed, 0.0f, 0.0f)); // trái (x-)
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->Move(glm::vec3(+speed, 0.0f, 0.0f)); // phải (x+)
    }

    // --- Di chuyển theo Z ---
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->Move(glm::vec3(0.0f, 0.0f, +speed)); // tiến (z+)
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->Move(glm::vec3(0.0f, 0.0f, -speed)); // lùi (z-)
    }

    StateAction action;
    action.type = StateActionType::None;
    return action;
}

void GSplay::Render()
{
    SceneManager::Instance().Draw();
}