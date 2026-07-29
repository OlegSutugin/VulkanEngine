#include "GLFWWindowManager.h"
#include <glfw/glfw3.h>

using namespace VulkanEngine;
// TODO::Logging

GLFWWindowManager::GLFWWindowManager()
{
    // callback for glfw
    glfwSetErrorCallback([](int errorCode, const char* description) {});

    if (!glfwInit())
    {
        // TODO::Logging
        return;
    }

    m_initialized = true;
}

GLFWWindowManager::~GLFWWindowManager()
{
    m_windows.clear();

    if (m_initialized)
    {
        glfwSetErrorCallback(nullptr);
        glfwTerminate();
    }

    m_initialized = false;
}

void GLFWWindowManager::update()
{
    if (!m_initialized) return;

    glfwPollEvents();
    cleanupClosedWindows();
}

std::expected<WindowId, WindowCreationError> GLFWWindowManager::createWindow(const WindowSettings& settings)
{
    if (!m_initialized) return std::unexpected(WindowCreationError::ManagerIsNotInitialized);

    auto window = std::make_shared<GLFWWindow>(settings);
    if (!window->isValid()) return std::unexpected(WindowCreationError::CreationFailed);

    const WindowId id = m_windowIdCounter++;
    m_windows[id] = window;

    return id;
}

std::shared_ptr<GLFWWindow> GLFWWindowManager::getWindowById(WindowId id) const
{
    const auto it = m_windows.find(id);
    return it != m_windows.end() ? it->second : nullptr ;
}

bool GLFWWindowManager::areAllWindowsClosed() const
{
    return m_windows.empty();
}

void GLFWWindowManager::cleanupClosedWindows() 
{
    auto it = m_windows.begin();
    while (it != m_windows.end())
    {
        if (it->second->shouldClose())
        {
            it = m_windows.erase(it);
            continue;
        }
        ++it;
    }
}
