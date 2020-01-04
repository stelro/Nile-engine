#pragma once

#include "Nile/graphics/rendering_device.hh"

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "Nile/log/log.hh"
#include <memory>
#include <optional>
#include <vector>

namespace nile {

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

      bool isComplete() const noexcept {
        return graphicsFamily.has_value();
      }
    };

    void initRenderingInternal() noexcept;

    // @move: make this seperate class to handle window
    void initWindow() noexcept;
    void createVulkanInstance() noexcept;
    void setupDebugMessenger() noexcept;
    void
    populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;
    void pickPhysicalDevice() noexcept;
    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device ) noexcept;

    bool checkValidationLayerSupport() const noexcept;
    std::vector<const char *> getRequiredExtensions() const noexcept;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData ) noexcept;

    bool isDeviceSuitable( VkPhysicalDevice device ) noexcept;

    SDL_Window *m_window = nullptr;
    VkInstance m_vulkanInstance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    std::vector<const char *> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

  public:
    VulkanRenderingDevice( const std::shared_ptr<Settings> &settings ) noexcept;

    void initialize() noexcept override;
    void destory() noexcept override;

    void submitFrame() noexcept override;
    void endFrame() noexcept override;
  };

}    // namespace nile
