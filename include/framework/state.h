#pragma once
#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class StateActionType
{
    None,
    Push,
    Pop,
    Switch,
    Quit
};

struct StateAction
{
    StateActionType type = StateActionType::None;
    std::string nextState;
};

class State
{
public:
    virtual ~State() = default;
    virtual void Init() = 0;
    virtual void Enter() = 0;
    virtual StateAction Update(float deltaTime, GLFWwindow* window = nullptr) = 0;
    virtual void Render() = 0;
    virtual void Exit() = 0;
    bool isInitialized = false;
};