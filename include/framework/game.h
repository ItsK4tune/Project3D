#pragma once
#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "state_machine.h"

class Game
{
public:
    static std::unique_ptr<Game> Create(int major, int minor, const std::string& title);
    ~Game();

    void SetCallback();
    void Initialize();
    void MainLoop();

    GLFWwindow* GetNativeHandle() const { return m_window; }
private:
    Game(int major, int minor, const std::string& title);

    GLFWwindow* m_window;
    int m_major;
    int m_minor;
    std::string m_title;
    std::unique_ptr<StateMachine> m_stateMachine;
};