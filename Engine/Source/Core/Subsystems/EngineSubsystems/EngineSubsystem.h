#pragma once

namespace VulkanEngine
{
class EngineSubsystem
{
public:
    EngineSubsystem();
    virtual ~EngineSubsystem();

    virtual void Init();
    virtual void Shutdown();
};

}  // namespace VulkanEngine
