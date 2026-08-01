#include "GLFWWindow.h"
#include "Log/Log.h"
#include "GLFW/glfw3.h"
#include <string>

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogGLFWWindow);

GLFWWindow::GLFWWindow(const WindowSettings& settings)
{
    m_window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        VE_LOG(LogGLFWWindow, Error, "Failed to create GLFW window!");
        return;
    }

    glfwSetWindowPos(m_window, settings.x_Pos, settings.y_Pos);
}

GLFWWindow::~GLFWWindow()
{
    if (m_window)
    {
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

GLFWwindow* VulkanEngine::GLFWWindow::getNativeHandle()
{
    return m_window;
}
