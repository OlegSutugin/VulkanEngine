#include "VulkanRenderer.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(VulkanDescriptorsLog)
DEFINE_LOG_CATEGORY_STATIC(VulkanDescriptorsDebugLog)

void VulkanRenderer::CreateDescriptorSetLayout(WindowRenderContext& context)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &context.descriptorSetLayout) != VK_SUCCESS)
    {
        VE_LOG(VulkanDescriptorsLog, Error, "failed to create descriptor set layout!");
    }
}
void VulkanRenderer::CreateDescriptorPool(WindowRenderContext& context)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(1);  // TODO: MaxFrames in flight

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &context.descriptorPool) != VK_SUCCESS)
    {
        VE_LOG(VulkanDescriptorsLog, Error, "Failed to create descriptor pool");
    }
}
void VulkanRenderer::CreateDescriptorSets(WindowRenderContext& context)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = context.descriptorPool;
    allocInfo.descriptorSetCount = 1;  // TODO: MaxFrames in flight
    allocInfo.pSetLayouts = &context.descriptorSetLayout;

    context.descriptorSets.resize(1);
    if (vkAllocateDescriptorSets(m_device, &allocInfo, context.descriptorSets.data()) != VK_SUCCESS)
    {
        VE_LOG(VulkanDescriptorsLog, Error, "Failed to allocate descriptor sets");
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = context.uniformBuffers[0];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = context.descriptorSets[0];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
}