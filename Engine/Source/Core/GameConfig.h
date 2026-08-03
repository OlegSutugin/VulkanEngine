#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "Render/Vertex.h"

namespace VulkanEngine
{
struct PipelineDesc
{
    std::string vertShader;
    std::string fragShader;
};

struct MeshDesc
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

struct GameConfig
{
    std::string shadersPath{};
    std::string windowTitle = "VulkanEngine";
    PipelineDesc pipeline;
    std::vector<MeshDesc> meshes;
};
}  // namespace VulkanEngine