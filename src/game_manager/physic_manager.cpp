#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include "physic_manager.h"

PhysicManager &PhysicManager::Instance()
{
    static PhysicManager instance;
    return instance;
}

PhysicManager::PhysicManager()
    : gFoundation(nullptr),
      gPhysics(nullptr),
      gDispatcher(nullptr),
      gScene(nullptr)
{
}

PhysicManager::~PhysicManager()
{
    if (gScene)
        gScene->release();
    if (gDispatcher)
        gDispatcher->release();
    if (gPhysics)
        gPhysics->release();
    if (gFoundation)
        gFoundation->release();
}

bool PhysicManager::Init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!gFoundation)
        return false;

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
    if (!gPhysics)
        return false;

    gDispatcher = PxDefaultCpuDispatcherCreate(std::max(1u, std::thread::hardware_concurrency() - 1));

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    sceneDesc.simulationEventCallback = this;

    gScene = gPhysics->createScene(sceneDesc);
    return (gScene != nullptr);
}

void PhysicManager::Update(float deltaTime)
{
    if (gScene)
    {
        gScene->simulate(deltaTime);
        gScene->fetchResults(true);
    }
}

PxPhysics *PhysicManager::GetPhysics()
{
    if (!gPhysics)
        std::cerr << "[PhysicManager] GetPhysics() -> nullptr\n";
    return gPhysics;
}

PxScene *PhysicManager::GetScene()
{
    if (!gScene)
        std::cerr << "[PhysicManager] GetScene() -> nullptr\n";
    return gScene;
}

void PhysicManager::RemoveActor(PxActor *actor)
{
    if (!gScene || !actor)
        return;

    gScene->removeActor(*actor);
    actor->release();
    std::cout << "[PhysX] Removed actor\n";
}

void PhysicManager::RemoveAllActors()
{
    if (!gScene)
        return;

    PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    if (nbActors == 0)
        return;

    std::vector<PxActor *> actors(nbActors);
    gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors.data(), nbActors);

    for (PxActor *actor : actors)
    {
        gScene->removeActor(*actor);
        actor->release();
    }

    std::cout << "[PhysX] Removed " << nbActors << " actors\n";
}

void PhysicManager::onTrigger(PxTriggerPair *pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; i++)
    {
        const PxTriggerPair &p = pairs[i];

        if (p.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            std::cout << "[PhysX] Trigger ENTER: "
                      << p.triggerActor << " với " << p.otherActor << "\n";
        }
        if (p.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
        {
            std::cout << "[PhysX] Trigger EXIT: "
                      << p.triggerActor << " với " << p.otherActor << "\n";
        }
    }
}