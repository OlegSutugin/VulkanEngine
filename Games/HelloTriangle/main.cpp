#pragma once

#include "Core/Engine.h"
#include "Core/GameConfig.h"

int main()
{
    VulkanEngine::GameConfig config;
    config.windowTitle = std::string("HelloTriangle");
    config.shadersPath = "Binaries/Shaders/";
    config.pipeline = { "Simple_Shader.vert.spv", "Simple_Shader.frag.spv" };

    VulkanEngine::Engine engine(config);
    engine.run();

    return 0;
}