#pragma once
#include "Render/IRenderer.h"
#include <vulkan/vulkan.h>
#include <array>
#include <vector>
#include <unordered_map>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    GLFWwindow* nativeWindowHandle{nullptr};

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages{};
    std::vector<VkImageView> swapchainImageViews{};
    std::vector<VkFramebuffer> swapchainFramebuffers{};
    std::vector<VkCommandBuffer> commandBuffers{};
    std::vector<VkDescriptorSet> descriptorSets{};
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent{};

    // resized flags
    bool framebufferResized = false;
    uint32_t newWidth = 1;
    uint32_t newHeight = 1;

    std::vector<VkBuffer> uniformBuffers{};
    std::vector<VkDeviceMemory> uniformBuffersMemory{};
    std::vector<void*> uniformBuffersMapped{};

    Math3D::Mat4 cameraView = Math3D::Mat4::Identity();
};

struct GpuMesh
{
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
    uint32_t indexCount = 0;
};

class VulkanRenderer final : public IRenderer
{
public:
    virtual void Init(const GameConfig& config) override;
    virtual void RegisterWindow(int windowId, void* nativeWindowHandle) override;
    virtual void UnregisterWindow(int windowId) override;
    virtual void DrawFrame() override;
    virtual void Shutdown() override;
    virtual void WindowWasResized(int id, int newWidth, int newHeight) override;
    virtual void SetCameraView(int windowId, const Math3D::Mat4& view) override;

    virtual MeshHandle CreateMesh(const MeshDesc& desc) override;
    virtual void DestroyMesh(MeshHandle handle) override;

private:
#pragma region Instance & Device
    void CreateInstance();
    void PickPhysicalDevice(VkSurfaceKHR surfaceForPresentCheck);
    void CreateLogicalDevice();
    void CreateCommandPool();
#pragma endregion

#pragma region Surface & Swapchain
    VkSurfaceKHR CreateSurfaceForHandle(void* nativeWindowHandle) const;
    void CreateSwapchainForWindow(
        WindowRenderContext& context, uint32_t width, uint32_t height, VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
    void CreateImageViews(WindowRenderContext& context);

    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) const;

    void RecreateSwapChainForWindow(WindowRenderContext& context, uint32_t newWidth, uint32_t newHeight);
#pragma endregion

#pragma region RenderPass & Pipeline
    void CreateRenderPass(WindowRenderContext& context);
    void CreateGraphicsPipeline(WindowRenderContext& context);
    VkShaderModule CreateShaderModule(const std::vector<char>& code) const;

    static std::vector<char> ReadFile(const std::string& filename);
#pragma endregion

#pragma region Descriptor
    void CreateDescriptorSetLayout(WindowRenderContext& context);
    void CreateDescriptorPool(WindowRenderContext& context);
    void CreateDescriptorSets(WindowRenderContext& context);
#pragma endregion

#pragma region Framebuffers
    void CreateFramebuffers(WindowRenderContext& context);
#pragma endregion

#pragma region Vertex & Index & Uniform buffers
    void CreateUniformBuffers(WindowRenderContext& context);
    void UpdateUniformBuffer(WindowRenderContext& context);
    void CreateBuffer(
        VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
#pragma endregion

#pragma region Commands & Synchronization
    void CreateCommandBuffers(WindowRenderContext& context);
    void CreateSyncObjects(WindowRenderContext& context);
    void RecordCommandBuffer(WindowRenderContext& context, VkCommandBuffer commandBuffer, uint32_t imageIndex);
#pragma endregion

#pragma region Queue Family Helpers
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
#pragma endregion

#pragma region Cleanup
    void DestroyWindowRenderContext(WindowRenderContext& context);
#pragma endregion

    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    QueueFamilyIndices m_queueFamilyIndices;
    bool m_deviceCreated = false;

    std::unordered_map<int, WindowRenderContext> m_windowContexts;

    std::unordered_map<uint32_t, GpuMesh> m_meshes;
    uint32_t m_nextMeshId = 1;

    GameConfig m_gameConfig;

    template <typename T>
    void CreateDeviceLocalBuffer(const std::vector<T>& data, VkBufferUsageFlagBits usage, VkBuffer& outBuffer, VkDeviceMemory& outMemory);
};

}  // namespace VulkanEngine
