#include "MeshComponent.h"
#include "Runtime/Actor/Actor.h"

using namespace VulkanEngine;

MeshComponent::MeshComponent(Actor* owner, const Transform& transform) : TransformableComponent(owner, transform)
{
    //
}

MeshComponent::~MeshComponent()
{
    //
}

void MeshComponent::BeginPlay()
{
    //
}

void MeshComponent::Tick(float deltaTime)
{
    //
}

MeshHandle MeshComponent::GetMesh() const
{
    return m_mesh;
}

void MeshComponent::SetMesh(const MeshHandle& mesh)
{
    m_mesh = mesh;
}
