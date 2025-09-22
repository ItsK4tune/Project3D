#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <vector>

enum class KeyState { 
    Pressed,   // mới nhấn frame này
    Released,  // thả ra hoặc chưa nhấn
    Held       // giữ phím
};

class InputManager {
public:
    static InputManager& Instance();

    void AddKeyToTrack(int key);
    void RemoveKeyToTrack(int key);
    void RemoveAllKeys();

    void Update(GLFWwindow* window);

    bool IsKeyPressed(int key) const;   // true nếu mới nhấn frame này
    bool IsKeyHeld(int key) const;      // true nếu đang giữ
    bool IsKeyReleased(int key) const;  // true nếu vừa thả hoặc ko nhấn

private:
    InputManager() = default;
    std::unordered_map<int, KeyState> trackedKeys;
};
