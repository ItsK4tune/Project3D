#include "game_state_machine.h"
#include "game_state.h"

void GameStateMachine::Render()
{
    if (!states.empty())
        states.top()->Render();
    else
        std::cout << "[GameStateMachine::Render] No state to render." << std::endl;
}

void GameStateMachine::HandleAction(const StateAction &action)
{
    switch (action.type)
    {
    case StateActionType::None:
        break;
    case StateActionType::Push:
        if (action.nextState == "GSPlay")
        {
            Push(std::make_unique<GSPlay>());
            
        } else
        {
            std::cout << "[GameStateMachine::HandleAction] StateActionType::Push: Not implemented yet." << std::endl;
        }
        break;
    case StateActionType::Pop:
        Pop();
        break;
    case StateActionType::Switch:
        std::cout << "[GameStateMachine::HandleAction] StateActionType::Switch: Not implemented yet." << std::endl;
        break;
    case StateActionType::Quit:
        while (!states.empty())
        {
            Pop();
        }
        break;
    default:
        std::cout << "[GameStateMachine::HandleAction] Unknown action type." << std::endl;
        break;
    }
}