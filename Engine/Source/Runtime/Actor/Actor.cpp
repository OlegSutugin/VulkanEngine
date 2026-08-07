#include "Actor.h"
#include "Runtime/World.h"
#include "Runtime/ActorComponents/ActorComponent.h"
#include "Runtime/ActorComponents/MeshComponent.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogActor);

Actor::Actor(World* world, uint32_t id, const Transform& transform) : m_world(world), m_uniqueId(id), m_transform(transform)
{
    VE_LOG(LogActor, Display, "Actor was created with {} id", m_uniqueId);
}

Actor::~Actor()
{
    VE_LOG(LogActor, Display, "Actor  with {} id was destroyed", m_uniqueId);
}

void Actor::Destroy()
{
    if (m_world)
    {
        m_world->DestroyActor(m_uniqueId);
    }
    else
    {
        VE_LOG(LogActor, Error, "Something goes wrong!!! no world at {} actor", m_uniqueId);
    }
}

void Actor::BeginPlay()
{
    for (auto& child : m_children)
    {
        child->BeginPlay();
    }

    for (auto& component : m_components)
    {
        component->BeginPlay();
    }
}

void Actor::Tick(float deltaTime)
{
    for (auto& child : m_children)
    {
        child->Tick(deltaTime);
    }

    for (auto& component : m_components)
    {
        component->Tick(deltaTime);
    }
}

uint32_t Actor::GetActorUniqueID() const
{
    return m_uniqueId;
}

Transform Actor::GetActorTransform() const
{
    return m_transform;
}

Math3D::Vec3 Actor::GetActorLocation() const
{
    return m_transform.location;
}

Math3D::Vec3 Actor::GetActorRotation() const
{
    return m_transform.rotation;
}

Math3D::Vec3 Actor::GetActorScale() const
{
    return m_transform.scale;
}
