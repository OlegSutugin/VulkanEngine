#include "VulkanRenderer.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(VulkanCommandsLog)
DEFINE_LOG_CATEGORY_STATIC(VulkanCommandsDebugLog)

void VulkanRenderer::CreateCommandBuffers(WindowRenderContext& context)
{
    context.commandBuffers.resize(context.swapchainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(context.commandBuffers.size());

    if (vkAllocateCommandBuffers(m_device, &allocInfo, context.commandBuffers.data()) != VK_SUCCESS)
    {
        VE_LOG(VulkanCommandsLog, Error, "Failed to allocate command buffers");
    }
}

void VulkanRenderer::CreateSyncObjects(WindowRenderContext& context)
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &context.imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &context.renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(m_device, &fenceInfo, nullptr, &context.inFlightFence) != VK_SUCCESS)
    {
        VE_LOG(VulkanCommandsLog, Error, "Failed to create sync objects");
    }
}

void VulkanRenderer::RecordCommandBuffer(WindowRenderContext& context, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        VE_LOG(VulkanCommandsLog, Error, "Failed to begin recording command buffer");
        return;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = context.renderPass;
    renderPassInfo.framebuffer = context.swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = context.swapchainExtent;

    VkClearValue clearColor = {{{0.2f, 0.3f, 0.8f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.graphicsPipeline);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout,  //
        0, 1, &context.descriptorSets[0], 0, nullptr);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(context.swapchainExtent.width);
    viewport.height = static_cast<float>(context.swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = context.swapchainExtent;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // for all meshes that clipped in CPU (from world)
    for (const auto& item : context.drawItems)
    {
        auto it = m_meshes.find(item.mesh.id);  // find gpu mesh by id
        if (it == m_meshes.end())
        {
            continue;
        }

        const GpuMesh& gpuMesh = it->second;  // concrete mesh

        vkCmdPushConstants(commandBuffer, context.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Math3D::Mat4), &item.worldMatrix);

        VkBuffer vertexBuffers[] = {gpuMesh.vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, gpuMesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(commandBuffer, gpuMesh.indexCount, 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        VE_LOG(VulkanCommandsLog, Error, "Failed to record command buffer");
    }
}