#pragma once
#include "state_machine.h"

class GameStateMachine : public StateMachine
{
protected:
    void HandleAction(const StateAction &action) override;
};