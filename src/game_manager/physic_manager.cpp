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

static PxFilterFlags MyFilterShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags &pairFlags, const void *constantBlock, PxU32 constantBlockSize)
{
    bool ghost0 = (filterData0.word0 & 1);
    bool ghost1 = (filterData1.word0 & 1);

    if (ghost0 || ghost1)
    {
        pairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
        return PxFilterFlag::eDEFAULT;
    }

    pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;
    return PxFilterFlag::eDEFAULT;
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
    sceneDesc.filterShader = MyFilterShader;
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