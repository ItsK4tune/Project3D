#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "entity.h"

static inline AABB TransformAABB(const AABB &local, const glm::mat4 &transform)
{
    glm::vec3 corners[8] = {
        {local.min.x, local.min.y, local.min.z},
        {local.min.x, local.min.y, local.max.z},
        {local.min.x, local.max.y, local.min.z},
        {local.min.x, local.max.y, local.max.z},
        {local.max.x, local.min.y, local.min.z},
        {local.max.x, local.min.y, local.max.z},
        {local.max.x, local.max.y, local.min.z},
        {local.max.x, local.max.y, local.max.z}};

    glm::vec3 newMin(std::numeric_limits<float>::max());
    glm::vec3 newMax(-std::numeric_limits<float>::max());

    for (int i = 0; i < 8; i++)
    {
        glm::vec4 worldPos = transform * glm::vec4(corners[i], 1.0f);
        glm::vec3 p = glm::vec3(worldPos);

        newMin = glm::min(newMin, p);
        newMax = glm::max(newMax, p);
    }

    return {newMin, newMax};
}

Entity::Entity(const std::string &i, std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
               const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Object(i, m, s, t, pos, rot, scl) {}

Entity::~Entity()
{
    if (actors.size())
    {
        for (auto actor : actors)
        {
            PhysicManager::Instance().RemoveActor(actor);
            actor->release();
            actor = nullptr;
        }
    }
}

void Entity::AttachRigidDynamic(float density)
{
    PxPhysics *physics = PhysicManager::Instance().GetPhysics();
    if (!physics)
        return;

    for (auto const &boundingBox : GetModel()->boundingBoxs)
    {
        PxRigidActor *actor;
        AABB bbox = TransformAABB(boundingBox, GetWorldMatrix());
        glm::vec3 center = (bbox.min + bbox.max) * 0.5f;
        glm::vec3 halfExtents = (bbox.max - bbox.min) * 0.5f;

        PxTransform transform(PxVec3(center.x, center.y, center.z));
        actor = physics->createRigidDynamic(transform);

        PxShape *shape = physics->createShape(
            PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z),
            *physics->createMaterial(0.0f, 0.0f, 0.6f));
        actor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic *>(actor), density);

        PhysicManager::Instance().GetScene()->addActor(*actor);
        actors.push_back(actor);
    }
}

void Entity::AttachRigidStatic()
{
    PxPhysics *physics = PhysicManager::Instance().GetPhysics();
    if (!physics)
        return;

    for (auto const &boundingBox : GetModel()->boundingBoxs)
    {
        PxRigidActor *actor;
        if (GetID() == "O_MAP")
            std::cout << "AABB min(" << boundingBox.min.x << ", " << boundingBox.min.y << ", " << boundingBox.min.z << ") max (" << boundingBox.max.x << ", " << boundingBox.max.y << ", " << boundingBox.max.z << ")\n";
        AABB bbox = TransformAABB(boundingBox, GetWorldMatrix());
        glm::vec3 center = (bbox.min + bbox.max) * 0.5f;
        glm::vec3 halfExtents = (bbox.max - bbox.min) * 0.5f;

        PxTransform transform(PxVec3(center.x, center.y, center.z));
        actor = physics->createRigidStatic(transform);

        PxShape *shape = physics->createShape(
            PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z),
            *physics->createMaterial(0.0f, 0.0f, 0.6f));
        actor->attachShape(*shape);
        shape->release();

        PhysicManager::Instance().GetScene()->addActor(*actor);
        actors.push_back(actor);
    }
}

void Entity::DetachRigid()
{
    if (actors.size())
        for (auto actor : actors)
            PhysicManager::Instance().RemoveActor(actor);
}

void Entity::SyncFromPhysX()
{
    if (actors.size())
        for (auto actor : actors)
        {
            PxTransform t = actor->getGlobalPose();
            SetPosition(glm::vec3(t.p.x, t.p.y, t.p.z));

            PxQuat q = t.q;
            glm::quat gq(q.w, q.x, q.y, q.z);
            glm::vec3 euler = glm::eulerAngles(gq);
            euler = glm::degrees(euler);

            SetRotation(euler);
        }
}

void Entity::LockRotation(bool lockX, bool lockY, bool lockZ)
{
    if (!actors.size())
        return;

    for (auto actor : actors)
    {
        PxRigidDynamic *dyn = actor->is<PxRigidDynamic>();
        if (!dyn)
            return;

        dyn->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lockX);
        dyn->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lockY);
        dyn->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lockZ);
    }
}

void Entity::LockTranslation(bool lockX, bool lockY, bool lockZ)
{
    if (!actors.size())
        return;

    for (auto actor : actors)
    {
        PxRigidDynamic *dyn = actor->is<PxRigidDynamic>();
        if (!dyn)
            return;

        dyn->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lockX);
        dyn->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lockY);
        dyn->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lockZ);
    }
}

std::vector<glm::bvec3> Entity::GetLockedRotation() const
{
    std::vector<glm::bvec3> status;
    for (auto actor : actors)
    {
        const PxRigidDynamic *dyn = actor->is<PxRigidDynamic>();
        if (!dyn)
            return status;

        PxRigidDynamicLockFlags flags = dyn->getRigidDynamicLockFlags();
        status.push_back(glm::bvec3(
            flags.isSet(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X),
            flags.isSet(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y),
            flags.isSet(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z)));
    }

    return status;
}

std::vector<glm::bvec3> Entity::GetLockedTranslation() const
{
    std::vector<glm::bvec3> status;
    for (auto actor : actors)
    {
        const PxRigidDynamic *dyn = actor->is<PxRigidDynamic>();
        if (!dyn)
            return status;

        PxRigidDynamicLockFlags flags = dyn->getRigidDynamicLockFlags();
        status.push_back(glm::bvec3(
            flags.isSet(PxRigidDynamicLockFlag::eLOCK_LINEAR_X),
            flags.isSet(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y),
            flags.isSet(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z)));
    }

    return status;
}

void Entity::Update(float deltaTime)
{
    if (!isStatic)
        SyncFromPhysX();
}
