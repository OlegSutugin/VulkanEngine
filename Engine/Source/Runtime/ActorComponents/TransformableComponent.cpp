#include "TransformableComponent.h"
#include "Log/Log.h"
#include "Runtime/Actor/Actor.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogTransformableComponent);

TransformableComponent::TransformableComponent(Actor* owner, const Transform& transform) : ActorComponent(owner), m_transform(transform)
{
    //
}

TransformableComponent::~TransformableComponent()
{
    //
}

Transform TransformableComponent::GetRelativeTransform() const
{
    return m_transform;
}

Math3D::Mat4 VulkanEngine::TransformableComponent::GetWorldTransformMatrix() const
{
    if (!GetOwner())
    {
        VE_LOG(LogTransformableComponent, Critical, "No owner on transformable component");
        return Math3D::Mat4::Identity();
    }

    Math3D::Mat4 ownerWorld = GetOwner()->GetActorTransform().ToMatrix();
    Math3D::Mat4 local = m_transform.ToMatrix();
    return ownerWorld * local;
}

Math3D::Vec3 TransformableComponent::GetRelativeLocation() const
{
    return m_transform.location;
}

Math3D::Vec3 TransformableComponent::GetWorldLocation() const
{
    Math3D::Mat4 m = GetWorldTransformMatrix();
    return Math3D::Vec3{m.m[3][0], m.m[3][1], m.m[3][2]};
}

Math3D::Vec3 TransformableComponent::GetRelativeRotation() const
{
    return m_transform.rotation;
}

Math3D::Vec3 TransformableComponent::GetRelativeScale() const
{
    return m_transform.scale;
}

void TransformableComponent::BeginPlay()
{
    //
}

void TransformableComponent::Tick(float deltaTime)
{
    //
}