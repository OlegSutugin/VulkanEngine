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

    static MeshDesc GetCube()
    {
        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},  // 0
            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},   // 1
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},    // 2
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},   // 3
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},   // 4
            {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},    // 5
            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},     // 6
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},    // 7
        };

        const std::vector<uint16_t> indices = {
            // back face  (z = -0.5)
            0, 1, 2, 2, 3, 0,  //
            // front face (z = 0.5)
            4, 6, 5, 6, 4, 7,  //
            // left face
            4, 0, 3, 3, 7, 4,  //
            // right face
            1, 5, 6, 6, 2, 1,  //
            // bottom face
            4, 5, 1, 1, 0, 4,  //
            // top face
            3, 2, 6, 6, 7, 3,  //
        };

        return MeshDesc{vertices, indices};
    }
};
}  // namespace VulkanEngine