#include "input_manager.h"
#include <iostream>

InputManager& InputManager::Instance() {
    static InputManager instance;
    return instance;
}

void InputManager::AddKeyToTrack(int key) {
    trackedKeys[key] = KeyState::Released; // mặc định Released
}

void InputManager::RemoveKeyToTrack(int key) {
    trackedKeys.erase(key);
}

void InputManager::RemoveAllKeys() {
    trackedKeys.clear();
}

void InputManager::Update(GLFWwindow* window) {
    std::cout << "Updating\n";
    for (auto& [key, state] : trackedKeys) {
        int glfwState = glfwGetKey(window, key);

        if (glfwState == GLFW_PRESS) {
            if (state == KeyState::Released) {
                state = KeyState::Pressed;
            } else {
                state = KeyState::Held;
            }
        } else {
            state = KeyState::Released;
        }
    }
}

bool InputManager::IsKeyPressed(int key) const {
    auto it = trackedKeys.find(key);
    return (it != trackedKeys.end() && it->second == KeyState::Pressed);
}

bool InputManager::IsKeyHeld(int key) const {
    auto it = trackedKeys.find(key);
    return (it != trackedKeys.end() && it->second == KeyState::Held);
}

bool InputManager::IsKeyReleased(int key) const {
    auto it = trackedKeys.find(key);
    return (it != trackedKeys.end() && it->second == KeyState::Released);
}
