#include "VulkanRenderer.h"
#include <cstring>
#include <set>
#include <vector>

using namespace VulkanEngine;

namespace
{
const std::vector<const char*> c_RequiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

// we need to find QueueFamilies that allos us do whatever we want to do with Vulkan. QueueFamilyIndices uses std::optional which is great
// (c++ 17)
QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // here we found first one that uses all we needed
    for (uint32_t i = 0; i < queueFamilies.size(); ++i)
    {
        // all graphic commands (drawing, indexing drowing, drawing region)
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        // we can show frame image, vsynk, working with swap chains
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        /* Additional device opportunities
         * queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT - computations
         * queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT - transfering
         */
    }

    return indices;
}

bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapchainAdequate = false;
    if (extensionsSupported)
    {
        SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device, surface);
        swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapchainAdequate;
}

bool VulkanRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(c_RequiredDeviceExtensions.begin(), c_RequiredDeviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    /* availableExtensions
     * "VK_KHR_swapchain"
     * "VK_EXT_debug_utils"
     * "VK_KHR_dedicated_allocation"
     * etc..
     */

    // we had c_RequiredDeviceExtensions and if requiredExtensions is empty that  - device is good and we can go further
    // for now we use only one extension: VK_KHR_SWAPCHAIN_EXTENSION_NAME

    return requiredExtensions.empty();
}