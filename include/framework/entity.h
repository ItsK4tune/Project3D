#pragma once
#include "object.h"
#include "physic_manager.h"

class Entity : public Object
{
public:
    Entity(const std::string &i, std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
           const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl);
    virtual ~Entity();

    void AttachRigidDynamic(float density = 1.0f);
    void AttachRigidStatic();
    void SyncFromPhysX();

    virtual void Update(float deltaTime);

protected:
    PxRigidActor *actor;
};