#pragma once

#include "Libraries/Math3D/Math3D.h"
#include <array>

namespace VulkanEngine
{
struct UniformBufferObject
{
    alignas(16) Math3D::Mat4 model;
    alignas(16) Math3D::Mat4 view;
    alignas(16) Math3D::Mat4 proj;
};

}  // namespace VulkanEngine