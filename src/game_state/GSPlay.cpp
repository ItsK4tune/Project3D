#include "game_state/GSPlay.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource_manager.h"
#include "scene_manager.h"
#include "physic_manager.h"
#include "input_manager.h"

void GSPlay::Init()
{
    std::cout << "[GSPlay::Init] Initializing game play state." << std::endl;
    ResourceManager::Instance().LoadFromFile("load/test.txt");
    SceneManager::Instance().LoadFromFile("scene/test.txt");
}

void GSPlay::Enter()
{
    std::cout << "[GSPlay::Enter] Entering game play state." << std::endl;
    auto &input = InputManager::Instance();
    input.AddKeyToTrack(GLFW_KEY_W);
    input.AddKeyToTrack(GLFW_KEY_A);
    input.AddKeyToTrack(GLFW_KEY_S);
    input.AddKeyToTrack(GLFW_KEY_D);
    input.AddKeyToTrack(GLFW_KEY_F1);
}

void GSPlay::Exit()
{
    std::cout << "[GSPlay::Exit] Exiting game play state." << std::endl;
    auto &input = InputManager::Instance();
    input.RemoveKeyToTrack(GLFW_KEY_W);
    input.RemoveKeyToTrack(GLFW_KEY_A);
    input.RemoveKeyToTrack(GLFW_KEY_S);
    input.RemoveKeyToTrack(GLFW_KEY_D);
    input.RemoveKeyToTrack(GLFW_KEY_F1);
}

StateAction GSPlay::Update(float deltaTime, GLFWwindow *window)
{
    InputManager::Instance().Update(window);
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