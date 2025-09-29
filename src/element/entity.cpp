#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "entity.h"

// static inline AABB TransformAABB(const AABB &local, const glm::mat4 &transform)
// {
//     glm::vec3 corners[8] = {
//         {local.min.x, local.min.y, local.min.z},
//         {local.min.x, local.min.y, local.max.z},
//         {local.min.x, local.max.y, local.min.z},
//         {local.min.x, local.max.y, local.max.z},
//         {local.max.x, local.min.y, local.min.z},
//         {local.max.x, local.min.y, local.max.z},
//         {local.max.x, local.max.y, local.min.z},
//         {local.max.x, local.max.y, local.max.z}};

//     glm::vec3 newMin(std::numeric_limits<float>::max());
//     glm::vec3 newMax(-std::numeric_limits<float>::max());

//     for (int i = 0; i < 8; i++)
//     {
//         glm::vec4 worldPos = transform * glm::vec4(corners[i], 1.0f);
//         glm::vec3 p = glm::vec3(worldPos);

//         newMin = glm::min(newMin, p);
//         newMax = glm::max(newMax, p);
//     }

//     return {newMin, newMax};
// }

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

    for (auto const &obb : GetModel()->boundingBoxs)
    {
        // Lấy center và halfSize từ OBB local
        glm::vec3 center = obb.center;
        glm::vec3 halfSize = obb.halfSize;
        glm::mat3 orientation = obb.orientation;

        // Actor world pose (Entity position)
        PxTransform actorPose(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z));
        PxRigidDynamic *actor = physics->createRigidDynamic(actorPose);

        // PhysX shape = BoxGeometry
        PxBoxGeometry geometry(
            std::max(halfSize.x, 0.001f),
            std::max(halfSize.y, 0.001f),
            std::max(halfSize.z, 0.001f));
        PxShape *shape = physics->createShape(geometry, *physics->createMaterial(0.0f, 0.0f, 0.6f));

        // Convert glm::mat3 orientation + glm::vec3 center => PxTransform local pose
        PxMat33 pxOrientation(
            PxVec3(orientation[0][0], orientation[1][0], orientation[2][0]),
            PxVec3(orientation[0][1], orientation[1][1], orientation[2][1]),
            PxVec3(orientation[0][2], orientation[1][2], orientation[2][2]));

        PxTransform localPose(PxVec3(center.x, center.y, center.z));
        localPose.q = PxQuat(pxOrientation); // gán orientation

        shape->setLocalPose(localPose);

        // Gán filter data (ghost flag)
        PxFilterData fd;
        fd.word0 = isGhost ? 1 : 0;
        shape->setSimulationFilterData(fd);

        // Attach
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
        // Lấy center và halfSize từ OBB local
        glm::vec3 center = obb.center;
        glm::vec3 halfSize = obb.halfSize;
        glm::mat3 orientation = obb.orientation;

        // Actor world pose (Entity position)
        PxTransform actorPose(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z));
        PxRigidStatic *actor = physics->createRigidStatic(actorPose);

        // PhysX shape = BoxGeometry
        PxBoxGeometry geometry(
            std::max(halfSize.x, 0.001f),
            std::max(halfSize.y, 0.001f),
            std::max(halfSize.z, 0.001f));
        PxShape *shape = physics->createShape(geometry, *physics->createMaterial(0.0f, 0.0f, 0.6f));

        // Convert glm::mat3 orientation + glm::vec3 center => PxTransform local pose
        PxMat33 pxOrientation(
            PxVec3(orientation[0][0], orientation[1][0], orientation[2][0]),
            PxVec3(orientation[0][1], orientation[1][1], orientation[2][1]),
            PxVec3(orientation[0][2], orientation[1][2], orientation[2][2]));

        PxTransform localPose(PxVec3(center.x, center.y, center.z));
        localPose.q = PxQuat(pxOrientation); // gán orientation

        shape->setLocalPose(localPose);

        // Gán filter data (ghost flag)
        PxFilterData fd;
        fd.word0 = isGhost ? 1 : 0;
        shape->setSimulationFilterData(fd);

        // Attach
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
