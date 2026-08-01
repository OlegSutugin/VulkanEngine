#pragma once

#include "Core/Engine.h"

int main()
{
    VulkanEngine::GameConfig config;
    config.windowTitle = std::string("HelloTriangle");
    config.shadersPath = "Resources/Shaders/";

    VulkanEngine::Engine engine(config);
    engine.run();

    return 0;
}