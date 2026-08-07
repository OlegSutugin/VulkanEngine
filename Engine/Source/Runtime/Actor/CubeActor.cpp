#include "CubeActor.h"
#include "Runtime/ActorComponents/MeshComponent.h"
#include "Core/Subsystems/WorldSubsystems/MeshSubsystem.h"
#include "Runtime/World.h"
#include "Core/EngineTypes.h"
#include "Log/log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(CubeActorLog);

CubeActor::CubeActor(World* world, uint32_t id, const Transform& transform) : Actor(world, id, transform)
{
    if (!world || !world->GetMeshSubsystem())
    {
        VE_LOG(CubeActorLog, Error, "CubeActor construction was failed !");
        return;
    }

    MeshHandle handle = world->GetMeshSubsystem()->GetOrCreatePrimitive(MeshPrimitive::Cube);
    MeshComponent* mesh = CreateComponent<MeshComponent>(Transform::ZeroTransform());
    mesh->SetMesh(handle);
}

CubeActor::~CubeActor()
{
    //
}
