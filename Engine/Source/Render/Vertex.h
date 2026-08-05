#pragma once

#include <vulkan/vulkan.h>
#include "Libraries/Math3D/Math3D.h"
#include <array>

namespace VulkanEngine
{

// TODO:: need to refactor on different API
struct Vertex
{
    Math3D::Vec2 pos;
    Math3D::Vec3 color;
};

struct UniformBufferObject
{
    alignas(16) Math3D::Mat4 model;
    alignas(16) Math3D::Mat4 view;
    alignas(16) Math3D::Mat4 proj;
};

}  // namespace VulkanEngine