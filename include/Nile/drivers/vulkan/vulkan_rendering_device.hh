#pragma once

#include "Nile/graphics/rendering_device.hh"

#include <SDL2/SDL.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "Nile/log/log.hh"
#include <array>
#include <chrono>
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
  // @temporary
  class AssetManager;

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

    std::shared_ptr<AssetManager> m_assetManager;

    // @temporary: this struct used for debug purpouses here.
    // we will be using the defualt vertex object provided by the engine
    struct Vertex {
      glm::vec2 position;
      glm::vec3 color;
      glm::vec2 texCoord;

      static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription binding_description = {};
        binding_description.binding = 0;
        binding_description.stride = sizeof( Vertex );
        // VK_VERTEX_INPUT_RATE_VERTEX -> move to the next data entry after each vertex
        // VK_VERTEX_INPUT_RATE_INTANCE -> move the the next data entry after each instance
        // used for instanced rendering
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_description;
      }

      static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions = {};

        attribute_descriptions[ 0 ].binding = 0;
        attribute_descriptions[ 0 ].location = 0;
        attribute_descriptions[ 0 ].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[ 0 ].offset = offsetof( Vertex, position );

        attribute_descriptions[ 1 ].binding = 0;
        attribute_descriptions[ 1 ].location = 1;
        attribute_descriptions[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[ 1 ].offset = offsetof( Vertex, color );

        attribute_descriptions[ 2 ].binding = 0;
        attribute_descriptions[ 2 ].location = 2;
        attribute_descriptions[ 2 ].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[ 2 ].offset = offsetof( Vertex, texCoord );

        return attribute_descriptions;
      }
    };

    struct UniformBufferObject {
      alignas( 16 ) glm::mat4 model;
      alignas( 16 ) glm::mat4 view;
      alignas( 16 ) glm::mat4 proj;
    };

    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };

    void initRenderingInternal() noexcept;

    // @move: make this seperate class to handle window
    void initWindow() noexcept;
    // the main vulkan instance
    void createVulkanInstance() noexcept;
    void createSurface() noexcept;
    void setupDebugMessenger() noexcept;
    void
    populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;
    // find supported GPU by Vulkan API
    void pickPhysicalDevice() noexcept;

    [[nodiscard]] QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device ) const noexcept;

    // Create a handle to the GPU ( logical represention of the GPU )
    void createLogicalDevice() noexcept;
    // Create the swapchain, swapchain will own the buffer we will render to
    // before we visualize them on the screen. The swap chain is essentially a queue of
    // images that are waiting to be presented on the screen.
    void createSwapChain() noexcept;
    // An imageview is quite literally a view into an image. It describes how to access an
    // image and which part of the image to access
    void createImageViews() noexcept;
    // With renderpass we tell vulkan about the framebuffer attachments that we will be
    // using while rendering. We specify how many color and depth buffers there will be
    // how many samples to use for each of them and how their contents should be handled.
    void createRenderPass() noexcept;

    void createDescriptorSetLayout() noexcept;
    // Create the main graphics pipeline, where all of the rendered results will be
    // presented to the grahics queue. In this method we specify shader modules and also
    // the rendering topology ( i.e. triangle list )
    void createGraphicsPipeline() noexcept;
    // Create framebuffers where our image will be rendered
    void createFrameBuffers() noexcept;
    // Command pools are memory pools for command bufrer objects
    void createCommandPool() noexcept;
    // Command in Vulkan, like drawing operations and memory transfers are not
    // executed directly using funcitons class, thus we have to record all of the
    // operations we want to perform in a command buffer object.
    void createCommandBuffers() noexcept;
    // Create synchronization primitievs that are used to synchornize operations
    // between GPU-GPU ( semaphores ) and CPU-GPU ( fences )
    void createSyncObjects() noexcept;

    // Recreate swapchain every time the size of the window changes
    // or some other events occures that make the current swap chain uncompatible.
    void recreateSwapChain() noexcept;
    void cleanupSwapChain() noexcept;
    void createVertexBuffer() noexcept;
    void createIndexBuffer() noexcept;
    void createUniformBuffers() noexcept;
    void createDescriptorPool() noexcept;
    void createDescriptorSets() noexcept;

    void updateUniformBuffer( u32 imageIndex ) noexcept;

    void createTextureImage() noexcept;
    void createTextureImageView() noexcept;
    void createTextureSampler() noexcept;

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

    // @fix: make this method static again ( for now only the m_window member is used )
    // when we will make the window object a seperate class
    [[nodiscard]] VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities,
                                               const std::shared_ptr<Settings> &settings ) noexcept;


    // Check if the GPU is suitable for this engine
    bool isDeviceSuitable( VkPhysicalDevice device ) const noexcept;
    bool checkDeviceExtensionSuport( VkPhysicalDevice device ) const noexcept;
    u32 findMemoryType( u32 typeFilter, VkMemoryPropertyFlags properties ) noexcept;

    void createBuffer( VkDeviceSize size, VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags properties, VkBuffer &buffer,
                       VkDeviceMemory &bufferMemory ) noexcept;
    void copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size ) noexcept;


    // those two next methods above belongs to copyBuffer but its also used by imageBuffer
    VkCommandBuffer beginSingleTimeCommands() noexcept;
    void endSingleTimeCommands( VkCommandBuffer commandBuffer ) noexcept;

    void createImage( u32 width, u32 height, VkFormat format, VkImageTiling tiling,
                      VkImageUsageFlags flags, VkMemoryPropertyFlags properties, VkImage &image,
                      VkDeviceMemory &imageMemory ) noexcept;

    void transitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout,
                                VkImageLayout newLayout ) noexcept;

    void copyBufferToImage( VkBuffer buffer, VkImage image, u32 width, u32 height ) noexcept;

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

    VkDescriptorSetLayout m_descriptorSetLayout;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;

    // PIpelineLayout, the uniform and push values that referenced by the shader that
    // can be updated at the draw time
    VkPipelineLayout m_pipelineLayout;

    VkPipeline m_graphicsPipeline;

    VkCommandPool m_commandPool;

    // @fix: this will be moved the seperte VulkanBuffer class
    // with all operations need for the buffer
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    // Driver developers recommend that we should store
    // multiple buffers into a single buffer using offsets in commands.
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;

    VkImage m_textureImage;
    VkDeviceMemory m_textureImageMemory;
    VkImageView m_textureImageView;
    VkSampler m_textureSampler;

    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;

    const std::vector<Vertex> m_vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                                            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                                            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                                            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};

    const std::vector<u16> m_indices = {0, 1, 2, 2, 3, 0};


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
    VulkanRenderingDevice( const std::shared_ptr<Settings> &settings,
                           const std::shared_ptr<AssetManager> &assetManager ) noexcept;

    void initialize() noexcept override;
    void destory() noexcept override;

    // Those both methods are called by the main loop every frame
    void submitFrame() noexcept override;
    void endFrame() noexcept override;
    // this method is caled after the main loop
    void waitIdel() noexcept;

    void setFrameBufferResized( bool value ) noexcept;
  };

}    // namespace nile
