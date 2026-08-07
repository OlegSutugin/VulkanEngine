#include "World.h"
#include "Core/EngineTypes.h"
#include "Core/Subsystems/WorldSubsystems/MeshSubsystem.h"
#include "Runtime/Actor/Actor.h"
#include "Render/IRenderer.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogWorld);

World::World(IRenderer* renderer) : m_renderer(renderer)
{
    VE_LOG(LogWorld, Display, "World was created!");

    if (!m_renderer)
    {
        VE_LOG(LogWorld, Error, "No renderer during world creation");
    }

    // for linking data and rendering - bad desicion after all :(
    m_meshSubsystem = std::make_unique<MeshSubsystem>(this, m_renderer);
    if (m_meshSubsystem)
    {
        m_meshSubsystem->Init();
    }
}

World::~World()
{
    m_actors.clear();
    m_actorsById.clear();
    m_pendingDestroy.clear();

    if (m_meshSubsystem)
    {
        m_meshSubsystem->Shutdown();
    }

    VE_LOG(LogWorld, Display, "World was destroyed!");
}

void World::FlushDestroyed()
{
    for (uint32_t id : m_pendingDestroy)
    {
        auto it = m_actorsById.find(id);
        if (it == m_actorsById.end())
        {
            continue;  // sanity check
        }

        Actor* target = it->second;

        // Swap-and-pop
        for (size_t i = 0; i < m_actors.size(); ++i)
        {
            if (m_actors[i].get() == target)
            {
                std::swap(m_actors[i], m_actors.back());
                m_actors.pop_back();  // destroying called explicitly cause of unique ptr, pretty handy))
                break;
            }
        }

        m_actorsById.erase(it);
    }

    m_pendingDestroy.clear();
}

void World::Tick(float deltaTime)
{
    for (auto& actor : m_actors)
    {
        if (actor)
        {
            actor->Tick(deltaTime);
        }
    }
}

void World::DestroyActor(uint32_t id)
{
    if (m_actorsById.find(id) == m_actorsById.end())
    {
        return;
    }

    m_pendingDestroy.push_back(id);
}

MeshSubsystem* World::GetMeshSubsystem() const
{
    return m_meshSubsystem.get();
}
