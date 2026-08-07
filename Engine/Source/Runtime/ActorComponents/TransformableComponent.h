#pragma once

#include "ActorComponent.h"
#include "Core/EngineTypes.h"

namespace VulkanEngine
{
class TransformableComponent : public ActorComponent
{
protected:
    Transform m_transform;

public:
    virtual void BeginPlay();
    virtual void Tick(float deltaTime);

    TransformableComponent(Actor* owner, const Transform& transform);
    virtual ~TransformableComponent();

    Transform GetRelativeTransform() const;

    Math3D::Mat4 GetWorldTransformMatrix() const;
    Math3D::Vec3 GetRelativeLocation() const;
    Math3D::Vec3 GetWorldLocation() const;
    Math3D::Vec3 GetRelativeRotation() const;
    Math3D::Vec3 GetRelativeScale() const;
};

}  // namespace VulkanEngine