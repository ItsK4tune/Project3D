#pragma once
#include <stack>
#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "state.h"

class StateMachine
{
public:
    void Push(std::unique_ptr<State> state);
    void Pop();
    void SwitchTo(std::unique_ptr<State> state);

    void Update(float deltaTime, void* context = nullptr);
    virtual void Render() = 0;

protected:
    std::stack<std::unique_ptr<State>> states;
    virtual void HandleAction(const StateAction &action) = 0;
};