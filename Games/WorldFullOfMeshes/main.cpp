#pragma once

#include "Core/Engine.h"
#include "Core/GameConfig.h"
#include "Libraries/Geometry/Vertex.h"
#include "Core/EngineTypes.h"
#include "Runtime/World.h"
#include "Runtime/Actor/CubeActor.h"
#include <chrono>

int main()
{
    VulkanEngine::GameConfig config;
    config.windowTitle = std::string("WorldFullOfMeshes");
    config.shadersPath = "Binaries/Shaders/";
    config.pipeline = {"Cube.vert.spv", "Cube.frag.spv"};

    VulkanEngine::Engine engine(config);

    if (engine.GetWorld())
    {
        engine.GetWorld()->SpawnActor<VulkanEngine::CubeActor>(VulkanEngine::Transform::ZeroTransform());
    }

    auto lastTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point currentTime;
    float deltaTime;

    while (engine.isRunning())
    {
        currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        engine.Tick(deltaTime);
    }

    return 0;
}
