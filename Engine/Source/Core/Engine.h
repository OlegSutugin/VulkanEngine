#pragma once

#include <memory>
#include <string>
#include "GameConfig.h"
#include "Render/MeshDesc.h"
#include "EngineTypes.h"

namespace VulkanEngine
{
class GLFWWindowManager;
class IRenderer;
class Camera;
class Input;

class Engine final
{
public:
    Engine(const GameConfig& inConfig = GameConfig());
    ~Engine();

    void Tick(float deltaTime);
    bool isRunning() const;

    // meshes temp
    MeshHandle CreateMesh(const MeshDesc& desc);
    void DestroyMesh(MeshHandle handle);

private:
    void EngineStop();
    std::unique_ptr<GLFWWindowManager> m_windowManager;
    std::unique_ptr<IRenderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Input> m_input;

    bool m_initialized{false};
    GameConfig m_gameConfig;
};

}  // namespace VulkanEngine