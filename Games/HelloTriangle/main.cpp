#pragma once

#include "Core/Engine.h"
#include "Core/GameConfig.h"
#include "Render/Vertex.h"

int main()
{
	VulkanEngine::GameConfig config;
	config.windowTitle = std::string("HelloTriangle");
	config.shadersPath = "Binaries/Shaders/";
	config.pipeline = { "Simple_Shader.vert.spv", "Simple_Shader.frag.spv" };

	const std::vector<VulkanEngine::Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},  //
	{{0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},   //
	{{0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    //
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}    //
	};

	const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
	config.meshes.push_back({ vertices, indices });

	VulkanEngine::Engine engine(config);
	engine.run();

	return 0;
}