#pragma once
#include <unordered_map>
#include <string>

#define GLFW_INCLUDE_VULKAN

class GLFWwindow;

namespace VulkanEngine
{
using WindowId = int;

struct WindowSettings
{
    std::string title{};
    int width{1920};
    int height{1080};
    int x_Pos{50};
    int y_Pos{50};
};

class GLFWWindow final
{
public:
    GLFWWindow(const WindowSettings& settings);
    ~GLFWWindow();

    void setTitle(const std::string& title);
    bool isValid() const;
    bool shouldClose() const;
    GLFWwindow* getNativeHandle();

private:
    GLFWwindow* m_window{nullptr};
};

}  // namespace VulkanEngine