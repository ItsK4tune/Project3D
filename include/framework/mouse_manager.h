#pragma once
#include <vector>
#include <memory>
#include "hud.h"

class MouseManager
{
public:
    int x, y;
    bool isPressed = false;
    static MouseManager &Instance();

    void AddElement(std::shared_ptr<HUD> element);
    void ClearElement();
    void OnMouseClickEvent(GLint x, GLint y, bool isPressed);
    void OnMouseMoveEvent(GLint x, GLint y);

private:
    std::vector<std::shared_ptr<HUD>> elements;

    MouseManager() = default;
    MouseManager(const MouseManager &) = delete;
    MouseManager &operator=(const MouseManager &) = delete;
};