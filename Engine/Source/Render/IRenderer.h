#pragma once

namespace VulkanEngine
{

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual void Init() = 0;
    virtual void RegisterWindow(int windowId, void* nativeWindowHandle) = 0;
    virtual void UnregisterWindow(int windowId) = 0;
    virtual void DrawFrame() = 0;
    virtual void Shutdown() = 0;
};
}  // namespace VulkanEngine