#include "GLFWWindowManager.h"
#include "Log/Log.h"
#include <glfw/glfw3.h>

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogGLFWWindowManager);

GLFWWindowManager::GLFWWindowManager()
{
    // callback for glfw
    glfwSetErrorCallback([](int errorCode, const char* description)
        { VE_LOG(LogGLFWWindowManager, Error, "GLFW error, code: {}, description: {}", errorCode, description); });

    if (!glfwInit())
    {
        VE_LOG(LogGLFWWindowManager, Error, "Failed to initialize GLFW!");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_initialized = true;
    VE_LOG(LogGLFWWindowManager, Display, "GLFW initialized successfully!");
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
    VE_LOG(LogGLFWWindowManager, Display, "GLFW shutdown complete!");
}

void GLFWWindowManager::update()
{
    if (!m_initialized) return;

    glfwPollEvents();
    cleanupClosedWindows();
}

std::expected<WindowId, WindowCreationError> GLFWWindowManager::createWindow(const WindowSettings& settings)
{
    if (!m_initialized)
    {
        VE_LOG(LogGLFWWindowManager, Error, "Cannot create window. GLFW is not initialized.");
        return std::unexpected(WindowCreationError::ManagerIsNotInitialized);
    }

    auto window = std::make_shared<GLFWWindow>(settings);
    if (!window->isValid())
    {
        VE_LOG(LogGLFWWindowManager, Error, "Failed to create GLFW window.");
        return std::unexpected(WindowCreationError::CreationFailed);
    }

    const WindowId id = m_windowIdCounter++;
    m_windows[id] = window;

    VE_LOG(LogGLFWWindowManager, Display, "Added window with id: {}", id);

    return id;
}

std::shared_ptr<GLFWWindow> GLFWWindowManager::getWindowById(WindowId id) const
{
    const auto it = m_windows.find(id);
    return it != m_windows.end() ? it->second : nullptr;
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
            VE_LOG(LogGLFWWindowManager, Display, "Remove closed window with id: {}", it->first);

            if (m_onWindowClosedCallback)
            {
                m_onWindowClosedCallback(it->first);
            }

            it = m_windows.erase(it);
            continue;
        }
        ++it;
    }
}

void GLFWWindowManager::setOnWindowClosedCallback(std::function<void(WindowId)> callback)
{
    m_onWindowClosedCallback = std::move(callback);
}
