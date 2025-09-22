#include "game.h"
#include <iostream>
#include "resource_manager.h"
#include "scene_manager.h"
#include "mouse_manager.h"
#include "physic_manager.h"
#include "game_state_machine.h"
#include "global.h"
#include "game_state.h"

Game::Game(int major, int minor, const std::string &title)
    : m_window(nullptr), m_title(title), m_major(major), m_minor(minor), m_stateMachine(std::make_unique<GameStateMachine>())
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
                                   { glViewport(0, 0, width, height); });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, int button, int action, int mods)
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

            MouseManager::Instance().OnMouseClickEvent(xpos, ypos, isPressed);
        } });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double xpos, double ypos)
                             {
        int winW, winH;
        glfwGetWindowSize(window, &winW, &winH);
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        xpos = xpos * fbW / winW;
        ypos = ypos * fbH / winH;

        MouseManager::Instance().OnMouseMoveEvent(xpos, ypos); });

    // glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::Initialize()
{
    PhysicManager::Instance().Init();
    m_stateMachine->Push(std::make_unique<GSPlay>());
}

void Game::MainLoop()
{
    const float fixedDelta = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window))
    {
        // std::cout << "FPS: " << 1.0f / (glfwGetTime() - lastTime) << "\n";
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += deltaTime;

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);

        int retry = 5;
        while (accumulator >= fixedDelta && retry-- > 0)
        {
            m_stateMachine->Update(fixedDelta, m_window);
            accumulator -= fixedDelta;  
        }
        
        m_stateMachine->Render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}