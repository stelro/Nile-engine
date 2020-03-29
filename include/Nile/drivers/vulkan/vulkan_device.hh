#pragma once

#include "Nile/core/types.hh"
#include "Nile/drivers/vulkan/vulkan_buffer.hh"

#include <vulkan/vulkan.h>

#include <vector>

namespace nile {

  class VulkanDevice {
  private:
#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif

    // Physical device, actually our handle to the GPU
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    // Logical device to interface with the physical device
    VkDevice m_logicalDevice;

    // Queue family properties of the physical device
    std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;

    // Features of the physical device that an application can use to check if a
    // feature is supported
    VkPhysicalDeviceFeatures m_features;

    // Features that have been enabled for that physical device
    VkPhysicalDeviceFeatures m_enabledFeatures;

    // Validations layers that are enabled by this device
    std::vector<const char *> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

    // Default command pool for graphics queue family index
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    struct QueueFamilyIndices {
      u32 graphics;
      u32 transfer;
      u32 compute;
      u32 present;

      bool isComplete() const noexcept {
        return graphics && present;
      }

    } m_queueFamilyIndices;


  public:
    VulkanDevice( VkPhysicalDevice physicalDevice ) noexcept;

    ~VulkanDevice() noexcept;

    operator VkDevice() noexcept;

    VkResult createLogicalDevice( VkPhysicalDeviceFeatures enabledFeatures,
                                  std::vector<const char *> enabledExtensions,
                                  bool useSwapChain = true,
                                  VkQueueFlags queueFalgBits = VK_QUEUE_GRAPHICS_BIT |
                                                               VK_QUEUE_TRANSFER_BIT ) noexcept;

    [[nodiscard]] VkDevice getDevice() const noexcept {
      return m_logicalDevice;
    }

    [[nodiscard]] QueueFamilyIndices getQueueFamilyIndices() const noexcept {
      return m_queueFamilyIndices;
    }

    [[nodiscard]] u32 getQueueFamilyIndex( VkQueueFlagBits queueFlag ) const noexcept;

    VkResult createBuffer( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                           VulkanBuffer *buffer, VkDeviceSize size, void *data = nullptr ) noexcept;

    void copyBuffer( VulkanBuffer *dest, VulkanBuffer *src, VkQueue queue,
                     VkBufferCopy *copyRegion = nullptr ) const noexcept;

    [[nodiscard]] u32 getMemoryType( u32 typeFilter,
                                     VkMemoryPropertyFlags memoryPropertyFlags ) const noexcept;

    [[nodiscard]] VkCommandPool
    createCommandPool( u32 queueFamilyIndex,
                       VkCommandPoolCreateFlags createFlags =
                           VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) noexcept;

    [[nodiscard]] VkCommandBuffer createCommandBuffer( VkCommandBufferLevel level,
                                                       VkCommandPool pool,
                                                       bool begin = false ) const noexcept;
    [[nodiscard]] VkCommandBuffer createCommandBuffer( VkCommandBufferLevel level,
                                                       bool begin = false ) const noexcept;

    void flushCommandBuffer( VkCommandBuffer buffer, VkQueue queue, VkCommandPool pool,
                             bool free = true ) const noexcept;
    void flushCommandBuffer( VkCommandBuffer buffer, VkQueue queue, bool free = true ) const
        noexcept;
  };

}    // namespace nile
