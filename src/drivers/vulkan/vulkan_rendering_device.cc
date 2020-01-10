#include "Nile/drivers/vulkan/vulkan_rendering_device.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/settings.hh"
#include "Nile/core/types.hh"
#include "Nile/drivers/vulkan/vulkan_utils.hh"
#include "Nile/log/log.hh"

#include <SDL2/SDL_vulkan.h>

#include <cstring>
#include <set>

namespace nile {

  VkResult CreateDebugUtilsMessengerEXT( VkInstance instance,
                                         const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                         const VkAllocationCallbacks *pAllocator,
                                         VkDebugUtilsMessengerEXT *pDebugMessenger ) {
    auto func = ( PFN_vkCreateDebugUtilsMessengerEXT )vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT" );
    if ( func != nullptr ) {
      return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks *pAllocator ) {
    auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT )vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT" );
    if ( func != nullptr ) {
      func( instance, debugMessenger, pAllocator );
    }
  }


  VulkanRenderingDevice::VulkanRenderingDevice( const std::shared_ptr<Settings> &settings ) noexcept
      : RenderingDevice( settings ) {}

  void VulkanRenderingDevice::initialize() noexcept {
    this->initRenderingInternal();
    this->initWindow();
    this->createVulkanInstance();
    this->createSurface();
    this->setupDebugMessenger();
    this->pickPhysicalDevice();
    this->createLogicalDevice();
    this->createSwapChain();
    this->createImageViews();
  }

  void VulkanRenderingDevice::destory() noexcept {

    for ( auto imageView : m_sawapChainImageViews ) {
      vkDestroyImageView( m_logicalDevice, imageView, nullptr );
    }

    vkDestroySwapchainKHR( m_logicalDevice, m_swapChain, nullptr );
    vkDestroyDevice( m_logicalDevice, nullptr );

    if ( m_enableValidationLayers ) {
      DestroyDebugUtilsMessengerEXT( m_vulkanInstance, m_debugMessenger, nullptr );
    }

    vkDestroySurfaceKHR( m_vulkanInstance, m_surface, nullptr );
    vkDestroyInstance( m_vulkanInstance, nullptr );
    SDL_DestroyWindow( m_window );
    m_window = nullptr;
    SDL_Quit();
  }

  void VulkanRenderingDevice::initRenderingInternal() noexcept {
    // Initialize SDL2 ( used for window creation and keyboard input)
    ASSERT_M( ( SDL_Init( SDL_INIT_VIDEO ) >= 0 ), "Failed to init SDL" )
  }

  void VulkanRenderingDevice::initWindow() noexcept {

    // Create SDL Window
    m_window = SDL_CreateWindow( m_settings->getWindowTitle().c_str(), SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, m_settings->getWidth(),
                                 m_settings->getHeight(), m_settings->getWindowFlags() );
  }

  void VulkanRenderingDevice::createVulkanInstance() noexcept {

    ASSERT_M( m_enableValidationLayers && checkValidationLayerSupport(),
              "Validaion layers requested, but not available!\n" );

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = m_settings->getWindowTitle().c_str();
    app_info.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    auto extensions = getRequiredExtensions();

    if ( m_enableValidationLayers ) {
      for ( const auto &extension : extensions )
        log::notice( " <~> enabled vulkan extension: %s\n", extension );
    }

    for ( const auto &extension : extensions )
      ASSERT_M( isVkExtensionSupported( extension ), "Extension is not supported\n" );

    create_info.enabledExtensionCount = static_cast<u32>( extensions.size() );
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;

    if ( m_enableValidationLayers ) {
      create_info.enabledLayerCount = static_cast<u32>( m_validationLayers.size() );
      create_info.ppEnabledLayerNames = m_validationLayers.data();

      populateDebugMessengerCreateInfo( debug_create_info );
      create_info.pNext = ( VkDebugUtilsMessengerCallbackDataEXT * )&debug_create_info;

    } else {
      create_info.enabledLayerCount = 0;
      create_info.pNext = nullptr;
    }


    VK_CHECK_RESULT( vkCreateInstance( &create_info, nullptr, &m_vulkanInstance ) );
  }    // namespace nile

