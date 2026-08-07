#pragma once

namespace VulkanEngine
{
class World;

class WorldSubsystem
{
    World* m_world;

public:
    WorldSubsystem(World* world);
    virtual ~WorldSubsystem();

    virtual void Init();
    virtual void Shutdown();
};

}  // namespace VulkanEngine