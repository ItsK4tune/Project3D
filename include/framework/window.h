#pragma once
#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    static std::unique_ptr<Window> Create(int major, int minor, const std::string& title);
    ~Window();

    void SetCallback();
    void Initialize();
    void MainLoop();

    GLFWwindow* GetNativeHandle() const { return m_window; }
private:
    Window(int major, int minor, const std::string& title);

    GLFWwindow* m_window;
    int m_major;
    int m_minor;
    std::string m_title;
};