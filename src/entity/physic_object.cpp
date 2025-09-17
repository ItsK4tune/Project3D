#include "physic_object.h"

PhysicObject::PhysicObject(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
                           const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Entity(m, s, t, pos, rot, scl)
{
}

void PhysicObject::Update(float deltaTime, const std::vector<std::shared_ptr<Entity>> &others)
{
    SetVelocity(GetVelocity() + GetAccel() * deltaTime);

    // std::cout << "Velocity: (" << GetVelocity().x << ", " << GetVelocity().y << ", " << GetVelocity().z << ")\n";

    // clamp speed
    float speed = glm::length(GetVelocity());
    if (speed > GetMaxSpeed() && speed > 0.0f)
        SetVelocity(glm::normalize(GetVelocity()) * GetMaxSpeed());

    // friction (simple)
    if (glm::length(GetVelocity()) > 0.0f)
    {
        glm::vec3 frictionForce = -glm::normalize(GetVelocity()) * GetFriction() * deltaTime;
        if (glm::length(frictionForce) > glm::length(GetVelocity()))
            SetVelocity(glm::vec3(0.0f));
        else
            SetVelocity(GetVelocity() + frictionForce);
    }

    // build list of nearby entities using broadphase
    for (const auto &other : others)
    {
        if (!other)
            continue;
        if (other.get() == this)
            continue;
        if (!BroadPhaseCheck(*other))
        {
            SetPosition(GetPosition() + GetVelocity() * deltaTime);
            continue;
        }
        
        NarrowPhaseCheck(*other, deltaTime);
    }

    SetAccel(glm::vec3(0.0f, -0.098f, 0.0f));
}