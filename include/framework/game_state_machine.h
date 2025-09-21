#pragma once
#include "state_machine.h"

class GameStateMachine : public StateMachine
{
public:
    void Render() override;
protected:
    void HandleAction(const StateAction &action) override;
};