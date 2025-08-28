#pragma once
#include "state.h"

class GSPlay : public State
{
public:
    void Init() override;
    void Enter() override;
    void Exit() override;
    StateAction Update(float deltaTime, GLFWwindow *window = nullptr) override;
    void Render() override;

private:
    bool m_firstMouse = true;
    double m_lastX = 0.0, m_lastY = 0.0;
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;

    bool m_isCursorDisabled = false;
};