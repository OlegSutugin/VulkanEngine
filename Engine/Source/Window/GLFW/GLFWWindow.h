#pragma once
#include <unordered_map>

class GLFWwindow;

namespace VulkanEngine
{
using WindowId = int;

struct WindowSettings
{
    std::string title{};
    int width{800};
    int height{600};
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

private:
    GLFWwindow* m_window{nullptr};
};

}  // namespace VulkanEngine