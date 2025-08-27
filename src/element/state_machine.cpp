#include "state_machine.h"
#include "game_state/game_state.h"

void StateMachine::Push(std::unique_ptr<State> state)
{
    states.push(std::move(state));
    states.top()->Enter();
    if (!states.top()->isInitialized)
    {
        states.top()->Init();
        states.top()->isInitialized = true;
    }
}

void StateMachine::Pop()
{
    if (!states.empty())
    {
        states.top()->Exit();
        states.pop();
    }
}

void StateMachine::SwitchTo(std::unique_ptr<State> state)
{
    Pop();
    Push(std::move(state));
}

void StateMachine::Update(float deltaTime, GLFWwindow* window)
{
    if (states.empty())
        return;

    StateAction action = states.top()->Update(deltaTime, window);
    HandleAction(action);
}

void StateMachine::Render()
{
    if (!states.empty())
        states.top()->Render();
    else
        std::cout << "[StateMachine::Render] No state to render." << std::endl;
}

void StateMachine::HandleAction(const StateAction &action)
{
    switch (action.type)
    {
    case StateActionType::Push:
        if (action.nextState == "GSPlay")
        {
            Push(std::make_unique<GSPlay>());
			// std::cout << "[StateMachine::HandleAction] Push PauseState not implemented yet." << std::endl;
        }
        break;
    case StateActionType::Pop:
        Pop();
        break;
    case StateActionType::Switch:
        break;
    case StateActionType::Quit:
        while (!states.empty())
            Pop();
        break;
    default:
        break;
    }
}