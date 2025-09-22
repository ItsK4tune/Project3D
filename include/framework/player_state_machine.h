#pragma once
#include "state_machine.h"

class PlayerStateMachine : public StateMachine
{
public:
    void Render() override {};
protected:
    void HandleAction(const StateAction &action) override;
};