#include "player_state_machine.h"
#include "player_state.h"

void PlayerStateMachine::HandleAction(const StateAction &action)
{
    switch (action.type)
    {
    case StateActionType::None:
        break;
    case StateActionType::Push:
        /*if (action.nextState == "PSMove")
        {
            Push(std::make_unique<PSMove>());
            
        } else
        {
            std::cout << "[PlayerStateMachine::HandleAction] StateActionType::Push: Not implemented yet." << std::endl;
        }*/
        break;
    case StateActionType::Pop:
        Pop();
        break;
    case StateActionType::Switch:
        std::cout << "[PlayerStateMachine::HandleAction] StateActionType::Switch: Not implemented yet." << std::endl;
        break;
    case StateActionType::Quit:
        while (!states.empty())
        {
            Pop();
        }
        break;
    default:
        std::cout << "[PlayerStateMachine::HandleAction] Unknown action type." << std::endl;
        break;
    }
}