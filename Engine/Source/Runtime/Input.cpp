#include "Input.h"

#include "Window/GLFW/GLFWWindow.h"
#include <GLFW/glfw3.h>

using namespace VulkanEngine;

bool Input::IsKeyPressed(int key) const
{
    if (!m_window || !m_window->getNativeHandle()) return false;
    return glfwGetKey(m_window->getNativeHandle(), key) == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(int button) const
{
    if (!m_window) return false;
    return glfwGetMouseButton(m_window->getNativeHandle(), button) == GLFW_PRESS;
}

void Input::Update()
{
    if (!m_window) return;

    const bool lmbDown = IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);

    if (lmbDown && !m_isCapturing)
    {
        BeginMouseCapture();
    }
    else if (!lmbDown && m_isCapturing)
    {
        EndMouseCapture();
    }

    if (!m_isCapturing)
    {
        m_deltaX = 0.0f;
        m_deltaY = 0.0f;
        return;
    }

    double x, y;
    glfwGetCursorPos(m_window->getNativeHandle(), &x, &y);

    if (m_firstUpdate)
    {
        m_lastX = x;
        m_lastY = y;
        m_firstUpdate = false;
    }

    m_deltaX = static_cast<float>(x - m_lastX);
    m_deltaY = static_cast<float>(y - m_lastY);

    m_lastX = x;
    m_lastY = y;
}

float Input::GetMouseDeltaX() const
{
    return m_deltaX;
}

float Input::GetMouseDeltaY() const
{
    return m_deltaY;
}

void Input::SetWindowToFocus(std::shared_ptr<GLFWWindow> window)
{
    if (window == m_window) return;

    if (m_isCapturing)
    {
        EndMouseCapture();
    }

    m_window = window;
}

void Input::BeginMouseCapture()
{
    m_isCapturing = true;

    glfwGetCursorPos(m_window->getNativeHandle(), &m_captureStartX, &m_captureStartY);
    m_lastX = m_captureStartX;
    m_lastY = m_captureStartY;

    glfwSetInputMode(m_window->getNativeHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::EndMouseCapture()
{
    m_isCapturing = false;

    glfwSetInputMode(m_window->getNativeHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPos(m_window->getNativeHandle(), m_captureStartX, m_captureStartY);
}
