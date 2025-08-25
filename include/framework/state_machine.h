#pragma once
#include <stack>
#include <memory>
#include <string>
#include "state.h"

class StateMachine
{
    std::stack<std::unique_ptr<State>> states;

public:
    void Push(std::unique_ptr<State> state);
    void Pop();
    void SwitchTo(std::unique_ptr<State> state);

    void Update();
    void Render();

private:
    void HandleAction(const StateAction &action);
};