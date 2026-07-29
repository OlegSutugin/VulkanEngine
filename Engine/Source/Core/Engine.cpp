#include "Engine.h"
#include "Window/GLFW/GLFWWindowManager.h"
#include <format>
#include <iostream>

using namespace VulkanEngine;

Engine::Engine()
{
    std::cout << "Initialize Engine" << std::endl;

    m_windowManager = std::make_unique<GLFWWindowManager>();

    const auto windowCreationResult = m_windowManager->createWindow(WindowSettings{});
    if (!windowCreationResult)
    {
        // log
        return;
    }

    if (auto window = m_windowManager->getWindowById(windowCreationResult.value()))
    {
        window->setTitle(std::format("Vulkan engine v1"));
    }

    m_initialized = true;
}

Engine::~Engine() = default;

void VulkanEngine::Engine::run()
{
    if (!m_initialized)
    {
        std::cout << "not initialized" << std::endl;
        return;
    }

    while (!m_windowManager->areAllWindowsClosed())
    {
        m_windowManager->update();
    }
}
