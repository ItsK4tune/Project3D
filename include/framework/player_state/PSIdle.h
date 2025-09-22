#pragma once
#include "state.h"
#include "player.h"

class PSIdle : public PlayerState
{
public:
    void Init() override;
    void Enter() override;
    void Exit() override;
    StateAction Update(float deltaTime, Player *window = nullptr) override;
};