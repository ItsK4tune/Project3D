#include "physic_object.h"

PhysicObject::PhysicObject(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
                           const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Entity(m, s, t, pos, rot, scl)
{
}

void PhysicObject::Update(float deltaTime, const std::vector<std::shared_ptr<Entity>> &others)
{
    SetVelocity(GetVelocity() + GetAccel() * deltaTime);

    if (glm::length(GetVelocity()) > GetMaxSpeed())
    {
        SetVelocity(glm::normalize(GetVelocity()) * GetMaxSpeed());
    }

    if (glm::length(GetVelocity()) > 0.0f)
    {
        glm::vec3 frictionForce = -glm::normalize(GetVelocity()) * GetFriction() * deltaTime;
        if (glm::length(frictionForce) > glm::length(GetVelocity()))
        {
            SetVelocity(glm::vec3(0.0f));
        }
        else
        {
            SetVelocity(GetVelocity() + frictionForce);
        }
    }

    glm::vec3 newPos = GetPosition() + GetVelocity() * deltaTime;

    bool collided = false;
    for (const auto &other : others)
    {
        if (other.get() == this)
            continue;
        glm::vec3 oldPos = GetPosition();
        SetPosition(newPos);
        if (NarrowPhaseCheck(*other))
        {
            collided = true;
            SetPosition(oldPos);
            SetVelocity(glm::vec3(0.0f));
            break;
        }
        SetPosition(oldPos);
    }
    if (!collided)
    {
        SetPosition(newPos);
    }

    SetAccel(glm::vec3(0.0f, -1.0f, 0.0f));
}