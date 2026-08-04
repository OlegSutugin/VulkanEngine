#pragma once

#include <cstdint>

namespace VulkanEngine
{

struct MeshHandle
{
    uint32_t id = 0;
    bool IsValid() const { return id != 0; }
};

}  // namespace VulkanEngine