#pragma once

#include <vector>
#include <memory>
#include "Core/EngineTypes.h"
#include "Libraries/Math3D/Math3D.h"

namespace VulkanEngine
{
class ActorComponent;
class MeshComponent;
class World;

class Actor
{
    uint32_t m_uniqueId;
    Transform m_transform;
    std::vector<std::unique_ptr<ActorComponent>> m_components;

    World* m_world = nullptr;

    std::vector<MeshComponent*> m_meshComponents;  // to avoid cast temp

    // temp fun
    float m_baseHeight = 0.f;
    float m_elapsedTime = 0.f;
    float m_bobAmplitude = 0.5f;
    float m_bobSpeed = 2.0f;  // speed
    float m_bobPhase = 0.f;

protected:
    template <typename T, typename... Args>
    T* CreateComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<ActorComponent, T>, "T must be derived from ActorComponent");

        auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* raw = component.get();
        m_components.push_back(std::move(component));

        if constexpr (std::is_base_of_v<MeshComponent, T>)
        {
            m_meshComponents.push_back(raw);
        }

        return raw;
    }

public:
    Actor(World* world, uint32_t id, const Transform& transform);
    virtual ~Actor();

    virtual void BeginPlay();
    virtual void Tick(float deltaTime);

    virtual void Destroy();

    uint32_t GetActorUniqueID() const;

    Transform GetActorTransform() const;
    Math3D::Vec3 GetActorLocation() const;
    Math3D::Vec3 GetActorRotation() const;
    Math3D::Vec3 GetActorScale() const;

    void SetActorTransform(const Transform& transform);
    void SetActorLocation(const Math3D::Vec3& location);
    void SetActorRotation(const Math3D::Vec3& rotation);
    void SetActorScale(const Math3D::Vec3& scale);

    void AddActorLocation(const Math3D::Vec3& delta);
    void AddActorRotation(const Math3D::Vec3& delta);

    bool HasMeshes() const;
    std::vector<MeshDrawItem> getMeshesTransforms() const;

    template <typename T>
    T* GetComponentByClass() const
    {
        for (auto& component : m_components)
        {
            if (T* result = dynamic_cast<T*>(component.get()))
            {
                return result;
            }
        }

        return nullptr;
    }
};

}  // namespace VulkanEngine