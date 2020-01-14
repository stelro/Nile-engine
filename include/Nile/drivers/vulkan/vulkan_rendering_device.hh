#pragma once

#include "Nile/graphics/rendering_device.hh"

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "Nile/log/log.hh"
#include <memory>
#include <optional>
#include <vector>

namespace nile {

  constexpr static const i32 MAX_FRAMES_IN_FLIGHT = 2;

  VkResult CreateDebugUtilsMessengerEXT( VkInstance instance,
                                         const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                         const VkAllocationCallbacks *pAllocator,
                                         VkDebugUtilsMessengerEXT *pDebugMessenger );

  void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks *pAllocator );

  class Settings;

  class VulkanRenderingDevice : public RenderingDevice {

  private:
#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif

  private:
    struct QueueFamilyIndices {
      std::optional<u32> graphicsFamily;
      std::optional<u32> presentFamily;

      bool isComplete() const noexcept {
        return graphicsFamily.has_value() && presentFamily.has_value();
      }
    };

    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };

    void initRenderingInternal() noexcept;

    // @move: make this seperate class to handle window
    void initWindow() noexcept;
    void createVulkanInstance() noexcept;
    void createSurface() noexcept;
    void setupDebugMessenger() noexcept;
    void
    populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;
    void pickPhysicalDevice() noexcept;

    [[nodiscard]] QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device ) const noexcept;

    void createLogicalDevice() noexcept;
    void createSwapChain() noexcept;
    void createImageViews() noexcept;
    void createRenderPass() noexcept;
    void createGraphicsPipeline() noexcept;
    void createFrameBuffers() noexcept;
    void createCommandPool() noexcept;
    void createCommandBuffers() noexcept;
    void createSyncObjects() noexcept;

    // Recreate swapchain every time the size of the window changes
    // or some other events occures that make the current swap chain uncompatible.
    void recreateSwapChain() noexcept;
    void cleanupSwapChain() noexcept;

    [[nodiscard]] VkShaderModule createShaderModule( const std::vector<char> &code ) noexcept;

    [[nodiscard]] SwapChainSupportDetails querySwapChainSupport( VkPhysicalDevice device ) const
        noexcept;
    bool checkValidationLayerSupport() const noexcept;
    std::vector<const char *> getRequiredExtensions() const noexcept;

    // Static functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData ) noexcept;

    [[nodiscard]] static VkSurfaceFormatKHR
    chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR> &availableFormats ) noexcept;

    [[nodiscard]] static VkPresentModeKHR
    chooseSwapPresentMode( const std::vector<VkPresentModeKHR> &availablePresentModes ) noexcept;

    [[nodiscard]] VkExtent2D
    chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities,
                      const std::shared_ptr<Settings> &settings ) noexcept;


    // Check if the GPU is suitable for this engine
    bool isDeviceSuitable( VkPhysicalDevice device ) const noexcept;
    bool checkDeviceExtensionSuport( VkPhysicalDevice device ) const noexcept;

    SDL_Window *m_window = nullptr;
    VkInstance m_vulkanInstance;
    VkDebugUtilsMessengerEXT m_debugMessenger;

    // Physical device, actually our handle to the GPU
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    // Logical device to interface with the physical device
    VkDevice m_logicalDevice;

    // Handle to interface with a graphics queue, which is automatically created
    // along with the logical device
    VkQueue m_graphicsQueue;

    // Handle to presentation queue
    VkQueue m_presentQueue;

    // Window surface ( where rendered iamges are presented at )
    VkSurfaceKHR m_surface;

    // @move(stel): move all swap chain operations to seprate class
    // Active swap chaings
    VkSwapchainKHR m_swapChain;

    // Handles of VkImages in the swapchain
    std::vector<VkImage> m_swapChainImages;

    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;

    VkRenderPass m_renderPass;

    // PIpelineLayout, the uniform and push values that referenced by the shader that
    // can be updated at the draw time
    VkPipelineLayout m_pipelineLayout;

    VkPipeline m_graphicsPipeline;

    VkCommandPool m_commandPool;

    // Sempahores are used for GPU-GPU synchronazation
    struct {
      std::vector<VkSemaphore> iamgeIsAvailable;
      std::vector<VkSemaphore> renderingHasFinished;
    } m_semaphores;

    // Fences are used for CPU-GPU synchronization
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;

    // Keep track of the current frame, so we know which is the right pair
    // of sempahores every frame
    u32 m_currentFrame = 0;

    bool m_frambufferResized = false;

    std::vector<VkImageView> m_sawapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFrameBuffers;
    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<const char *> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char *> m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  public:
    VulkanRenderingDevice( const std::shared_ptr<Settings> &settings ) noexcept;

    void initialize() noexcept override;
    void destory() noexcept override;

    // Those both methods are called by the main loop every frame
    void submitFrame() noexcept override;
    void endFrame() noexcept override;
    // this method is caled after the main loop
    void waitIdel() noexcept;

    void setFrameBufferResized(bool value) noexcept;
  };

}    // namespace nile
