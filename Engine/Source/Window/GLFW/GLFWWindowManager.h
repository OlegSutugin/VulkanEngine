#pragma once
#include <memory>
#include <expected>
#include "GLFWWindow.h"

namespace VulkanEngine
{
enum class WindowCreationError
{
    ManagerIsNotInitialized,
    CreationFailed,
};

class GLFWWindowManager final
{
public:
    GLFWWindowManager();
    ~GLFWWindowManager();

    void update();
    std::expected<WindowId, WindowCreationError> createWindow(const WindowSettings& settings);
    std::shared_ptr<GLFWWindow> getWindowById(WindowId id) const;
    bool areAllWindowsClosed() const;
    void cleanupClosedWindows();

private:
    bool m_initialized{false};
    std::unordered_map<WindowId, std::shared_ptr<GLFWWindow>> m_windows;
    int m_windowIdCounter{1};
};

}  // namespace VulkanEngine