#pragma once
#include "Core/GameConfig.h"

namespace VulkanEngine
{

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual void Init(const GameConfig& config) = 0;
    virtual void RegisterWindow(int windowId, void* nativeWindowHandle) = 0;
    virtual void UnregisterWindow(int windowId) = 0;
    virtual void DrawFrame() = 0;
    virtual void Shutdown() = 0;
    virtual void WindowWasResized(int id, int newWidth, int newHeight) = 0;
};
}  // namespace VulkanEngine