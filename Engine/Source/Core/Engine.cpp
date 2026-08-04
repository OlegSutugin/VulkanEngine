#include "Engine.h"
#include "Log/Log.h"
#include "Window/GLFW/GLFWWindowManager.h"
#include "Render/Vulkan/VulkanRenderer.h"
#include <format>

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogEngine);

Engine::Engine(const GameConfig& inConfig) : m_gameConfig(inConfig)
{
    VE_LOG(LogEngine, Display, "Initialize Engine");

    m_windowManager = std::make_unique<GLFWWindowManager>();
    m_renderer = std::make_unique<VulkanRenderer>();
    m_renderer->Init(m_gameConfig);

    m_windowManager->setOnWindowClosedCallback([this](WindowId id) { m_renderer->UnregisterWindow(id); });
    m_windowManager->setOnWindowResizedCallback(
        [this](int id, int newWidth, int newHeight)
        {
            if (m_renderer)
            {
                m_renderer->WindowWasResized(id, newWidth, newHeight);
            }
        });

    const auto windowCreationResult = m_windowManager->createWindow(WindowSettings{});
    if (!windowCreationResult)
    {
        VE_LOG(LogEngine, Error, "Failed to create main window");
        return;
    }

    if (auto window = m_windowManager->getWindowById(windowCreationResult.value()))
    {
        window->setTitle(m_gameConfig.windowTitle);
        m_renderer->RegisterWindow(windowCreationResult.value(), window->getNativeHandle());
    }

#pragma region MultiWindowing
    /*WindowSettings settings2;
    settings2.title = "Second";
    settings2.width = 400;
    settings2.height = 400;
    const auto windowCreationResult2 = m_windowManager->createWindow(settings2);
    if (auto window = m_windowManager->getWindowById(windowCreationResult2.value()))
    {
        window->setTitle(std::format("Vulkan engine v1 - second window"));
        m_renderer->RegisterWindow(windowCreationResult2.value(), window->getNativeHandle());
    }*/
#pragma endregion

    m_initialized = true;
}

Engine::~Engine() = default;

void Engine::Tick(float deltaTime)
{
    if (m_windowManager->areAllWindowsClosed())
    {
        EngineStop();
        return;
    }

    m_windowManager->update();
    m_renderer->DrawFrame();
}

bool Engine::isRunning() const
{
    return m_initialized;
}

MeshHandle Engine::CreateMesh(const MeshDesc& desc)
{
    if (!m_renderer)
    {
        VE_LOG(LogEngine, Error, "Cannot create mesh: renderer is not initialized");
        return MeshHandle();
    }
    return m_renderer->CreateMesh(desc);
}

void Engine::DestroyMesh(MeshHandle handle)
{
    if (m_renderer)
    {
        m_renderer->DestroyMesh(handle);
    }
}

void Engine::EngineStop()
{
    m_renderer->Shutdown();
    m_initialized = false;
}
