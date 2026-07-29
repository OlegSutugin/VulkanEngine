#pragma once

#include <memory>

namespace VulkanEngine
{
class GLFWWindowManager;

class Engine final
{
public:
    Engine();
    ~Engine();

    void run();

private:
    std::unique_ptr<GLFWWindowManager> m_windowManager;
    bool m_initialized{false};
};

}  // namespace VulkanEngine