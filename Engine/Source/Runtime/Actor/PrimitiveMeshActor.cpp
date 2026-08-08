#include "PrimitiveMeshActor.h"
#include "Runtime/ActorComponents/MeshComponent.h"
#include "Runtime/World.h"
#include "Core/EngineTypes.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(CubeActorLog);

PrimitiveMeshActor::PrimitiveMeshActor(World* world, uint32_t id, const Transform& transform, MeshPrimitive type)
    : Actor(world, id, transform)
{
    if (!world || !world->GetMeshSubsystem())
    {
        VE_LOG(CubeActorLog, Error, "CubeActor construction was failed !");
        return;
    }

    MeshHandle handle = world->GetMeshSubsystem()->GetOrCreatePrimitive(type);
    MeshComponent* mesh = CreateComponent<MeshComponent>(Transform::ZeroTransform());
    mesh->SetMesh(handle);
}

PrimitiveMeshActor::~PrimitiveMeshActor()
{
    //
}
