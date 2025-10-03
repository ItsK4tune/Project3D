#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "entity.h"

Entity::Entity(const std::string &i, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl,
               std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t)
    : Object(i, pos, rot, scl, m, s, t) {}

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

    for (auto const &obb : GetModel()->boundingBoxs)
    {
        glm::vec3 center = obb.center;           // model-space center
        glm::vec3 halfSize = obb.halfSize;       // already scaled in ProcessMesh
        glm::mat3 orientation = obb.orientation; // orthonormal rotation matrix

        PxTransform actorPose(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z));
        PxRigidDynamic *actor = physics->createRigidDynamic(actorPose);

        // Ensure extents > eps
        const float eps = 1e-4f;
        PxBoxGeometry geometry(
            std::max(halfSize.x, eps),
            std::max(halfSize.y, eps),
            std::max(halfSize.z, eps));
        PxShape *shape = physics->createShape(geometry, *physics->createMaterial(0.0f, 0.0f, 0.6f));

        // Convert glm::mat3 (orientation) -> glm::quat -> PxQuat
        glm::quat gq = glm::quat_cast(orientation);
        if (glm::length2(gq) < 1e-6f)
            gq = glm::quat(1, 0, 0, 0); // fallback
        gq = glm::normalize(gq);
        PxQuat pxq((float)gq.x, (float)gq.y, (float)gq.z, (float)gq.w);

        // local pose: center in model-space (shape local pose is relative to actor pose which we set to entity pos)
        PxTransform localPose(PxVec3(center.x, center.y, center.z), pxq);

        // sanity check: avoid invalid pose
        if (!localPose.isValid())
        {
            localPose = PxTransform(PxVec3(center.x, center.y, center.z));
        }

        shape->setLocalPose(localPose);

        // filter data
        PxFilterData fd;
        fd.word0 = isGhost ? 1 : 0;
        shape->setSimulationFilterData(fd);

        actor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*actor, density);
        PhysicManager::Instance().GetScene()->addActor(*actor);
        actors.push_back(actor);
    }
}

void Entity::AttachRigidStatic()
{
    PxPhysics *physics = PhysicManager::Instance().GetPhysics();
    if (!physics)
        return;

    for (auto const &obb : GetModel()->boundingBoxs)
    {
        glm::vec3 center = obb.center;
        glm::vec3 halfSize = obb.halfSize;
        glm::mat3 orientation = obb.orientation;

        PxTransform actorPose(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z));
        PxRigidStatic *actor = physics->createRigidStatic(actorPose);

        const float eps = 1e-4f;
        PxBoxGeometry geometry(
            std::max(halfSize.x, eps),
            std::max(halfSize.y, eps),
            std::max(halfSize.z, eps));
        PxShape *shape = physics->createShape(geometry, *physics->createMaterial(0.0f, 0.0f, 0.6f));

        glm::quat gq = glm::quat_cast(orientation);
        if (glm::length2(gq) < 1e-6f)
            gq = glm::quat(1, 0, 0, 0);
        gq = glm::normalize(gq);
        PxQuat pxq((float)gq.x, (float)gq.y, (float)gq.z, (float)gq.w);

        PxTransform localPose(PxVec3(center.x, center.y, center.z), pxq);
        if (!localPose.isValid())
            localPose = PxTransform(PxVec3(center.x, center.y, center.z));

        shape->setLocalPose(localPose);

        PxFilterData fd;
        fd.word0 = isGhost ? 1 : 0;
        shape->setSimulationFilterData(fd);

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
