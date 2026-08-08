#include "VulkanRenderer.h"
#include "Log/Log.h"
#include "Libraries/Common/Math.h"
#include "Core/PlatformDefines.h"
#include <cstring>

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(VulkanBuffersLog)
DEFINE_LOG_CATEGORY_STATIC(VulkanBuffersDebugLog)

void VulkanRenderer::CreateFramebuffers(WindowRenderContext& context)
{
    context.swapchainFramebuffers.resize(context.swapchainImageViews.size());

    for (size_t i = 0; i < context.swapchainImageViews.size(); ++i)
    {
        VkImageView attachments[] = {context.swapchainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = context.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = context.swapchainExtent.width;
        framebufferInfo.height = context.swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &context.swapchainFramebuffers[i]) != VK_SUCCESS)
        {
            VE_LOG(VulkanBuffersLog, Error, "Failed to create framebuffer");
        }
    }
}

void VulkanRenderer::CreateUniformBuffers(WindowRenderContext& context)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    // todo MAX_FRAMES_IN_FLIGHT
    context.uniformBuffers.resize(1);
    context.uniformBuffersMemory.resize(1);
    context.uniformBuffersMapped.resize(1);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        context.uniformBuffers[0], context.uniformBuffersMemory[0]);

    vkMapMemory(m_device, context.uniformBuffersMemory[0], 0, bufferSize, 0, &context.uniformBuffersMapped[0]);
}

void VulkanRenderer::UpdateUniformBuffer(WindowRenderContext& context)
{
    UniformBufferObject ubo{};
    ubo.view = context.cameraView;

    ubo.proj = Math3D::Mat4::PerspectiveProjectionMatrix(
        Math::DegreesToRadians(45.0f), context.swapchainExtent.width / (float)context.swapchainExtent.height, 0.1f, 100.0f);
    CLIP_SPACE_Y_FLIP(ubo.proj);

    memcpy(context.uniformBuffersMapped[0], &ubo, sizeof(ubo));
}

void VulkanRenderer::CreateBuffer(
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        VE_LOG(VulkanBuffersLog, Critical, "failed to create buffer!!");
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        VE_LOG(VulkanBuffersLog, Critical, "failed to allocate buffer memory!!");
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    VE_LOG(VulkanBuffersLog, Critical, "can't find memory type!!");
    throw std::runtime_error("Failed to find suitable memory type");
}
