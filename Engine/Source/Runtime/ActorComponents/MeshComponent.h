#pragma once

#include "TransformableComponent.h"
#include "Core/EngineTypes.h"

namespace VulkanEngine
{
class MeshComponent : public TransformableComponent
{
protected:
    MeshHandle m_mesh;

public:
    MeshComponent(Actor* owner, const Transform& transform);
    virtual ~MeshComponent();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    MeshHandle GetMesh() const;
    void SetMesh(const MeshHandle& mesh);
};

}  // namespace VulkanEngine