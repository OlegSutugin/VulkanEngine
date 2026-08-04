#pragma once
#include <memory>

namespace VulkanEngine
{

class GLFWWindow;

class Input
{
public:
    void Init(std::shared_ptr<GLFWWindow> window);
    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;

    void Update();
    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

private:
    void BeginMouseCapture();
    void EndMouseCapture();

    std::shared_ptr<GLFWWindow> m_window;

    bool m_isCapturing = false;
    double m_captureStartX = 0.0;
    double m_captureStartY = 0.0;
    double m_lastX = 0.0;
    double m_lastY = 0.0;
    float m_deltaX = 0.0f;
    float m_deltaY = 0.0f;

    bool m_firstUpdate = true;
};
}  // namespace VulkanEngine
