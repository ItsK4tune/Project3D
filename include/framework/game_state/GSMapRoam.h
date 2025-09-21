#pragma once
#include "state.h"
#include <glm/glm.hpp>

class GSMapRoam : public GameState
{
public:
    void Init() override;
    void Enter() override;
    void Exit() override;
    StateAction Update(float deltaTime, GLFWwindow *window = nullptr) override;
    void Render() override;

private:
    bool isPressed = false;
    glm::vec3 newPos;
};