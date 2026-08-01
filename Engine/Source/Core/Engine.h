#pragma once

#include <memory>
#include <string>

namespace VulkanEngine
{
class GLFWWindowManager;
class IRenderer;

struct GameConfig
{
    std::string shadersPath{};
    uint32_t windowWidth = 1920;
    uint32_t windowHeight = 1080;
    std::string windowTitle = "VulkanEngine";
};

class Engine final
{
public:
    Engine(const GameConfig& inConfig = GameConfig());
    ~Engine();

    void run();

private:
    std::unique_ptr<GLFWWindowManager> m_windowManager;
    std::unique_ptr<IRenderer> m_renderer;

    bool m_initialized{false};
    GameConfig m_gameConfig;
};

}  // namespace VulkanEngine