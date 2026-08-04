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

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

}  // namespace VulkanEngine