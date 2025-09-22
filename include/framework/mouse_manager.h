#pragma once
#include <vector>
#include <memory>
#include "hud.h"

class MouseManager
{
public:
    double x, y;
    double last_x, last_y;
    bool isPressed = false;
    static MouseManager &Instance();

    void AddElement(std::shared_ptr<HUD> element);
    void ClearElement();
    void OnMouseClickEvent(double x, double y, bool isPressed);
    void OnMouseMoveEvent(double x, double y);

private:
    std::vector<std::shared_ptr<HUD>> elements;

    MouseManager() = default;
    MouseManager(const MouseManager &) = delete;
    MouseManager &operator=(const MouseManager &) = delete;
};