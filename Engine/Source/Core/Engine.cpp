#include "Engine.h"
#include "Log/Log.h"
#include "Window/GLFW/GLFWWindowManager.h"
#include "Render/Vulkan/VulkanRenderer.h"
#include "Runtime/World.h"
#include "Runtime/Camera.h"
#include "Runtime/Input.h"
#include "Runtime/Actor/Actor.h"
#include <format>

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogEngine);

Engine::Engine(const GameConfig& inConfig) : m_gameConfig(inConfig)
{
    VE_LOG(LogEngine, Display, "Initialize Engine");

    m_input = std::make_unique<Input>();
    m_windowManager = std::make_unique<GLFWWindowManager>();
    m_renderer = std::make_unique<VulkanRenderer>();

    m_world = World::CreateWorld(m_renderer.get());

    m_renderer->Init(m_gameConfig);

    m_windowManager->setOnWindowClosedCallback(
        [this](WindowId id)
        {
            m_renderer->UnregisterWindow(id);
            m_cameras.erase(id);
            if (m_focusedWindowId == id)
            {
                m_focusedWindowId.reset();
                m_input->SetWindowToFocus(nullptr);
            }
        });

    m_windowManager->setOnWindowResizedCallback(
        [this](int id, int newWidth, int newHeight)
        {
            if (m_renderer)
            {
                m_renderer->WindowWasResized(id, newWidth, newHeight);
            }
        });

    m_windowManager->setOnWindowFocusChangedCallback(
        [this](WindowId id, bool focused)
        {
            if (focused)
            {
                m_focusedWindowId = id;
                if (auto window = m_windowManager->getWindowById(id))
                {
                    m_input->SetWindowToFocus(window);
                }
            }
            else if (m_focusedWindowId == id)
            {
                m_focusedWindowId.reset();
                m_input->SetWindowToFocus(nullptr);
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
        RegisterWindowViewport(windowCreationResult.value(), window);
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
        RegisterWindowViewport(windowCreationResult2.value(), window);
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
    m_input->Update();

    if (m_focusedWindowId.has_value())
    {
        auto it = m_cameras.find(*m_focusedWindowId);
        if (it != m_cameras.end())
        {
            Camera* activeCamera = it->second.get();

            if (m_input->IsKeyPressed(GLFW_KEY_W)) activeCamera->MoveForward(deltaTime);
            if (m_input->IsKeyPressed(GLFW_KEY_S)) activeCamera->MoveForward(-deltaTime);
            if (m_input->IsKeyPressed(GLFW_KEY_D)) activeCamera->MoveRight(deltaTime);
            if (m_input->IsKeyPressed(GLFW_KEY_A)) activeCamera->MoveRight(-deltaTime);

            const float mouseSensitivity = 0.1f;
            float yawDelta = -m_input->GetMouseDeltaX() * mouseSensitivity;
            float pitchDelta = -m_input->GetMouseDeltaY() * mouseSensitivity;
            activeCamera->Rotate(yawDelta, pitchDelta);
        }
    }

    for (const auto& [windowId, camera] : m_cameras)
    {
        m_renderer->SetCameraView(windowId, camera->GetView());
    }

    m_renderer->DrawFrame();
}

bool Engine::isRunning() const
{
    return m_initialized;
}

World* Engine::GetWorld() const
{
    return m_world.get();
}

void Engine::EngineStop()
{
    if (m_renderer)
    {
        m_renderer->Shutdown();
    }

    m_initialized = false;
}

void Engine::RegisterWindowViewport(int windowId, std::shared_ptr<GLFWWindow> window)
{
    m_cameras[windowId] = std::make_unique<Camera>();

    if (!m_focusedWindowId.has_value())
    {
        m_focusedWindowId = windowId;
        m_input->SetWindowToFocus(window);
    }
}
