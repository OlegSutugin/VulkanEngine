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
    std::vector<std::unique_ptr<Actor>> m_children;

    World* m_world = nullptr;

protected:
    template <typename T, typename... Args>
    T* CreateComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<ActorComponent, T>, "T must be derived from ActorComponent");

        auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* raw = component.get();
        m_components.push_back(std::move(component));
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