  void VulkanRenderingDevice::submitFrame() noexcept {}

  void VulkanRenderingDevice::endFrame() noexcept {
    // swap buffers
  }

  bool VulkanRenderingDevice::checkValidationLayerSupport() const noexcept {
    u32 layer_count;
    vkEnumerateInstanceLayerProperties( &layer_count, nullptr );

    std::vector<VkLayerProperties> available_layers( layer_count );
    vkEnumerateInstanceLayerProperties( &layer_count, available_layers.data() );

    for ( const char *layerName : m_validationLayers ) {

      bool layer_found = false;

      for ( const auto &layerProperties : available_layers ) {
        if ( strcmp( layerName, layerProperties.layerName ) == 0 ) {
          layer_found = true;
          break;
        }
      }

      if ( !layer_found ) {
        return false;
      }
    }

    return true;
  }

  std::vector<const char *> VulkanRenderingDevice::getRequiredExtensions() const noexcept {

    u32 extensions_count = 0;
    ASSERT_M( SDL_Vulkan_GetInstanceExtensions( m_window, &extensions_count, nullptr ),
              "Could not find any SDL2 Instance extensions\n" );
    std::vector<const char *> extensions( extensions_count );
    ASSERT_M( SDL_Vulkan_GetInstanceExtensions( m_window, &extensions_count, extensions.data() ),
              "Could not find any SDL2 Instance extensions\n" );

    if ( m_enableValidationLayers ) {
      extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }
    return extensions;
  }

  VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderingDevice::debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData ) noexcept {


    log::error( "Validation layer: %s\n", pCallbackData->pMessage );

    return VK_FALSE;
  }

  void VulkanRenderingDevice::setupDebugMessenger() noexcept {

    if ( !m_enableValidationLayers )
      return;

    VkDebugUtilsMessengerCreateInfoEXT create_info = {};
    populateDebugMessengerCreateInfo( create_info );

    VK_CHECK_RESULT( CreateDebugUtilsMessengerEXT( m_vulkanInstance, &create_info, nullptr,
                                                   &m_debugMessenger ) );
  }

  void VulkanRenderingDevice::populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept {

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
  }

  void VulkanRenderingDevice::pickPhysicalDevice() noexcept {

    u32 device_count = 0;
    vkEnumeratePhysicalDevices( m_vulkanInstance, &device_count, nullptr );
    ASSERT_M( ( device_count >= 1 ), "Failed to find GPUs with Vulkan Support!\n" );

    std::vector<VkPhysicalDevice> devices( device_count );
    vkEnumeratePhysicalDevices( m_vulkanInstance, &device_count, devices.data() );

    for ( const auto &device : devices ) {
      if ( isDeviceSuitable( device ) ) {
        m_physicalDevice = device;
        break;
      }
    }

    ASSERT_M( ( m_physicalDevice != VK_NULL_HANDLE ), "Failed to find suitable GPU!\n" );
  }

  bool VulkanRenderingDevice::isDeviceSuitable( VkPhysicalDevice device ) const noexcept {

    QueueFamilyIndices indices = findQueueFamilies( device );

    bool extension_is_supported = checkDeviceExtensionSuport( device );

    bool swap_chain_adequate = false;
    if ( extension_is_supported ) {
      auto swap_chain_support = querySwapChainSupport( device );
      swap_chain_adequate =
          !swap_chain_support.formats.empty() && !swap_chain_support.presentModes.empty();
    }


    return indices.isComplete() && extension_is_supported && swap_chain_adequate;
  }

  bool VulkanRenderingDevice::checkDeviceExtensionSuport( VkPhysicalDevice device ) const noexcept {

    u32 extensions_count;
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensions_count, nullptr );

    std::vector<VkExtensionProperties> available_extensions( extensions_count );
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensions_count,
                                          available_extensions.data() );

    std::set<std::string> required_extensions( m_deviceExtensions.begin(),
                                               m_deviceExtensions.end() );

    for ( const auto &extension : available_extensions ) {
      required_extensions.erase( extension.extensionName );
    }

    return required_extensions.empty();
  }


  [[nodiscard]] VulkanRenderingDevice::QueueFamilyIndices
  VulkanRenderingDevice::findQueueFamilies( VkPhysicalDevice device ) const noexcept {

    QueueFamilyIndices indices;

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queue_family_count, nullptr );

    std::vector<VkQueueFamilyProperties> queue_families( queue_family_count );
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queue_family_count, queue_families.data() );

    u32 queue_count = 0;

    for ( const auto &queue_family : queue_families ) {

      if ( queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
        indices.graphicsFamily = queue_count;
      }

      VkBool32 present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR( device, queue_count, m_surface, &present_support );

      if ( present_support ) {
        indices.presentFamily = queue_count;
      }


      if ( indices.isComplete() )
        break;

      queue_count++;
    }

    return indices;
  }

  void VulkanRenderingDevice::createLogicalDevice() noexcept {

    QueueFamilyIndices indices = findQueueFamilies( m_physicalDevice );

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<u32> unique_queue_families = {indices.graphicsFamily.value(),
                                           indices.presentFamily.value()};

    f32 queue_priority = 1.0f;

    for ( u32 queue_family : unique_queue_families ) {
      VkDeviceQueueCreateInfo queue_create_info = {};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = queue_family;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &queue_priority;
      queue_create_infos.push_back( queue_create_info );
    }

    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = static_cast<u32>( queue_create_infos.size() );
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<u32>( m_deviceExtensions.size() );
    create_info.ppEnabledExtensionNames = m_deviceExtensions.data();

    if ( m_enableValidationLayers ) {
      create_info.enabledLayerCount = static_cast<u32>( m_validationLayers.size() );
      create_info.ppEnabledLayerNames = m_validationLayers.data();
    } else {
      create_info.enabledLayerCount = 0;
    }

    VK_CHECK_RESULT( vkCreateDevice( m_physicalDevice, &create_info, nullptr, &m_logicalDevice ) );

    vkGetDeviceQueue( m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue );
    vkGetDeviceQueue( m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue );
  }

  void VulkanRenderingDevice::createSurface() noexcept {
    ASSERT_M( SDL_Vulkan_CreateSurface( m_window, m_vulkanInstance, &m_surface ),
              "Failed to create window surface\n" );
  }

  [[nodiscard]] VulkanRenderingDevice::SwapChainSupportDetails
  VulkanRenderingDevice::querySwapChainSupport( VkPhysicalDevice device ) const noexcept {

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, m_surface, &details.capabilities );

    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &format_count, nullptr );

    if ( format_count != 0 ) {
      details.formats.resize( format_count );
      vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &format_count,
                                            details.formats.data() );
    }

    u32 present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &present_mode_count, nullptr );

    if ( present_mode_count != 0 ) {
      details.presentModes.resize( present_mode_count );
      vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &present_mode_count,
                                                 details.presentModes.data() );
    }

    return details;
  }
  [[nodiscard]] VkSurfaceFormatKHR VulkanRenderingDevice::chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats ) noexcept {

    for ( const auto &available_format : availableFormats ) {
      if ( available_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
           available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
        return available_format;
      }
    }

    // If the above fails, return the first available format ( in most cases this is ok
    // but we could write a rankning system to choose the format)
    return availableFormats[ 0 ];
  }

  [[nodiscard]] VkPresentModeKHR VulkanRenderingDevice::chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes ) noexcept {

    // four possible presentation modes available in vulkan:
    // 1.) VK_PRESENT_MODE_IMMEDIATE_KHR : images are submited and transfered to
    //     the screen right away, which may resulting in tearing.
    // 2.) VK_PRESENT_MODE_FIFO_KHR : The swap chain is a queue where the display is
    //     take an image form the front of the queue, and the rendering image is pushed
    //     to the back of the queue. If the queue is full then the program has to wait.
    //     this is most similar to vertical sync.
    // 3.) VK_PRESENT_MODE_FIFO_RELAXED_KHR: The mode is only differs from the previous one
    //     in that if the application is late and the queue was empty at the last
    //     vertical blank, instead of waiting the image is transfered right away when in
    //     it finialy arives. this may cause visible tearing.
    // 4.) VK_PRESENT_MODE_MAILBOX_KHR: this is another variation of the second mode
    //     Instead of blocking the application when the queue is full, the images that are
    //     already qued are simply replaced with the newer ones. This mode can be used to
    //     implement triple buffering.

    for ( const auto &available_present_mode : availablePresentModes ) {
      if ( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR ) {
        return available_present_mode;
      }
    }

    // the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available, so in case of
    // failure we return this
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  [[nodiscard]] VkExtent2D
  VulkanRenderingDevice::chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities,
                                           const std::shared_ptr<Settings> &settings ) noexcept {

    if ( capabilities.currentExtent.width != std::numeric_limits<u32>::max() ) {
      return capabilities.currentExtent;
    } else {
      VkExtent2D actual_extent = {settings->getWidth(), settings->getHeight()};

      actual_extent.width = std::clamp( actual_extent.width, capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width );
      actual_extent.height = std::clamp( actual_extent.height, capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height );

      return actual_extent;
    }
  }

  void VulkanRenderingDevice::createSwapChain() noexcept {

    auto swap_chain_support = querySwapChainSupport( m_physicalDevice );

    auto surface_format = chooseSwapSurfaceFormat( swap_chain_support.formats );
    auto present_mode = chooseSwapPresentMode( swap_chain_support.presentModes );
    auto extent = chooseSwapExtent( swap_chain_support.capabilities, m_settings );

    u32 image_count = swap_chain_support.capabilities.minImageCount + 1;

    if ( swap_chain_support.capabilities.maxImageCount > 0 &&
         image_count > swap_chain_support.capabilities.maxImageCount ) {
      image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    // we are going to render directly to images, that's why we are using
    // color attachment, if we want to do some post-procssing we would use
    // VK_IMAGE_USAGE_TRANSFER_DST_BIT
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = findQueueFamilies( m_physicalDevice );
    u32 queue_family_indices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if ( indices.graphicsFamily != indices.presentFamily ) {
      create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      create_info.queueFamilyIndexCount = 2;
      create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
      create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      create_info.queueFamilyIndexCount = 0;
      create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    // We should use this when we will recreate the swapchain, in case of window resizing
    create_info.oldSwapchain = nullptr;

    VK_CHECK_RESULT( vkCreateSwapchainKHR( m_logicalDevice, &create_info, nullptr, &m_swapChain ) );

    vkGetSwapchainImagesKHR( m_logicalDevice, m_swapChain, &image_count, nullptr );
    m_swapChainImages.resize( image_count );
    vkGetSwapchainImagesKHR( m_logicalDevice, m_swapChain, &image_count, m_swapChainImages.data() );

    m_swapChainImageFormat = surface_format.format;
    m_swapChainExtent = extent;
  }

  void VulkanRenderingDevice::createImageViews() noexcept {

    m_sawapChainImageViews.resize( m_swapChainImages.size() );

    for ( size_t i = 0; i < m_swapChainImages.size(); i++ ) {

      VkImageViewCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      create_info.image = m_swapChainImages[ i ];
      create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      create_info.format = m_swapChainImageFormat;

      create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

      create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      create_info.subresourceRange.baseMipLevel = 0;
      create_info.subresourceRange.levelCount = 1;
      create_info.subresourceRange.baseArrayLayer = 0;
      create_info.subresourceRange.layerCount = 1;

      VK_CHECK_RESULT( vkCreateImageView( m_logicalDevice, &create_info, nullptr,
                                          &m_sawapChainImageViews[ i ] ) );
    }
  }

}    // namespace nile

