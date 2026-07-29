#include "GLFWWindow.h"
#include "GLFW/glfw3.h"
#include <string>

using namespace VulkanEngine;

//todo: logging

GLFWWindow::GLFWWindow(const WindowSettings& settings) 
{
    m_window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), nullptr, nullptr);

	if (!m_window) return;

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


