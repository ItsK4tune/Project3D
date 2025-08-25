#include "hud.h"
#include "scene_manager.h"

HUD::HUD(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Object(m, s, t, pos, rot, scl), isHolding(false), clickFunc(nullptr), hoverFunc(nullptr) {}

void HUD::SetOnClick(void (*function)())
{
    clickFunc = function;
}

void HUD::SetOnHover(void (*function)())
{
    hoverFunc = function;
}

bool HUD::HandleTouchEvent(int x, int y, bool bIsPressed)
{
    bool isHandled = false;

    auto hudPos = GetPosition();
    auto hudScale = GetScale();

    float left   = hudPos.x - hudScale.x;
    float right  = hudPos.x + hudScale.x;
    float bottom = hudPos.y - hudScale.y;
    float top    = hudPos.y + hudScale.y;

    if (x >= left && x <= right && y >= bottom && y <= top)
    {
        isHandled = true;
        if (bIsPressed)
        {
            isHolding = true;
            if (clickFunc)
                clickFunc();
        }
        else
        {
            isHolding = false;
        }
    }
    else
    {
        if (!bIsPressed)
            isHolding = false;
    }

    return isHandled;
}

bool HUD::HandleHoverEvent(int x, int y)
{
    bool isHandled = false;

    auto hudPos = GetPosition();
    auto hudScale = GetScale();

    float left   = hudPos.x - hudScale.x;
    float right  = hudPos.x + hudScale.x;
    float bottom = hudPos.y - hudScale.y;
    float top    = hudPos.y + hudScale.y;

    if (x >= left && x <= right && y >= bottom && y <= top)
    {
        isHandled = true;
        if (hoverFunc)
            hoverFunc();
    }

    return isHandled;
}

bool HUD::IsHolding()
{
    return isHolding;
}
