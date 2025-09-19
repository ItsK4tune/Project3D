#pragma once
#include "object.h"
#include "physic_manager.h"

class Entity : public Object
{
public:
    Entity(const std::string &i, std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
           const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl);
    virtual ~Entity();

    void SetStatic() { isStatic = true; }

    void AttachRigidDynamic(float density = 1.0f);
    void AttachRigidStatic();
    void DetachRigid();
    void SyncFromPhysX();

    void LockRotation(bool lockX, bool lockY, bool lockZ);
    void LockTranslation(bool lockX, bool lockY, bool lockZ);

    std::vector<glm::bvec3> GetLockedRotation() const;
    std::vector<glm::bvec3> GetLockedTranslation() const;

    virtual void Update(float deltaTime);

protected:
    bool isStatic = false;
    std::vector<PxRigidActor *> actors;
};