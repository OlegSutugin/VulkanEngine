#pragma once

#include <memory>
#include <string>
#include "GameConfig.h"

namespace VulkanEngine
{
class GLFWWindowManager;
class IRenderer;

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