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
    std::string nextState = "None";
};

class State {
public:
    virtual ~State() = default;
    virtual void Init() = 0;
    virtual void Enter() = 0;
    virtual void Render() {};
    virtual void Exit() = 0;

    virtual StateAction UpdateGame(float deltaTime, void* context) = 0;

    bool isInitialized = false;
};

class GameState : public State {
public:
    StateAction UpdateGame(float deltaTime, void* context) override {
        auto window = static_cast<GLFWwindow*>(context);
        return Update(deltaTime, window);
    }

    virtual StateAction Update(float deltaTime, GLFWwindow* window) = 0;
};

class Player;

class PlayerState : public State {
public:
    StateAction UpdateGame(float deltaTime, void* context) override {
        auto player = static_cast<Player*>(context);
        return Update(deltaTime, player);
    }

    virtual StateAction Update(float deltaTime, Player* player) = 0;
};