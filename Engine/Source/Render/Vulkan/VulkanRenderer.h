#pragma once
#include "Render/IRenderer.h"
#include <vulkan/vulkan.h>
#include <optional>

namespace VulkanEngine
{

// handful struct for our physical device
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct WindowRenderContext
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent{};
};

class VulkanRenderer final : public IRenderer
{

public:
    virtual void Init() override;
    virtual void RegisterWindow(int windowId, void* nativeWindowHandle) override;
    virtual void UnregisterWindow(int windowId) override;
    virtual void DrawFrame() override;
    virtual void Shutdown() override;

private:
    void CreateInstance();
    VkSurfaceKHR CreateSurfaceForHandle(void* nativeWindowHandle) const;
    void PickPhysicalDevice(VkSurfaceKHR surfaceForPresentCheck);
    void CreateLogicalDevice();
    void CreateSwapchainForWindow(WindowRenderContext& context, uint32_t width, uint32_t height);
    void CreateImageViews(WindowRenderContext& context);
    void DestroyWindowRenderContext(WindowRenderContext& context);
    void CreateRenderPass(WindowRenderContext& context);

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) const;

    static std::vector<char> ReadFile(const std::string& filename);
    VkShaderModule CreateShaderModule(const std::vector<char>& code) const;
    void CreateGraphicsPipeline(WindowRenderContext& context);

    void CreateFramebuffers(WindowRenderContext& context);

    void CreateCommandPool();
    void CreateCommandBuffers(WindowRenderContext& context);
    void CreateSyncObjects(WindowRenderContext& context);
    void RecordCommandBuffer(WindowRenderContext& context, VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    QueueFamilyIndices m_queueFamilyIndices;
    bool m_deviceCreated = false;

    std::unordered_map<int, WindowRenderContext> m_windowContexts;
};

}  // namespace VulkanEngine
