#include "VulkanRenderer.h"
#include "Log/Log.h"
#include <algorithm>
#include <limits>

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(VulkanSwapchainLog)
DEFINE_LOG_CATEGORY_STATIC(VulkanSwapchainDebugLog)

// using concrete window to render
VkSurfaceKHR VulkanRenderer::CreateSurfaceForHandle(void* nativeWindowHandle) const
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(nativeWindowHandle);

    if (glfwCreateWindowSurface(m_instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        VE_LOG(VulkanSwapchainLog, Error, "Failed to create window surface");
        return VK_NULL_HANDLE;
    }

    return surface;
}

void VulkanRenderer::CreateSwapchainForWindow(WindowRenderContext& context, uint32_t width, uint32_t height, VkSwapchainKHR oldSwapchain)
{
    SwapchainSupportDetails support = QuerySwapchainSupport(m_physicalDevice, context.surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(support.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(support.presentModes);
    VkExtent2D extent = ChooseSwapExtent(support.capabilities, width, height);

    uint32_t imageCount = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount)
    {
        imageCount = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = context.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value()};

    if (m_queueFamilyIndices.graphicsFamily.value() != m_queueFamilyIndices.presentFamily.value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &context.swapchain) != VK_SUCCESS)
    {
        VE_LOG(VulkanSwapchainLog, Critical, "Failed to create swapchain");
        return;
    }

    uint32_t actualImageCount = 0;
    vkGetSwapchainImagesKHR(m_device, context.swapchain, &actualImageCount, nullptr);
    context.swapchainImages.resize(actualImageCount);
    vkGetSwapchainImagesKHR(m_device, context.swapchain, &actualImageCount, context.swapchainImages.data());

    context.swapchainImageFormat = surfaceFormat.format;
    context.swapchainExtent = extent;
}

// it's like an interpreter for raw data
void VulkanRenderer::CreateImageViews(WindowRenderContext& context)
{
    context.swapchainImageViews.resize(context.swapchainImages.size());

    for (size_t i = 0; i < context.swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = context.swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = context.swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &createInfo, nullptr, &context.swapchainImageViews[i]) != VK_SUCCESS)
        {
            VE_LOG(VulkanSwapchainLog, Error, "Failed to create image view");
        }
    }
}

SwapchainSupportDetails VulkanRenderer::QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
{
    for (const auto& format : availableFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{
    for (const auto& mode : availablePresentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D extent = {width, height};
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}

void VulkanRenderer::RecreateSwapChainForWindow(WindowRenderContext& context, uint32_t newWidth, uint32_t newHeight)
{
    // waiting
    vkDeviceWaitIdle(m_device);

    // clearing
    for (auto framebuffer : context.swapchainFramebuffers)
    {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    context.swapchainFramebuffers.clear();

    for (auto imageView : context.swapchainImageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }

    context.swapchainImageViews.clear();

    VkSwapchainKHR oldSwapchain = context.swapchain;
    CreateSwapchainForWindow(context, newWidth, newHeight, oldSwapchain);

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device, oldSwapchain, nullptr);
    }

    // recreating
    CreateImageViews(context);
    CreateFramebuffers(context);
}