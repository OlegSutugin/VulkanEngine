#pragma once

#include <string>
#include <cstdint>

namespace VulkanEngine
{
struct PipelineDesc
{
    std::string vertShader;
    std::string fragShader;
};

struct GameConfig
{
    std::string shadersPath{};
    std::string windowTitle = "VulkanEngine";
    PipelineDesc pipeline;
};
}  // namespace VulkanEngine