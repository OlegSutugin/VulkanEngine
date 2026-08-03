#include "VulkanRenderer.h"
#include "pch.h"
#include <fstream>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(VulkanRenderLog)
DEFINE_LOG_CATEGORY_STATIC(VulkanRenderDebugLog)

namespace
{
const std::vector<const char*> kRequiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

void VulkanRenderer::Init(const GameConfig& config)
{
    m_gameConfig = config;
    CreateInstance();
}

void VulkanRenderer::RegisterWindow(int windowId, void* nativeWindowHandle)
{
    VkSurfaceKHR surface = CreateSurfaceForHandle(nativeWindowHandle);
    if (surface == VK_NULL_HANDLE)
    {
        return;
    }

    if (!m_deviceCreated)
    {
        PickPhysicalDevice(surface);
        CreateLogicalDevice();
        CreateCommandPool();
        m_deviceCreated = true;
    }

    WindowRenderContext context;
    context.surface = surface;
    context.nativeWindowHandle = reinterpret_cast<GLFWwindow*>(nativeWindowHandle);

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(nativeWindowHandle), &width, &height);

    CreateSwapchainForWindow(context, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    CreateImageViews(context);
    CreateRenderPass(context);
    CreateGraphicsPipeline(context);
    CreateFramebuffers(context);
    CreateCommandBuffers(context);
    CreateSyncObjects(context);

    m_windowContexts[windowId] = std::move(context);
}

void VulkanRenderer::UnregisterWindow(int windowId)
{
    auto it = m_windowContexts.find(windowId);
    if (it == m_windowContexts.end())
    {
        return;
    }

    DestroyWindowRenderContext(it->second);
    m_windowContexts.erase(it);
}

#pragma region Instance & Device
/** The instance is the connection between your application and the Vulkan library and creating it
 *  involves specifying some details about your application to the driver.
 */
void VulkanRenderer::CreateInstance()
{
    // TODO: path version to here from CMAke in future
    //  meta data about application
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // telling how to create VkInstance - first object in Vulkan
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // specify the desired global extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    // basically creating instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Critical, "Failed to create Vulkan Instance with code: {}", static_cast<uint32_t>(result));
    }
}

void VulkanRenderer::PickPhysicalDevice(VkSurfaceKHR surfaceForPresentCheck)
{
    // just getting number of graphical cards on a machine
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        VE_LOG(VulkanRenderLog, Critical, "No GPUs with Vulkan support found");
        return;
    }

    // now get all graphic cards handles
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, surfaceForPresentCheck))
        {
            m_physicalDevice = device;
            m_queueFamilyIndices = FindQueueFamilies(device, surfaceForPresentCheck);

            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(device, &props);
            VE_LOG(VulkanRenderLog, Display, "Selected GPU: {}", props.deviceName);
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        VE_LOG(VulkanRenderLog, Critical, "Failed to find a suitable GPU");
    }
}

void VulkanRenderer::CreateLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value()};

    // here we telling how many queues we want to create and from what family
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // for now empty - later geometry shaders, tesselation etc...
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(kRequiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = kRequiredDeviceExtensions.data();
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Critical, "Failed to create logical device");
        return;
    }

    vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
}

void VulkanRenderer::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to create command pool");
    }
}
#pragma endregion

#pragma region Surface & Swapchain
// using concrete window to render
VkSurfaceKHR VulkanRenderer::CreateSurfaceForHandle(void* nativeWindowHandle) const
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(nativeWindowHandle);

    if (glfwCreateWindowSurface(m_instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to create window surface");
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
        VE_LOG(VulkanRenderLog, Critical, "Failed to create swapchain");
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
            VE_LOG(VulkanRenderLog, Error, "Failed to create image view");
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

#pragma endregion

#pragma region RenderPass & Pipeline
void VulkanRenderer::CreateRenderPass(WindowRenderContext& context)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = context.swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &context.renderPass) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to create render pass");
    }
}

void VulkanRenderer::CreateGraphicsPipeline(WindowRenderContext& context)
{
    auto vertCode = ReadFile(m_gameConfig.shadersPath + m_gameConfig.pipeline.vertShader);
    auto fragCode = ReadFile(m_gameConfig.shadersPath + m_gameConfig.pipeline.fragShader);

    VkShaderModule vertShaderModule = CreateShaderModule(vertCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateInfo.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &context.pipelineLayout) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = context.pipelineLayout;
    pipelineInfo.renderPass = context.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &context.graphicsPipeline) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

VkShaderModule VulkanRenderer::CreateShaderModule(const std::vector<char>& code) const
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to create shader module");
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

std::vector<char> VulkanRenderer::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to open file: {}", filename);
        return {};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
#pragma endregion

#pragma region Framebuffers
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
            VE_LOG(VulkanRenderLog, Error, "Failed to create framebuffer");
        }
    }
}
#pragma endregion

