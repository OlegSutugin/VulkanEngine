#include "VulkanRenderer.h"
#include "Render/Vertex.h"
#include "Render/Vulkan/VulkanVertexLayout.h"
#include "Core/PlatformDefines.h"
#include "pch.h"
#include "Libraries/Common/Math.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(VulkanRenderLog)
DEFINE_LOG_CATEGORY_STATIC(VulkanRenderDebugLog)

namespace
{
const std::vector<const char*> c_RequiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
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
    CreateDescriptorSetLayout(context);
    CreateUniformBuffers(context);
    CreateDescriptorPool(context);
    CreateDescriptorSets(context);
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
    createInfo.enabledExtensionCount = static_cast<uint32_t>(c_RequiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = c_RequiredDeviceExtensions.data();
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

#pragma region Queue Family Helpers

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

        UpdateUniformBuffer(context);

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

    for (auto& [id, mesh] : m_meshes)
    {
        vkDestroyBuffer(m_device, mesh.indexBuffer, nullptr);
        vkFreeMemory(m_device, mesh.indexBufferMemory, nullptr);
        vkDestroyBuffer(m_device, mesh.vertexBuffer, nullptr);
        vkFreeMemory(m_device, mesh.vertexBufferMemory, nullptr);
    }
    m_meshes.clear();

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

void VulkanRenderer::SetCameraView(int windowId, const Math3D::Mat4& view)
{
    auto it = m_windowContexts.find(windowId);
    if (it != m_windowContexts.end())
    {
        it->second.cameraView = view;
    }
}

MeshHandle VulkanRenderer::CreateMesh(const MeshDesc& desc)
{
    GpuMesh gpuMesh{};
    gpuMesh.indexCount = static_cast<uint32_t>(desc.indices.size());

    CreateDeviceLocalBuffer(desc.vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, gpuMesh.vertexBuffer, gpuMesh.vertexBufferMemory);
    CreateDeviceLocalBuffer(desc.indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, gpuMesh.indexBuffer, gpuMesh.indexBufferMemory);

    uint32_t id = m_nextMeshId++;
    m_meshes[id] = gpuMesh;
    return MeshHandle{id};
}

void VulkanRenderer::DestroyMesh(MeshHandle handle)
{
    auto it = m_meshes.find(handle.id);
    if (it == m_meshes.end()) return;

    vkDeviceWaitIdle(m_device);  // to do lag!!!!!

    vkDestroyBuffer(m_device, it->second.vertexBuffer, nullptr);
    vkFreeMemory(m_device, it->second.vertexBufferMemory, nullptr);
    vkDestroyBuffer(m_device, it->second.indexBuffer, nullptr);
    vkFreeMemory(m_device, it->second.indexBufferMemory, nullptr);

    m_meshes.erase(it);
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

    if (context.descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_device, context.descriptorPool, nullptr);
        context.descriptorPool = VK_NULL_HANDLE;
    }

    if (!context.uniformBuffers.empty() && context.uniformBuffers[0] != VK_NULL_HANDLE)
    {
        vkUnmapMemory(m_device, context.uniformBuffersMemory[0]);
        vkDestroyBuffer(m_device, context.uniformBuffers[0], nullptr);
        vkFreeMemory(m_device, context.uniformBuffersMemory[0], nullptr);
    }

    if (context.descriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_device, context.descriptorSetLayout, nullptr);
        context.descriptorSetLayout = VK_NULL_HANDLE;
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

    context.nativeWindowHandle = nullptr;
}
#pragma endregion
