#include "Actor.h"
#include "Runtime/World.h"
#include "Runtime/ActorComponents/ActorComponent.h"
#include "Runtime/ActorComponents/MeshComponent.h"
#include "Log/Log.h"

// for fun
#include <random>
namespace
{
float RandomRange(float min, float max)
{
    static std::mt19937 generator{std::random_device{}()};
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}
}  // namespace

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogActor);

std::vector<MeshDrawItem> Actor::getMeshesTransforms() const
{
    std::vector<MeshDrawItem> result;

    for (const auto& meshComponent : m_meshComponents)
    {
        MeshDrawItem item;
        item.mesh = meshComponent->GetMesh();
        item.worldMatrix = meshComponent->GetWorldTransformMatrix();
        result.push_back(item);
    }

    return result;
}

Actor::Actor(World* world, uint32_t id, const Transform& transform) : m_world(world), m_uniqueId(id), m_transform(transform)
{
    VE_LOG(LogActor, Display, "Actor was created with {} id", m_uniqueId);

    // for fun
    m_bobAmplitude = RandomRange(0.3f, 0.8f);
    m_bobSpeed = RandomRange(1.0f, 3.0f);
    m_bobPhase = RandomRange(0.0f, 6.2831853f);
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
    for (auto& component : m_components)
    {
        component->BeginPlay();
    }
}

void Actor::Tick(float deltaTime)
{
    const float rotationSpeedDegPerSec = 45.0f;
    m_elapsedTime += deltaTime;
    Math3D::Vec3 location = GetActorLocation();
    location.z = m_baseHeight + std::sin(m_elapsedTime * m_bobSpeed + m_bobPhase) * m_bobAmplitude;
    SetActorLocation(location);

    AddActorRotation({0.f, rotationSpeedDegPerSec * deltaTime, 0.f});

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

void Actor::SetActorTransform(const Transform& transform)
{
    m_transform = transform;
}

void Actor::SetActorLocation(const Math3D::Vec3& location)
{
    m_transform.location = location;
}

void Actor::SetActorRotation(const Math3D::Vec3& rotation)
{
    m_transform.rotation = rotation;
}

void Actor::SetActorScale(const Math3D::Vec3& scale)
{
    m_transform.scale = scale;
}

void Actor::AddActorLocation(const Math3D::Vec3& delta)
{
    m_transform.location += delta;
}

void Actor::AddActorRotation(const Math3D::Vec3& delta)
{
    m_transform.rotation += delta;
}

bool Actor::HasMeshes() const
{
    return !m_meshComponents.empty();
}
