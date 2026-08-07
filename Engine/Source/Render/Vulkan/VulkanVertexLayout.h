#pragma once

#include "Libraries/Geometry/Vertex.h"
#include <vulkan/vulkan.h>
#include <array>

namespace VulkanEngine
{
struct VulkanVertexLayout
{
    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};
}  // namespace VulkanEngine