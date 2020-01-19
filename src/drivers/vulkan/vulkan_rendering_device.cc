#include "Nile/drivers/vulkan/vulkan_rendering_device.hh"
#include "Nile/core/assert.hh"
#include "Nile/core/file_system.hh"
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
    this->createRenderPass();
    this->createDescriptorSetLayout();
    this->createGraphicsPipeline();
    this->createFrameBuffers();
    this->createCommandPool();
    this->createVertexBuffer();
    this->createIndexBuffer();
    this->createUniformBuffers();
    this->createDescriptorPool();
    this->createDescriptorSets();
    this->createCommandBuffers();
    this->createSyncObjects();
  }

  void VulkanRenderingDevice::destory() noexcept {

    this->cleanupSwapChain();

    vkDestroyDescriptorSetLayout( m_logicalDevice, m_descriptorSetLayout, nullptr );

    vkDestroyBuffer( m_logicalDevice, m_indexBuffer, nullptr );
    vkFreeMemory( m_logicalDevice, m_indexBufferMemory, nullptr );

    vkDestroyBuffer( m_logicalDevice, m_vertexBuffer, nullptr );
    vkFreeMemory( m_logicalDevice, m_vertexBufferMemory, nullptr );

    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
      vkDestroySemaphore( m_logicalDevice, m_semaphores.renderingHasFinished[ i ], nullptr );
      vkDestroySemaphore( m_logicalDevice, m_semaphores.iamgeIsAvailable[ i ], nullptr );
      vkDestroyFence( m_logicalDevice, m_inFlightFences[ i ], nullptr );
    }

    vkDestroyCommandPool( m_logicalDevice, m_commandPool, nullptr );

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

  void VulkanRenderingDevice::submitFrame() noexcept {

    // @info: submitFrame will perform every frame the following operations:
    // - aquire an image from the swap chain
    // - execute the command buffer with that image as attachment in the framebuffer
    // - return the image to the swap chain for presentation
    //

    vkWaitForFences( m_logicalDevice, 1, &m_inFlightFences[ m_currentFrame ], VK_TRUE, UINT64_MAX );

    u32 image_index;


    auto result = vkAcquireNextImageKHR( m_logicalDevice, m_swapChain, UINT64_MAX,
                                         m_semaphores.iamgeIsAvailable[ m_currentFrame ],
                                         VK_NULL_HANDLE, &image_index );

    if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
      this->recreateSwapChain();
      return;
    }

    this->updateUniformBuffer( image_index );

    ASSERT_M( result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
              "Failed to acquire swap chain image" );

    // Check if previous frame is using this image ( i.e. there is its fence to wait on)
    if ( m_imagesInFlight[ image_index ] != VK_NULL_HANDLE ) {
      vkWaitForFences( m_logicalDevice, 1, &m_imagesInFlight[ image_index ], VK_TRUE, UINT64_MAX );
    }

    // mark the image as now being in use by this frame
    m_imagesInFlight[ image_index ] = m_inFlightFences[ m_currentFrame ];

    // submitting the command buffer
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {m_semaphores.iamgeIsAvailable[ m_currentFrame ]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_commandBuffers[ image_index ];

    VkSemaphore signal_semaphores[] = {m_semaphores.renderingHasFinished[ m_currentFrame ]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences( m_logicalDevice, 1, &m_inFlightFences[ m_currentFrame ] );

    VK_CHECK_RESULT(
        vkQueueSubmit( m_graphicsQueue, 1, &submit_info, m_inFlightFences[ m_currentFrame ] ) );

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {m_swapChain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR( m_presentQueue, &present_info );

    if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
         m_frambufferResized ) {
      m_frambufferResized = false;
      recreateSwapChain();
    } else if ( result != VK_SUCCESS ) {
      ASSERT_M( false, "Failed to present swap chain image \n" );
    }

    m_currentFrame = ( m_currentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
  }

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

      i32 width, height;
      SDL_GetWindowSize( m_window, &width, &height );

      // VkExtent2D actual_extent = {settings->getWidth(), settings->getHeight()};
      VkExtent2D actual_extent = {static_cast<u32>( width ), static_cast<u32>( height )};

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

  void VulkanRenderingDevice::createRenderPass() noexcept {

    VkAttachmentDescription color_attachment = {};
    color_attachment.format = m_swapChainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // Color depth buffer
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // We don't do anything with stencil data yeat ( maybe we will be using this
    // in the editor )
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VK_CHECK_RESULT(
        vkCreateRenderPass( m_logicalDevice, &render_pass_info, nullptr, &m_renderPass ) );
  }

  void VulkanRenderingDevice::createGraphicsPipeline() noexcept {

    auto vertex_shader_result = FileSystem::readFile( "../assets/vk_shaders/shader.vert.spv" );
    auto fragment_shader_result = FileSystem::readFile( "../assets/vk_shaders/shader.frag.spv" );

    auto vertex_shader_code = std::get_if<std::vector<char>>( &vertex_shader_result );
    auto fragment_shader_code = std::get_if<std::vector<char>>( &fragment_shader_result );

    ASSERT_M( vertex_shader_code,
              errorToString( std::get<ErrorCode>( vertex_shader_result ) ).c_str() );
    ASSERT_M( fragment_shader_code,
              errorToString( std::get<ErrorCode>( fragment_shader_result ) ).c_str() );

    auto vertex_shader_module = createShaderModule( *vertex_shader_code );
    auto fragment_shader_module = createShaderModule( *fragment_shader_code );

    VkPipelineShaderStageCreateInfo vert_shader_stage_create_info = {};
    vert_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_create_info.module = vertex_shader_module;
    vert_shader_stage_create_info.pName = "main";
    // pSpecializationInfo allows you to specify values for shader constants.
    // this make the code more efficient rathen thatn settings constants at runtime
    vert_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {};
    frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_create_info.module = fragment_shader_module;
    frag_shader_stage_create_info.pName = "main";
    frag_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_create_info,
                                                       frag_shader_stage_create_info};

    auto binding_description = Vertex::getBindingDescription();
    auto attribute_descriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount =
        static_cast<u32>( attribute_descriptions.size() );
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // @tip: what kind of gemotetry will be drawn?
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport view_port = {};
    view_port.x = 0.0f;
    view_port.y = 0.0f;
    view_port.width = static_cast<f32>( m_swapChainExtent.width );
    view_port.height = static_cast<f32>( m_swapChainExtent.height );
    view_port.minDepth = 0.0f;
    view_port.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;

    VkPipelineViewportStateCreateInfo view_port_state = {};
    view_port_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    view_port_state.viewportCount = 1;
    view_port_state.pViewports = &view_port;
    view_port_state.scissorCount = 1;
    view_port_state.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // @disabled: alpha blending is didabled
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;


    // the folowing code is to enable alpha blending
    // color_blend_attachment.blendEnable = VK_TRUE;
    // color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    // color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    // color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    // color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[ 0 ] = 0.0f;
    color_blending.blendConstants[ 1 ] = 0.0f;
    color_blending.blendConstants[ 2 ] = 0.0f;
    color_blending.blendConstants[ 3 ] = 0.0f;

    // @brief: we specify here descriptor sets ( uniform buffers ) or push constants
    // to use inside our shaders
    VkPipelineLayoutCreateInfo pipeline_layotu_info = {};
    pipeline_layotu_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layotu_info.setLayoutCount = 1;
    pipeline_layotu_info.pSetLayouts = &m_descriptorSetLayout;
    pipeline_layotu_info.pushConstantRangeCount = 0;
    pipeline_layotu_info.pPushConstantRanges = nullptr;

    VK_CHECK_RESULT( vkCreatePipelineLayout( m_logicalDevice, &pipeline_layotu_info, nullptr,
                                             &m_pipelineLayout ) );

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &view_port_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr;

    pipeline_info.layout = m_pipelineLayout;
    pipeline_info.renderPass = m_renderPass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    VK_CHECK_RESULT( vkCreateGraphicsPipelines( m_logicalDevice, VK_NULL_HANDLE, 1, &pipeline_info,
                                                nullptr, &m_graphicsPipeline ) );


    vkDestroyShaderModule( m_logicalDevice, vertex_shader_module, nullptr );
    vkDestroyShaderModule( m_logicalDevice, fragment_shader_module, nullptr );
  }

  [[nodiscard]] VkShaderModule
  VulkanRenderingDevice::createShaderModule( const std::vector<char> &code ) noexcept {

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const u32 *>( code.data() );

    VkShaderModule shader_module;

    VK_CHECK_RESULT(
        vkCreateShaderModule( m_logicalDevice, &create_info, nullptr, &shader_module ) );

    return shader_module;
  }

  void VulkanRenderingDevice::createFrameBuffers() noexcept {

    m_swapChainFrameBuffers.resize( m_sawapChainImageViews.size() );

    for ( size_t i = 0; i < m_sawapChainImageViews.size(); i++ ) {

      VkImageView attachments[] = {m_sawapChainImageViews[ i ]};

      VkFramebufferCreateInfo framebuffer_info = {};
      framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebuffer_info.renderPass = m_renderPass;
      framebuffer_info.attachmentCount = 1;
      framebuffer_info.pAttachments = attachments;
      framebuffer_info.width = m_swapChainExtent.width;
      framebuffer_info.height = m_swapChainExtent.height;
      framebuffer_info.layers = 1;

      VK_CHECK_RESULT( vkCreateFramebuffer( m_logicalDevice, &framebuffer_info, nullptr,
                                            &m_swapChainFrameBuffers[ i ] ) );
    }
  }

  void VulkanRenderingDevice::createCommandPool() noexcept {

    auto queue_family_indices = findQueueFamilies( m_physicalDevice );

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    // Command buffers are executed by submitting then on one of the device queues, like
    // the graphics or presentaion queues.
    pool_info.queueFamilyIndex = queue_family_indices.graphicsFamily.value();
    pool_info.flags = 0;

    VK_CHECK_RESULT( vkCreateCommandPool( m_logicalDevice, &pool_info, nullptr, &m_commandPool ) );
  }


  void VulkanRenderingDevice::createCommandBuffers() noexcept {

    m_commandBuffers.resize( m_swapChainFrameBuffers.size() );

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = m_commandPool;
    // VK_COMMAND_BUFFER_LEVEL_PRIMARY : can be submitted to a queue for execution, but
    // cannot be called from other command buffers
    // VK_COMMAND_BUFFER_LEVEL_SECONDARY: cannot be submitted to a queue directly, but can
    // be called from primary command buffers
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<u32>( m_commandBuffers.size() );

    VK_CHECK_RESULT(
        vkAllocateCommandBuffers( m_logicalDevice, &alloc_info, m_commandBuffers.data() ) );

    // @ start the recording of the command buffers
    for ( size_t i = 0; i < m_commandBuffers.size(); i++ ) {
      VkCommandBufferBeginInfo begin_info = {};
      begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      begin_info.flags = 0;
      begin_info.pInheritanceInfo = nullptr;

      VK_CHECK_RESULT( vkBeginCommandBuffer( m_commandBuffers[ i ], &begin_info ) );

      VkRenderPassBeginInfo render_pass_info = {};
      render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass_info.renderPass = m_renderPass;
      render_pass_info.framebuffer = m_swapChainFrameBuffers[ i ];
      render_pass_info.renderArea.offset = {0, 0};
      render_pass_info.renderArea.extent = m_swapChainExtent;

      VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
      render_pass_info.clearValueCount = 1;
      render_pass_info.pClearValues = &clear_color;

      // Begin the recording
      vkCmdBeginRenderPass( m_commandBuffers[ i ], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE );

      vkCmdBindPipeline( m_commandBuffers[ i ], VK_PIPELINE_BIND_POINT_GRAPHICS,
                         m_graphicsPipeline );

      VkBuffer vertexBuffers[] = {m_vertexBuffer};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers( m_commandBuffers[ i ], 0, 1, vertexBuffers, offsets );
      vkCmdBindIndexBuffer( m_commandBuffers[ i ], m_indexBuffer, 0, VK_INDEX_TYPE_UINT16 );

      vkCmdBindDescriptorSets( m_commandBuffers[ i ], VK_PIPELINE_BIND_POINT_GRAPHICS,
                               m_pipelineLayout, 0, 1, &m_descriptorSets[ i ], 0, nullptr );

      // Draw a triangle
      // vkCmdDraw( m_commandBuffers[ i ], static_cast<u32>( m_vertices.size() ), 1, 0, 0 );
      vkCmdDrawIndexed( m_commandBuffers[ i ], static_cast<u32>( m_indices.size() ), 1, 0, 0, 0 );

      vkCmdEndRenderPass( m_commandBuffers[ i ] );

      // Finish the recording
      VK_CHECK_RESULT( vkEndCommandBuffer( m_commandBuffers[ i ] ) );
    }
  }

  void VulkanRenderingDevice::createSyncObjects() noexcept {

    m_semaphores.iamgeIsAvailable.resize( MAX_FRAMES_IN_FLIGHT );
    m_semaphores.renderingHasFinished.resize( MAX_FRAMES_IN_FLIGHT );
    m_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );
    m_imagesInFlight.resize( m_swapChainImages.size(), VK_NULL_HANDLE );
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
      VK_CHECK_RESULT( vkCreateSemaphore( m_logicalDevice, &semaphore_info, nullptr,
                                          &m_semaphores.iamgeIsAvailable[ i ] ) );

      VK_CHECK_RESULT( vkCreateSemaphore( m_logicalDevice, &semaphore_info, nullptr,
                                          &m_semaphores.renderingHasFinished[ i ] ) );
      VK_CHECK_RESULT(
          vkCreateFence( m_logicalDevice, &fence_info, nullptr, &m_inFlightFences[ i ] ) );
    }
  }

  void VulkanRenderingDevice::waitIdel() noexcept {
    vkDeviceWaitIdle( m_logicalDevice );
  }

  void VulkanRenderingDevice::recreateSwapChain() noexcept {
    vkDeviceWaitIdle( m_logicalDevice );

    this->cleanupSwapChain();

    this->createSwapChain();
    this->createImageViews();
    this->createRenderPass();
    this->createGraphicsPipeline();
    this->createFrameBuffers();
    this->createUniformBuffers();
    this->createDescriptorPool();
    this->createDescriptorSets();
    this->createCommandBuffers();
  }

  void VulkanRenderingDevice::cleanupSwapChain() noexcept {


    for ( size_t i = 0; i < m_swapChainImages.size(); i++ ) {
      vkDestroyBuffer( m_logicalDevice, m_uniformBuffers[ i ], nullptr );
      vkFreeMemory( m_logicalDevice, m_uniformBuffersMemory[ i ], nullptr );
    }

    vkDestroyDescriptorPool( m_logicalDevice, m_descriptorPool, nullptr );

    for ( const auto &framebuffer : m_swapChainFrameBuffers ) {
      vkDestroyFramebuffer( m_logicalDevice, framebuffer, nullptr );
    }

    vkFreeCommandBuffers( m_logicalDevice, m_commandPool,
                          static_cast<u32>( m_commandBuffers.size() ), m_commandBuffers.data() );

    vkDestroyPipeline( m_logicalDevice, m_graphicsPipeline, nullptr );
    vkDestroyPipelineLayout( m_logicalDevice, m_pipelineLayout, nullptr );
    vkDestroyRenderPass( m_logicalDevice, m_renderPass, nullptr );

    for ( const auto &imageView : m_sawapChainImageViews ) {
      vkDestroyImageView( m_logicalDevice, imageView, nullptr );
    }

    vkDestroySwapchainKHR( m_logicalDevice, m_swapChain, nullptr );
  }

  void VulkanRenderingDevice::setFrameBufferResized( bool value ) noexcept {
    m_frambufferResized = value;
  }

  void VulkanRenderingDevice::createVertexBuffer() noexcept {

    VkDeviceSize buffer_size = sizeof( m_vertices[ 0 ] ) * m_vertices.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    createBuffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  staging_buffer, staging_buffer_memory );

    void *data;
    vkMapMemory( m_logicalDevice, staging_buffer_memory, 0, buffer_size, 0, &data );
    memcpy( data, m_vertices.data(), static_cast<size_t>( buffer_size ) );
    vkUnmapMemory( m_logicalDevice, staging_buffer_memory );


    createBuffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory );

    copyBuffer( staging_buffer, m_vertexBuffer, buffer_size );

    vkDestroyBuffer( m_logicalDevice, staging_buffer, nullptr );
    vkFreeMemory( m_logicalDevice, staging_buffer_memory, nullptr );
  }

  u32 VulkanRenderingDevice::findMemoryType( u32 typeFilter,
                                             VkMemoryPropertyFlags properties ) noexcept {

    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &mem_properties );

    for ( u32 i = 0; i < mem_properties.memoryTypeCount; i++ ) {
      if ( ( typeFilter & ( 1 << i ) ) &&
           ( mem_properties.memoryTypes[ i ].propertyFlags & properties ) == properties ) {
        return i;
      }
    }
    ASSERT_M( false, "Failed to find suitable memory type!" );
    return 0;
  }

  void VulkanRenderingDevice::createBuffer( VkDeviceSize size, VkBufferUsageFlags usage,
                                            VkMemoryPropertyFlags properties, VkBuffer &buffer,
                                            VkDeviceMemory &bufferMemory ) noexcept {

    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT( vkCreateBuffer( m_logicalDevice, &buffer_info, nullptr, &buffer ) );

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements( m_logicalDevice, buffer, &mem_requirements );

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = findMemoryType( mem_requirements.memoryTypeBits, properties );

    // @warning: In reality we are not supposed to actually call vkAllocateMemory for every
    // individual buffer. The maximum number of simulatenous memory allocations is limited
    // by the maxMemoryAllocationsCount physical device limit. The right way to allocate
    // memory for a large number of objects at the same time is to create a custom allocator
    // that splits up a single allocation among many different objects by using offset
    // parameters.
    VK_CHECK_RESULT( vkAllocateMemory( m_logicalDevice, &alloc_info, nullptr, &bufferMemory ) );

    vkBindBufferMemory( m_logicalDevice, buffer, bufferMemory, 0 );
  }

  void VulkanRenderingDevice::copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer,
                                          VkDeviceSize size ) noexcept {

    // @fix: we should create seperate command pool for this short-lived operations
    // we should use the VK_COMMAND_POOL_CREATE_TRANSFER_BIT
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_commandPool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers( m_logicalDevice, &alloc_info, &command_buffer );

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer( command_buffer, &begin_info );

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer( command_buffer, srcBuffer, dstBuffer, 1, &copy_region );
    vkEndCommandBuffer( command_buffer );

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    // we should query a separte queue for transfer operations
    vkQueueSubmit( m_graphicsQueue, 1, &submit_info, VK_NULL_HANDLE );
    vkQueueWaitIdle( m_graphicsQueue );

    vkFreeCommandBuffers( m_logicalDevice, m_commandPool, 1, &command_buffer );
  }

  void VulkanRenderingDevice::createIndexBuffer() noexcept {

    VkDeviceSize buffer_size = sizeof( m_indices[ 0 ] ) * m_indices.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    createBuffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  staging_buffer, staging_buffer_memory );

    void *data;
    vkMapMemory( m_logicalDevice, staging_buffer_memory, 0, buffer_size, 0, &data );
    memcpy( data, m_indices.data(), static_cast<size_t>( buffer_size ) );
    vkUnmapMemory( m_logicalDevice, staging_buffer_memory );

    createBuffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory );

    copyBuffer( staging_buffer, m_indexBuffer, buffer_size );

    vkDestroyBuffer( m_logicalDevice, staging_buffer, nullptr );
    vkFreeMemory( m_logicalDevice, staging_buffer_memory, nullptr );
  }

  void VulkanRenderingDevice::createDescriptorSetLayout() noexcept {

    VkDescriptorSetLayoutBinding ubo_layout_binding = {};
    ubo_layout_binding.binding = 0;
    ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.descriptorCount = 1;
    // Which shader stage will be using the descriptor ? use
    // VK_SHADER_STAGE_ALL_GRAPHICS to use it everywhere
    ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &ubo_layout_binding;

    VK_CHECK_RESULT( vkCreateDescriptorSetLayout( m_logicalDevice, &layout_info, nullptr,
                                                  &m_descriptorSetLayout ) );
  }

  void VulkanRenderingDevice::createUniformBuffers() noexcept {

    VkDeviceSize buffer_size = sizeof( UniformBufferObject );

    m_uniformBuffers.resize( m_swapChainImages.size() );
    m_uniformBuffersMemory.resize( m_swapChainImages.size() );

    for ( size_t i = 0; i < m_swapChainImages.size(); i++ ) {
      createBuffer( buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    m_uniformBuffers[ i ], m_uniformBuffersMemory[ i ] );
    }
  }

  void VulkanRenderingDevice::updateUniformBuffer( u32 imageIndex ) noexcept {

    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    f32 time = std::chrono::duration<f32, std::chrono::seconds::period>( current_time - start_time )
                   .count();

    UniformBufferObject ubo = {};
    ubo.model = glm::rotate( glm::mat4( 1.0f ), time * glm::radians( 90.0f ),
                             glm::vec3( 0.0f, 0.0f, 1.0f ) );

    ubo.view = glm::lookAt( glm::vec3( 2.0f, 2.0f, 2.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ),
                            glm::vec3( 0.0f, 0.0f, 1.0f ) );

    ubo.proj = glm::perspective(
        glm::radians( 45.0f ),
        m_swapChainExtent.width / static_cast<f32>( m_swapChainExtent.height ), 0.1f, 10.0f );

    // GLM has the Y coordinate of the clip coordinates inverted.
    ubo.proj[ 1 ][ 1 ] *= -1;

    void *data;
    vkMapMemory( m_logicalDevice, m_uniformBuffersMemory[ imageIndex ], 0, sizeof( ubo ), 0,
                 &data );
    memcpy( data, &ubo, sizeof( ubo ) );
    vkUnmapMemory( m_logicalDevice, m_uniformBuffersMemory[ imageIndex ] );
  }

  void VulkanRenderingDevice::createDescriptorPool() noexcept {

    VkDescriptorPoolSize pool_size = {};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount = static_cast<u32>( m_swapChainImages.size() );

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = static_cast<u32>( m_swapChainImages.size() );

    VK_CHECK_RESULT(
        vkCreateDescriptorPool( m_logicalDevice, &pool_info, nullptr, &m_descriptorPool ) );
  }

  void VulkanRenderingDevice::createDescriptorSets() noexcept {

    std::vector<VkDescriptorSetLayout> layouts( m_swapChainImages.size(), m_descriptorSetLayout );

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_descriptorPool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>( m_swapChainImages.size() );
    alloc_info.pSetLayouts = layouts.data();

    m_descriptorSets.resize( m_swapChainImages.size() );

    VK_CHECK_RESULT(
        vkAllocateDescriptorSets( m_logicalDevice, &alloc_info, m_descriptorSets.data() ) );

    for ( size_t i = 0; i < m_swapChainImages.size(); i++ ) {

      VkDescriptorBufferInfo buffer_info = {};
      buffer_info.buffer = m_uniformBuffers[ i ];
      buffer_info.offset = 0;
      buffer_info.range = sizeof( UniformBufferObject );

      VkWriteDescriptorSet descriptor_write = {};
      descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptor_write.dstSet = m_descriptorSets[ i ];
      descriptor_write.dstBinding = 0;
      descriptor_write.dstArrayElement = 0;
      descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptor_write.descriptorCount = 1;
      descriptor_write.pBufferInfo = &buffer_info;
      descriptor_write.pImageInfo = nullptr;
      descriptor_write.pTexelBufferView = nullptr;

      vkUpdateDescriptorSets( m_logicalDevice, 1, &descriptor_write, 0, nullptr );
    }
  }

}    // namespace nile

