#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "object.h"

class HUD : public Object
{
public:
    HUD(const std::string &i, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl,
        std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t);

    void SetOnClick(void (*function)());
    void SetOnHover(void (*function)());
    bool HandleTouchEvent(double x, double y, bool bIsPressed);
    bool HandleHoverEvent(double x, double y);
    bool IsHolding();

private:
    bool isHolding;
    void (*hoverFunc)();
    void (*clickFunc)();
};