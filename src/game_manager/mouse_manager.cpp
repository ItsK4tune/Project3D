#include "mouse_manager.h"
#include "global.h"

MouseManager &MouseManager::Instance()
{
    static MouseManager instance;
    return instance;
}

void MouseManager::AddElement(std::shared_ptr<HUD> obj)
{
    elements.push_back(obj);
}

void MouseManager::ClearElement()
{
    elements.clear();
}

void MouseManager::OnMouseClickEvent(double x, double y, bool isPressed)
{
    this->x = x;
    this->y = y;
    this->isPressed = isPressed;
    GLint flippedY = Global::ScreenHeight - y;
    for (auto it = elements.rbegin(); it != elements.rend(); ++it)
    {
        if ((*it)->HandleTouchEvent(x, flippedY, isPressed))
        {
            break;
        }
    }
}

void MouseManager::OnMouseMoveEvent(double x, double y)
{   
    this->x = x;
    this->y = y;
    GLint flippedY = Global::ScreenHeight - y;
    for (auto it = elements.rbegin(); it != elements.rend(); ++it)
    {
        if ((*it)->HandleHoverEvent(x, flippedY))
        {
            break;
        }
    }
}
