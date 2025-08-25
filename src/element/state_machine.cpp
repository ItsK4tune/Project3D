#include "state_machine.h"

void StateMachine::Push(std::unique_ptr<State> state)
{
    states.push(std::move(state));
    states.top()->Enter();
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

void StateMachine::Update()
{
    if (states.empty())
        return;

    StateAction action = states.top()->Update();
    HandleAction(action);
}

void StateMachine::Render()
{
    if (!states.empty())
        states.top()->Render();
}

void StateMachine::HandleAction(const StateAction &action)
{
    switch (action.type)
    {
    case StateActionType::Push:
        if (action.nextState == "Pause")
        {
            //Push(std::make_unique<PauseState>());
			std::cout << "[StateMachine::HandleAction] Push PauseState not implemented yet." << std::endl;
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