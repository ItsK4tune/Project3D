#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "entity.h"

Entity::Entity(const std::string &i, std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
               const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
    : Object(i, m, s, t, pos, rot, scl), actor(nullptr) {}

Entity::~Entity() {
    if (actor) {
        PhysicManager::Instance().GetScene()->removeActor(*actor);
        actor->release();
        actor = nullptr;
    }
}

void Entity::AttachRigidDynamic(float density) {
    PxPhysics* physics = PhysicManager::Instance().GetPhysics();
    if (!physics) return;

    AABB bbox = GetModel()->boundingBox;
    glm::vec3 halfExtents = (bbox.max - bbox.min) * 0.5f;

    PxTransform transform(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z));
    actor = physics->createRigidDynamic(transform);

    PxShape* shape = physics->createShape(
        PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z),
        *physics->createMaterial(0.5f, 0.5f, 0.6f)
    );
    actor->attachShape(*shape);
    shape->release();

    PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(actor), density);

    PhysicManager::Instance().GetScene()->addActor(*actor);
}


void Entity::AttachRigidStatic() {
    PxPhysics* physics = PhysicManager::Instance().GetPhysics();
    if (!physics) return;

    AABB bbox = GetModel()->boundingBox;
    glm::vec3 halfExtents = (bbox.max - bbox.min) * 0.5f;

    PxTransform transform(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z));
    actor = physics->createRigidStatic(transform);

    PxShape* shape = physics->createShape(
        PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z),
        *physics->createMaterial(0.5f, 0.5f, 0.6f)
    );
    actor->attachShape(*shape);
    shape->release();

    PhysicManager::Instance().GetScene()->addActor(*actor);
}

void Entity::SyncFromPhysX() {
    if (actor) {
        PxTransform t = actor->getGlobalPose();
        SetPosition(glm::vec3(t.p.x, t.p.y, t.p.z));

        PxQuat q = t.q;
        glm::quat gq(q.w, q.x, q.y, q.z);
        glm::vec3 euler = glm::eulerAngles(gq);
        euler = glm::degrees(euler);

        SetRotation(euler);
    }
}

void Entity::Update(float deltaTime) {
    SyncFromPhysX();
}
