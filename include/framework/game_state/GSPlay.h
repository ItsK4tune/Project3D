#pragma once
#include "state.h"

class GSplay : public State
{
public:
    void Init() override;
    void Enter() override;
    void Exit() override;
    StateAction Update(float deltaTime, GLFWwindow* window = nullptr) override;
    void Render() override;
};