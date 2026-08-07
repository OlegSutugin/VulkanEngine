#pragma once

#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include "GameConfig.h"
#include "Libraries/Geometry/MeshDescriptor.h"
#include "EngineTypes.h"

namespace VulkanEngine
{
class GLFWWindowManager;
class GLFWWindow;
class IRenderer;
class World;
class Camera;
class Input;
class MeshSubsystem;

class Engine final
{
public:
    Engine(const GameConfig& inConfig = GameConfig());
    ~Engine();

    void Tick(float deltaTime);
    bool isRunning() const;

    World* GetWorld() const;

private:
    void EngineStop();
    void RegisterWindowViewport(int windowId, std::shared_ptr<GLFWWindow> window);

    std::unique_ptr<GLFWWindowManager> m_windowManager;
    std::unique_ptr<IRenderer> m_renderer;
    std::unique_ptr<World> m_world;
    std::unordered_map<int, std::unique_ptr<Camera>> m_cameras;
    std::unique_ptr<Input> m_input;

    bool m_initialized{false};
    GameConfig m_gameConfig;

    std::optional<int> m_focusedWindowId;
};

}  // namespace VulkanEngine