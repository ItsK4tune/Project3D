#include <iostream>
#include <vector>
#include "physic_manager.h"

PhysicManager& PhysicManager::Instance()
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
    if (gScene)      gScene->release();
    if (gDispatcher) gDispatcher->release();
    if (gPhysics)    gPhysics->release();
    if (gFoundation) gFoundation->release();
}

bool PhysicManager::Init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!gFoundation) return false;

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
    if (!gPhysics) return false;

    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    // gDispatcher = PxDefaultCpuDispatcherCreate(std::max(1u, std::thread::hardware_concurrency() - 1));

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

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

PxPhysics* PhysicManager::GetPhysics()
{
    if (!gPhysics) std::cerr << "[PhysicManager] GetPhysics() -> nullptr\n";
    return gPhysics;
}

PxScene* PhysicManager::GetScene()
{
    if (!gScene) std::cerr << "[PhysicManager] GetScene() -> nullptr\n";
    return gScene;
}
