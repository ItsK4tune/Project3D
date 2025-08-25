#include "game.h"
#include <iostream>
#include "resource_manager.h"
#include "scene_manager.h"
#include "mouse_manager.h"
#include "global.h"

Game::Game(int major, int minor, const std::string &title)
    : m_window(nullptr), m_title(title), m_major(major), m_minor(minor)
{
    if (!glfwInit())
    {
        throw std::runtime_error("[Game::Game] Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(Global::ScreenWidth, Global::ScreenHeight, m_title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("[Game::Game] Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("[Game::Game] Failed to initialize GLAD");
    }

    glViewport(0, 0, Global::ScreenWidth, Global::ScreenHeight);
}

Game::~Game()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

std::unique_ptr<Game> Game::Create(int major, int minor, const std::string &title)
{
    return std::unique_ptr<Game>(new Game(major, minor, title));
}

void Game::SetCallback()
{
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int winW, winH;
            glfwGetWindowSize(window, &winW, &winH);
            int fbW, fbH;
            glfwGetFramebufferSize(window, &fbW, &fbH);

            xpos = xpos * fbW / winW;
            ypos = ypos * fbH / winH;

            bool isPressed = (action == GLFW_PRESS);

            MouseManager::Instance().OnMouseClickEvent((GLint)xpos, (GLint)ypos, isPressed);
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
    {
        int winW, winH;
        glfwGetWindowSize(window, &winW, &winH);
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        xpos = xpos * fbW / winW;
        ypos = ypos * fbH / winH;

        MouseManager::Instance().OnMouseMoveEvent((GLint)xpos, (GLint)ypos);
    });
}

void Game::Initialize()
{
    ResourceManager::Instance().LoadFromFile("load/test.txt");
    SceneManager::Instance().LoadFromFile("scene/test.txt");
}

void Game::MainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        SceneManager::Instance().Draw();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}
