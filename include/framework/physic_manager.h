#pragma once
#include <memory>
#include "PxPhysicsAPI.h"

using namespace physx;

class PhysicManager : public PxSimulationEventCallback
{
public:
    static PhysicManager &Instance();

    PhysicManager(const PhysicManager &) = delete;
    PhysicManager &operator=(const PhysicManager &) = delete;

    bool Init();
    void Update(float deltaTime);

    PxPhysics *GetPhysics();
    PxScene *GetScene();

    void RemoveActor(PxActor *actor);
    void RemoveAllActors();

    void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override {}
    void onWake(PxActor **actors, PxU32 count) override {}
    void onSleep(PxActor **actors, PxU32 count) override {}
    void onContact(const PxContactPairHeader &pairHeader,
                   const PxContactPair *pairs, PxU32 nbPairs) override {}
    void onTrigger(PxTriggerPair *pairs, PxU32 count) override {}
    void onAdvance(const PxRigidBody *const *bodyBuffer,
                   const PxTransform *poseBuffer, const PxU32 count) override {}

private:
    PhysicManager();
    ~PhysicManager();

    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;
    PxFoundation *gFoundation;
    PxPhysics *gPhysics;
    PxDefaultCpuDispatcher *gDispatcher;
    PxScene *gScene;
};
