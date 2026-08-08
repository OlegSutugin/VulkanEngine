#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "Core/EngineTypes.h"
#include "Libraries/Math3D/Math3D.h"

namespace VulkanEngine
{
class Engine;
class Actor;
class IRenderer;
class MeshSubsystem;

class World final
{
private:
    friend class Engine;
    World(IRenderer* renderer);

    std::unique_ptr<MeshSubsystem> m_meshSubsystem;

    std::vector<std::unique_ptr<Actor>> m_actors;
    std::unordered_map<uint32_t, Actor*> m_actorsById;
    std::vector<uint32_t> m_pendingDestroy;

    uint32_t m_nextActorId = 1;
    void FlushDestroyed();

    IRenderer* m_renderer;

    // only for engine class
    static std::unique_ptr<World> CreateWorld(IRenderer* renderer)  //
    {
        return std::unique_ptr<World>(new World(renderer));
    }

    std::vector<MeshDrawItem> CollectMeshDrawItems() const;

public:
    ~World();

    void Tick(float deltaTime);

    template <typename T, typename... Args>
    T* SpawnActor(Args&&... args)
    {
        static_assert(std::is_base_of_v<Actor, T>, "T must be derived from Actor");

        uint32_t id = m_nextActorId;
        auto actor = std::make_unique<T>(this, id, std::forward<Args>(args)...);
        T* raw = actor.get();
        m_actorsById[id] = raw;
        m_actors.push_back(std::move(actor));

        m_nextActorId++;

        return raw;
    }

    Actor* FindActorById(uint32_t id) const
    {
        auto it = m_actorsById.find(id);
        return it != m_actorsById.end() ? it->second : nullptr;
    }

    void DestroyActor(uint32_t id);

    MeshSubsystem* GetMeshSubsystem() const;
};

}  // namespace VulkanEngine