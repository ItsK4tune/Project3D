#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include <string>

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
    virtual void Enter() = 0;
    virtual StateAction Update() = 0;
    virtual void Render() = 0;
    virtual void Exit() = 0;
};