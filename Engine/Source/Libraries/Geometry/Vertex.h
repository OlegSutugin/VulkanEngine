#pragma once

#include "Libraries/Math3D/Math3D.h"

namespace VulkanEngine
{

// TODO:: need to refactor on different API
struct Vertex
{
    Math3D::Vec3 pos;
    Math3D::Vec3 color;
};

}  // namespace VulkanEngine