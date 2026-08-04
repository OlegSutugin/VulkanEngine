#pragma once

#include "Core/Engine.h"
#include "Core/GameConfig.h"
#include "Render/Vertex.h"
#include <chrono>

void GenerateSierpinski(const VulkanEngine::Vertex& a,  //
    const VulkanEngine::Vertex& b,                      //
    const VulkanEngine::Vertex& c,                      //
    int depth,                                          //
    std::vector<VulkanEngine::Vertex>& outVertices)
{
    if (depth == 0)
    {
        outVertices.push_back(a);
        outVertices.push_back(b);
        outVertices.push_back(c);
        return;
    }

    VulkanEngine::Vertex ab{{(a.pos.x + b.pos.x) * 0.5f, (a.pos.y + b.pos.y) * 0.5f}, a.color};
    VulkanEngine::Vertex bc{{(b.pos.x + c.pos.x) * 0.5f, (b.pos.y + c.pos.y) * 0.5f}, b.color};
    VulkanEngine::Vertex ca{{(c.pos.x + a.pos.x) * 0.5f, (c.pos.y + a.pos.y) * 0.5f}, c.color};

    GenerateSierpinski(a, ab, ca, depth - 1, outVertices);
    GenerateSierpinski(ab, b, bc, depth - 1, outVertices);
    GenerateSierpinski(ca, bc, c, depth - 1, outVertices);
}

VulkanEngine::MeshDesc BuildSierpinskiMesh(  //
    const VulkanEngine::Vertex& a,           //
    const VulkanEngine::Vertex& b,           //
    const VulkanEngine::Vertex& c, int depth)
{
    VulkanEngine::MeshDesc desc;
    GenerateSierpinski(a, b, c, depth, desc.vertices);

    desc.indices.resize(desc.vertices.size());
    for (uint16_t i = 0; i < desc.vertices.size(); ++i)
    {
        desc.indices[i] = i;
    }

    return desc;
}

int main()
{
    VulkanEngine::GameConfig config;
    config.windowTitle = std::string("HelloTriangle");
    config.shadersPath = "Binaries/Shaders/";
    config.pipeline = {"Simple_Shader.vert.spv", "Simple_Shader.frag.spv"};

    const std::vector<VulkanEngine::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 1.0f, 0.0f}},  //
        {{0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},   //
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},  //
    };

    const std::vector<uint16_t> indices = {0, 1, 2};
    // const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
    // config.meshes.push_back({ vertices, indices });

    VulkanEngine::Engine engine(config);

    const auto mesh = VulkanEngine::MeshDesc{vertices, indices};
    VulkanEngine::MeshHandle currentMeshHandle = engine.CreateMesh(mesh);

    auto lastTime = std::chrono::steady_clock::now();

    /*game*/
    const int MaxGeneratedLayers = 6;
    float timebetweengenerations = 1.f;
    int CurrentLayer = 0;

    while (engine.isRunning())
    {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        /*our code*/
        timebetweengenerations -= deltaTime;
        if (timebetweengenerations < 0)
        {
            timebetweengenerations = 1.f;

            if (CurrentLayer > MaxGeneratedLayers)
            {
                CurrentLayer = 0;
            }
            else
            {
                CurrentLayer++;
            }

            engine.DestroyMesh(currentMeshHandle);
            currentMeshHandle = engine.CreateMesh(BuildSierpinskiMesh(vertices[0], vertices[1], vertices[2], CurrentLayer));
        }

        engine.Tick(deltaTime);
    }

    return 0;
}
