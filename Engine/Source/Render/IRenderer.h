#pragma once
#include "Core/GameConfig.h"
#include "Core/EngineTypes.h"
#include "MeshDesc.h"

#include <glm/glm.hpp>

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
    virtual void SetCameraView(const glm::mat4& view) = 0;

    virtual MeshHandle CreateMesh(const MeshDesc& desc) = 0;
    virtual void DestroyMesh(MeshHandle handle) = 0;
};
}  // namespace VulkanEngine