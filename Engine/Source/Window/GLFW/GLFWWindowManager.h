#pragma once
#include <memory>
#include <expected>
#include <functional>
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

    void setOnWindowClosedCallback(std::function<void(WindowId)> callback);
    void setOnWindowResizedCallback(std::function<void(WindowId, int, int)> callback);

private:
    bool m_initialized{false};
    std::unordered_map<WindowId, std::shared_ptr<GLFWWindow>> m_windows;
    int m_windowIdCounter{1};
    std::function<void(WindowId)> m_onWindowClosedCallback;
    std::function<void(WindowId, int, int)> m_onWindowResizedCallback;
};

}  // namespace VulkanEngine