#pragma region Commands & Synchronization
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
        VE_LOG(VulkanRenderLog, Error, "Failed to allocate command buffers");
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
        VE_LOG(VulkanRenderLog, Error, "Failed to create sync objects");
    }
}

void VulkanRenderer::RecordCommandBuffer(WindowRenderContext& context, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to begin recording command buffer");
        return;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = context.renderPass;
    renderPassInfo.framebuffer = context.swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = context.swapchainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.graphicsPipeline);

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

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        VE_LOG(VulkanRenderLog, Error, "Failed to record command buffer");
    }
}
#pragma endregion

#pragma region Queue Family Helpers
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

    std::set<std::string> requiredExtensions(kRequiredDeviceExtensions.begin(), kRequiredDeviceExtensions.end());

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

    // we had kRequiredDeviceExtensions and if requiredExtensions is empty that  - device is good and we can go further
    // for now we use only one extension: VK_KHR_SWAPCHAIN_EXTENSION_NAME

    return requiredExtensions.empty();
}
#pragma endregion

void VulkanRenderer::DrawFrame()
{
    for (auto& [id, context] : m_windowContexts)
    {
        vkWaitForFences(m_device, 1, &context.inFlightFence, VK_TRUE, UINT64_MAX);

        // pass this window draw om that frame
        if (context.framebufferResized)
        {
            if (context.newWidth == 0 || context.newHeight == 0)
            {
                continue;
            }
            RecreateSwapChainForWindow(context, context.newWidth, context.newHeight);
            context.framebufferResized = false;
            continue;
        }

        uint32_t imageIndex;
        VkResult result =
            vkAcquireNextImageKHR(m_device, context.swapchain, UINT64_MAX, context.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // actually we can reach to here but doesn't store actual numbers for width and height - so we'll fill them manually
            int width, height;
            glfwGetFramebufferSize(context.nativeWindowHandle, &width, &height);
            RecreateSwapChainForWindow(context, width, height);
            continue;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            VE_LOG(VulkanRenderLog, Error, "Failed to acquire swapchain image");
            continue;
        }

        vkResetFences(m_device, 1, &context.inFlightFence);

        vkResetCommandBuffer(context.commandBuffers[imageIndex], 0);
        RecordCommandBuffer(context, context.commandBuffers[imageIndex], imageIndex);

        VkSemaphore waitSemaphores[] = {context.imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {context.renderFinishedSemaphore};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &context.commandBuffers[imageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, context.inFlightFence) != VK_SUCCESS)
        {
            VE_LOG(VulkanRenderLog, Error, "Failed to submit draw command buffer");
            continue;
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = {context.swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        VkResult presentResult = vkQueuePresentKHR(m_presentQueue, &presentInfo);

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
        {
            // actually we can reach to here but doesn't store actual numbers for width and height - so we'll fill them manually
            int width, height;
            glfwGetFramebufferSize(context.nativeWindowHandle, &width, &height);
            RecreateSwapChainForWindow(context, width, height);
            continue;
        }
        else if (presentResult != VK_SUCCESS)
        {
            VE_LOG(VulkanRenderLog, Error, "Failed to present");
        }
    }
}

void VulkanRenderer::Shutdown()
{
    vkDeviceWaitIdle(m_device);

    for (auto& [id, context] : m_windowContexts)
    {
        DestroyWindowRenderContext(context);
    }
    m_windowContexts.clear();

    if (m_commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }

    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }

    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::WindowWasResized(int id, int newWidth, int newHeight)
{
    if (m_windowContexts.contains(id))
    {
        m_windowContexts[id].framebufferResized = true;
        m_windowContexts[id].newWidth = newWidth;
        m_windowContexts[id].newHeight = newHeight;
    }
    else
    {
        VE_LOG(VulkanRenderLog, Warning, "Window was resized, but render doesn't store that id");
    }
}

#pragma region Cleanup
void VulkanRenderer::DestroyWindowRenderContext(WindowRenderContext& context)
{
    if (context.imageAvailableSemaphore != VK_NULL_HANDLE) vkDestroySemaphore(m_device, context.imageAvailableSemaphore, nullptr);
    if (context.renderFinishedSemaphore != VK_NULL_HANDLE) vkDestroySemaphore(m_device, context.renderFinishedSemaphore, nullptr);
    if (context.inFlightFence != VK_NULL_HANDLE) vkDestroyFence(m_device, context.inFlightFence, nullptr);

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

    if (context.graphicsPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, context.graphicsPipeline, nullptr);
        context.graphicsPipeline = VK_NULL_HANDLE;
    }

    if (context.pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, context.pipelineLayout, nullptr);
        context.pipelineLayout = VK_NULL_HANDLE;
    }

    if (context.renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(m_device, context.renderPass, nullptr);
        context.renderPass = VK_NULL_HANDLE;
    }

    if (context.swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device, context.swapchain, nullptr);
        context.swapchain = VK_NULL_HANDLE;
    }

    if (context.surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, context.surface, nullptr);
        context.surface = VK_NULL_HANDLE;
    }
}
#pragma endregion
