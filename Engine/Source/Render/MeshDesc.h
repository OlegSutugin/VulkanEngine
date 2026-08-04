#pragma once

#include <vector>
#include <cstdint>
#include "Vertex.h"

namespace VulkanEngine
{
struct MeshDesc
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};
}  // namespace VulkanEngine