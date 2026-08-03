#include "GLFWWindow.h"
#include "Log/Log.h"
#include "GLFW/glfw3.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogGLFWWindow);

namespace
{
void GLFWSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self && self->m_onWindowResizedCallback)
    {
        self->m_onWindowResizedCallback(width, height);
    }
}
}  // namespace

GLFWWindow::GLFWWindow(const WindowSettings& settings)
{
    m_window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        VE_LOG(LogGLFWWindow, Error, "Failed to create GLFW window!");
        return;
    }

    glfwSetWindowPos(m_window, settings.x_Pos, settings.y_Pos);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, GLFWSizeCallback);
}

GLFWWindow::~GLFWWindow()
{
    if (m_window)
    {
        glfwSetWindowUserPointer(m_window, nullptr);
        glfwSetWindowSizeCallback(m_window, nullptr);
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

void GLFWWindow::setTitle(const std::string& title)
{
    if (!m_window) return;

    glfwSetWindowTitle(m_window, title.c_str());
}

bool GLFWWindow::isValid() const
{
    return m_window != nullptr;
}

bool GLFWWindow::shouldClose() const
{
    if (!m_window) return true;
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* GLFWWindow::getNativeHandle()
{
    return m_window;
}
