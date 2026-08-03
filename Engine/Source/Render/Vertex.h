#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>

namespace VulkanEngine
{

// TODO:: need to refactor on different API
struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

}  // namespace VulkanEngine