#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "object.h"

class HUD : public Object
{
public:
    HUD(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
              const glm::vec3 &pos, const glm::vec3&rot, const glm::vec3&scl);

    void SetOnClick(void (*function)());
    void SetOnHover(void (*function)());
    bool HandleTouchEvent(int x, int y, bool bIsPressed);
    bool HandleHoverEvent(int x, int y);
    bool IsHolding();

private:
    bool isHolding;
    void (*hoverFunc)();
    void (*clickFunc)();

    
};