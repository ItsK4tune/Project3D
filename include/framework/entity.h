#pragma once
#include "object.h"

class Entity : public Object
{
public:
    Entity(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
           const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl);

    const glm::vec3 &GetAccel() const { return accel; }
    const glm::vec3 &GetVelocity() const { return velocity; }
    float GetMaxSpeed() const { return maxSpeed; }
    float GetFriction() const { return friction; }

    void SetAccel(const glm::vec3 &a) { accel = a; }
    void SetVelocity(const glm::vec3 &v) { velocity = v; }
    void SetMaxSpeed(float ms) { maxSpeed = ms; }
    void SetFriction(float f) { friction = f; }

    virtual void Update(float deltaTime, const std::vector<std::shared_ptr<Entity>>& others) = 0;

    bool BroadPhaseCheck(const Entity &other) const;
    bool NarrowPhaseCheck(const Entity &other, float deltaTime);

private:
    glm::vec3 previousPosition;

    glm::vec3 accel;
    glm::vec3 velocity;
    float maxSpeed;
    float friction;
};